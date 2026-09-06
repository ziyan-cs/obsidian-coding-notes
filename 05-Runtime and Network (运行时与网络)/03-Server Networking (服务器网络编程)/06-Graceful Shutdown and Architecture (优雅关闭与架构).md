---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# 30 秒回答

**核心结论**：学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# Graceful Shutdown (优雅关闭)

> [!note] 本节重点：核心考点：信号驱动的关闭流程、graceful period、drain 连接、C++ 服务平滑重启

# 为什么需要优雅关闭

线上服务直接 `kill -9` 会导致：
- 正在处理的请求被中断，客户端收到连接重置
- 数据写一半（日志、DB、消息队列）处于不一致状态
- 连接未正常关闭，对端 TIME_WAIT 堆积
- 注册中心未及时摘除节点，流量仍分发到此实例

**优雅关闭的目标：** 停止接受新请求 → 处理完正在进行的请求 → 释放资源 → 退出。

---

# 信号驱动的关闭流程

## 信号选择

| 信号 | 来源 | 建议行为 |
|------|------|---------|
| `SIGTERM` (15) | `kill` 命令/k8s pod 删除 | 优雅关闭（默认） |
| `SIGINT` (2) | Ctrl+C | 优雅关闭 |
| `SIGQUIT` (3) | Ctrl+\ | 优雅关闭 + core dump 调试 |
| `SIGUSR1` (10) | 自定义 | 重新打开日志文件（log rotate） |
| `SIGHUP` (1) | 终端断开 | 重载配置 |
| `SIGKILL` (9) | `kill -9` | **无法捕获**，最后手段 |

**C++ 服务器应当捕获：** `SIGTERM`, `SIGINT`, `SIGQUIT` 触发关闭，`SIGHUP` 重载配置，`SIGUSR1` 日志轮转。

---

# C++ 优雅关闭的实现

## 基本模式：事件循环 + 退出标志

```cpp
#include <csignal>
#include <csignal>

volatile sig_atomic_t g_stop = 0;

extern "C" void signalHandler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        g_stop = 1;
    }
}

int main() {
    // 注册信号处理
    struct sigaction sa{};
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
    // SIGQUIT 忽略（用默认 core dump）
    // SIGPIPE 忽略（防止 write 到关闭连接时进程退出）
    signal(SIGPIPE, SIG_IGN);

    // 事件循环
    while (!g_stop) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, 1000);  // 带超时
        // ... 处理事件
    }

    // --- 优雅关闭阶段 ---
    std::cerr << "Shutting down gracefully..." << std::endl;

    // 1. 停止接受新连接
    close(listen_fd);

    // 2. 关闭空闲连接
    for (auto& conn : connections) {
        if (conn.is_idle()) conn.close();
    }

    // 3. 等待正在处理的请求完成（graceful period）
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (hasPendingRequests() && std::chrono::steady_clock::now() < deadline) {
        // 继续处理已有连接的事件
        int n = epoll_wait(epfd, events, MAX_EVENTS, 100);
        // ... 只处理已有连接，不接受新连接
    }

    // 4. 超时未完成 → 强制关闭
    for (auto& conn : connections) {
        if (conn.is_active()) conn.forceClose();
    }

    // 5. 清理资源
    cleanup();

    std::cerr << "Shutdown complete." << std::endl;
    return 0;
}
```

## 带 graceful timeout 的 RAII 封装

```cpp
class GracefulShutdown {
public:
    using Callback = std::function<void()>;

    static GracefulShutdown& instance() {
        static GracefulShutdown inst;
        return inst;
    }

    void init(std::chrono::seconds timeout = 10s) {
        timeout_ = timeout;
        struct sigaction sa{};
        // handler 只通知主循环；不要在信号上下文启动线程或调用 std::function。
        sa.sa_handler = [](int) { stopping_signal_ = 1; };
        sigemptyset(&sa.sa_mask);
        sigaction(SIGTERM, &sa, nullptr);
        sigaction(SIGINT, &sa, nullptr);
        signal(SIGPIPE, SIG_IGN);
    }

    void onShutdown(Callback cb) { cb_ = std::move(cb); }

    static bool signalRequested() { return stopping_signal_ != 0; }
    bool isStopping() const { return stopping_.load(std::memory_order_relaxed); }

    void stop() {  // 仅由正常线程在观察到 stopping_signal_ 后调用
        bool expected = false;
        if (!stopping_.compare_exchange_strong(expected, true))
            return;  // 已在关闭中
        std::thread t([this] {
            auto deadline = std::chrono::steady_clock::now() + timeout_;
            std::this_thread::sleep_until(deadline);
            if (cb_) cb_();      // 超时回调（强制退出）
            std::exit(0);         // 防止卡死
        });
        t.detach();
    }

private:
    inline static volatile sig_atomic_t stopping_signal_ = 0;
    std::atomic<bool> stopping_{false};
    std::chrono::seconds timeout_{10s};
    Callback cb_;
};

// 使用
int main() {
    auto& gs = GracefulShutdown::instance();
    gs.init(15s);
    gs.onShutdown([] { /* 强制清理 */ });
    // 事件循环中：if (GracefulShutdown::signalRequested()) gs.stop();
    // 再根据 gs.isStopping() 停止接入并 drain。
}
```

---

# 服务注册中心的优雅摘除

gRPC/微服务场景下，关闭前需要先从注册中心摘除本节点：

```
关闭流程（含服务发现）：
1. 收到 SIGTERM
2. 从注册中心（etcd/Consul）注销本节点
3. 等待 5-10s（让负载均衡器更新路由表）
4. 关闭 listen fd（停止接受新请求）
5. Drain 已有连接（处理完或超时强制关闭）
6. 释放资源，退出
```

```cpp
void shutdownWithDiscovery() {
    // 1. 摘除
    consulClient.deregister(serviceName, instanceId);

    // 2. 等待 LB 感知
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 3. 停止接受新连接
    reactor.stopAccept();

    // 4. drain
    auto deadline = now() + 10s;
    while (reactor.hasActiveConnections() && now() < deadline) {
        reactor.pollOnce(100ms);  // 继续处理事件
    }
    reactor.forceCloseAll();
    reactor.stop();
}
```

---

# k8s 环境中的优雅关闭

Kubernetes 删除 Pod 时：

1. 发送 `SIGTERM` 给进程
2. 等待 `terminationGracePeriodSeconds`（默认 30s）
3. 超时未退出 → `SIGKILL`

**Pod 同时从 Service Endpoint 中摘除**（与 SIGTERM 近乎同时），摘除和优雅关闭之间有短暂窗口，需要客户端侧重试逻辑配合。

---

# 常见陷阱

| 陷阱 | 原因 | 解决 |
|------|------|------|
| `write()` 到已关闭连接 | 客户端在对端关闭后继续写 | 检查 `EPIPE`/`SIGPIPE`，忽略 SIGPIPE |
| 信号处理中调用非可重入函数 | `printf`、`malloc` 在信号上下文中不安全 | handler 只设 `volatile sig_atomic_t`，其余在主循环处理 |
| 关闭顺序错误 | 先释放资源再等待请求完成 | 先 stop accept → drain → cleanup |
| 关闭超时未退出 | 某个环节阻塞 | 启动 watchdog 线程，超时强制 `exit()` |

> [!tip]- **工程要点**：优雅关闭是生产级服务的基本要求。核心三原则：1）收到信号后立即停 listen（不接受新连接）；2）给存量请求一个 deadline（通常 10-30s）；3）超时未完成也要强制退出（比无限等待好）。k8s 环境中配合 readiness probe 和 terminationGracePeriodSeconds 一起使用。

# 30 秒回答 / 自测

- **30 秒回答**：信号 handler 只置 `volatile sig_atomic_t`，主循环观察到后依次：停 listen → 关闭空闲连接 → 给存量请求 deadline drain → 超时强制关闭 → 清理资源退出。
- **常见误区**：在信号 handler 里做重活（`printf`/`malloc` 等非可重入操作）；先关资源再等请求完成（顺序颠倒）。
- **自测**：1) 为什么信号 handler 里只允许置一个 `volatile sig_atomic_t` 或 `atomic<bool>`？ 2) k8s 里 `SIGTERM` 后最多多久不退出会被强杀？

---

服务器设计实践见 → [Connection Pool Design (连接池设计)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/10-Connection%20Pool%20Design%20(连接池设计)%20⭐.md>) · [Backend Architecture Patterns：分层架构, CQRS, 事件驱动 (后端架构模式)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/14-Backend%20Architecture%20Patterns：分层架构,%20CQRS,%20事件驱动%20(后端架构模式)%20⭐.md>)

---

# Backend Architecture Patterns (后端架构模式)

> [!note] 本节重点：核心考点：分层架构、CQRS、event-driven架构、微服务划分原则、C++ 后端项目结构

# 分层架构（Layered Architecture）

C++ 后端服务最经典的结构，从上到下分层，每层只依赖下层：

```text
┌───────────────────────────────────────────┐
│  Layered Architecture                     │
├───────────────────────────────────────────┤
│  ┌─────────────────────────────────────┐  │
│  │  Presentation Layer                 │  │
│  │  (API Gateway / Nginx)              │  │
│  └──────────────────┬──────────────────┘  │
│                     │                     │
│                     ▼                     │
│  ┌─────────────────────────────────────┐  │
│  │  Business Logic Layer               │  │
│  │  (Service)                          │  │
│  └──────────────────┬──────────────────┘  │
│                     │                     │
│                     ▼                     │
│  ┌─────────────────────────────────────┐  │
│  │  Data Layer                         │  │
│  │  (DB / Cache)                       │  │
│  └─────────────────────────────────────┘  │
└───────────────────────────────────────────┘

┌───────────────────────────────────────────┐     ┌───────────────────────────────────────────┐
│  CQRS                                     │     │  Event-Driven Architecture                │
├───────────────────────────────────────────┤     ├───────────────────────────────────────────┤
│  ┌─────────────────┐  ┌───────────────┐   │     │  ┌──────────┐  ┌───────────┐  ┌─────────┐ │
│  │ Command (Write) │  │ Query (Read)  │   │     │  │Producer  │  │Message Q  │  │Consumer │ │
│  │ Insert/Update/  │  │ Select        │   │     │  │publish   │→ │(Kafka/    │→ │async    │ │
│  │ Delete          │  │               │   │     │  │events    │  │RabbitMQ)  │  │process  │ │
│  └─────────────────┘  └───────────────┘   │     │  └──────────┘  └───────────┘  └─────────┘ │
└───────────────────────────────────────────┘     └───────────────────────────────────────────┘
```

## C++ 项目目录结构

```
server/
├── cmake/                    # CMake 模块
├── src/
│   ├── handler/              # 接入层（HTTP/gRPC handler）
│   │   ├── user_handler.cpp
│   │   └── order_handler.cpp
│   ├── service/              # 业务层
│   │   ├── user_service.cpp
│   │   └── order_service.cpp
│   ├── repository/           # 数据层
│   │   ├── user_repo.cpp
│   │   └── cache_repo.cpp
│   ├── common/               # 公共类型、工具函数
│   │   ├── types.h
│   │   └── error.h
│   └── server.cpp            # main: 初始化、启动、关闭
├── proto/                    # Protobuf 定义
├── test/
│   ├── unit/
│   └── integration/
└── CMakeLists.txt
```

**分层原则：**
- **依赖方向**：外层向内层依赖，内层不依赖外层
- **数据流**：Handler → Service(入参校验) → Repository(数据访问) → DB
- **返回类型**：每一层返回结果/错误，不跨层抛异常（用 `Result<T, Error>` 模式）

---

# CQRS（命令查询职责分离）

将写操作（Command）和读操作（Query）分离到不同的模型：

```
┌─────────────────────────────────────────────────────────┐
│ Client                                                  │
│   │──────> Command (Write) ───────> Write Model         │
│   │                 CreateOrder               (MySQL)   │
│   │                                                     │
│   │                        Async Replication            │
│   │                                 ▲                   │
│   │                                 │                   │
│   │<────── Query (Read) <───── Read Model               │
│   │               GetOrder                     (Redis)  │
└─────────────────────────────────────────────────────────┘
```

## C++ 实现示例

```cpp
// Command（写）
class CreateOrderCommand {
public:
    Result<OrderId> execute(const CreateOrderReq& req) {
        // 1. 校验
        if (!validate(req)) return Error("invalid request");
        // 2. 写主库
        auto order = orderRepo_.save(req);
        // 3. async同步到读模型
        eventBus_.publish(OrderCreated{order});
        return order.id();
    }
};

// Query（读）
class GetOrderQuery {
public:
    Result<OrderView> execute(OrderId id) {
        // 从缓存/只读副本读取
        auto cached = cacheRepo_.get(id);
        if (cached) return *cached;
        auto order = readRepo_.findById(id);  // 从库
        if (!order) return Error("not found");
        cacheRepo_.set(id, *order, 300s);
        return *order;
    }
};
```

**适用场景：** 读写负载差异大、需要为读优化独立 schema、团队规模大需要职责分离。

---

# event-driven架构（Event-Driven）

服务之间通过事件（消息）async通信，不直接调用：

```
服务A ──publish──> Message Queue
                           │
                           ├────consume────> 服务B
                           │
                           └────consume────> 服务C
```

## C++ 事件总线实现（简化）

```cpp
// 事件基类
struct Event { virtual ~Event() = default; };
struct OrderCreated : Event { OrderId id; UserId uid; int64_t amount; };
struct OrderPaid   : Event { OrderId id; };

// 事件总线（单机版）
class EventBus {
    using Handler = std::function<void(const Event&)>;
    std::unordered_map<size_t, std::vector<Handler>> handlers_;

public:
    template<typename E>
    void subscribe(std::function<void(const E&)> handler) {
        size_t type = typeid(E).hash_code();
        handlers_[type].push_back([handler](const Event& e) {
            handler(static_cast<const E&>(e));
        });
    }

    void publish(const Event& e) {
        size_t type = typeid(e).hash_code();
        if (auto it = handlers_.find(type); it != handlers_.end()) {
            for (auto& h : it->second) h(e);
        }
    }
};

// 使用
EventBus bus;
bus.subscribe<OrderCreated>([](const OrderCreated& e) {
    spdlog::info("Order {} created, amount={}", e.id, e.amount);
});
bus.subscribe<OrderCreated>([](const OrderCreated& e) {
    notificationService.send(e.uid, "订单创建成功");
});
bus.publish(OrderCreated{1001, 42, 9900});
```

**生产级选择：** 单机用 EventBus + 线程池，分布式用 Kafka/RabbitMQ。

---

# 微服务划分原则

| 原则 | 说明 | 反面案例 |
|------|------|---------|
| **按业务边界拆分** | 一个服务负责一个完整业务域 | 一个服务做所有事（大泥球） |
| **dedicated DB** | 每个服务拥有自己的数据库 | 多个服务共享一个库 |
| **接口优先** | 先定 proto/API 再实现 | 实现完发现接口不满足调用方 |
| **无同步依赖** | 服务间尽量async解耦 | A -> B -> C 同步链调用 |
| **独立部署** | 每个服务可单独发布 | 修改一个功能需要同时部署 5 个服务 |

---

# 经典 C++ 后端项目架构参考

```
                    ┌───────────────────┐
                    │  Client           │
                    │  (App/Web)        │
                    └────────┬──────────┘
                             │ HTTP/gRPC
                    ┌────────▼──────────┐
                    │  API Gateway      │  ← Nginx/Envoy (rate-limit, auth, route)
                    └────────┬──────────┘
                             │
              ┌──────────────┼──────────────┐
              ▼              ▼              ▼
        ┌────────────┐ ┌───────────┐ ┌────────────┐
        │ User Svc   │ │ Order Svc │ │ Payment Svc│  ← gRPC
        │ (C++)      │ │ (C++)     │ │ (C++)      │
        └─────┬──────┘ └────┬──────┘ └─────┬──────┘
              ▼             ▼              ▼
        ┌────────────┐ ┌───────────┐ ┌────────────┐
        │ User DB    │ │ Order DB  │ │ Payment DB │  ← dedicated DB
        │ (MySQL)    │ │ (MySQL)   │ │ (MySQL)    │
        └─────┬──────┘ └─────┬─────┘ └──────┬─────┘
              │              │              │
              └──────────────┼──────────────┘
                             ▼ async
                    ┌───────────────────┐
                    │  Message Queue    │  ← Kafka (event-driven)
                    └────────┬──────────┘
                             ▼
                    ┌───────────────────┐
                    │  Data Analysis    │
                    │  Notification Svc │
                    └───────────────────┘
```

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| 分层架构优缺点 | 优点：职责清晰、可测试；缺点：层数多时性能损耗 |
| CQRS 适用场景 | 读写不对等、复杂查询、需要独立优化读模型 |
| event-driven vs 同步调用 | event-driven解耦更彻底但最终一致，同步调用更简单但耦合 |
| 微服务如何拆分 | 按业务域、dedicated DB、团队自治、接口契约 |
| C++ 微服务通信 | 首选 gRPC（强类型、流支持），次选 HTTP + JSON |
| 分布式事务方案 | 单体：2PC；微服务：Saga（编排/编排） |

> [!tip]- **工程要点**：C++ 后端服务不要过度设计——先分层架构，大了再拆微服务。gRPC 是 C++ 微服务间通信最佳选择。Message Queue不只是做async，更是服务的"防洪堤"（削峰填谷）。**不要为了微服务而微服务**，3 个以下服务用单体 + 分层就行。

---

生产环境实践见 → [Graceful Shutdown (优雅关闭)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/13-Graceful%20Shutdown%20(优雅关闭)%20⭐.md>) · [Server Performance：Benchmarking with wrk (压测)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/12-Server%20Performance：Benchmarking%20with%20wrk%20(压测)%20⭐.md>)

# 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

# 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **06-Graceful Shutdown and Architecture (优雅关闭与架构)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
