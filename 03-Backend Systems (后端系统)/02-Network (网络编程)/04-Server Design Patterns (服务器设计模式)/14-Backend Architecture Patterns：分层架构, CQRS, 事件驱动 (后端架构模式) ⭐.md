---
tags:
  - network/server
status: 🌱
---

# Backend Architecture Patterns：分层架构, CQRS, 事件驱动 — 后端架构模式

> [!important] **核心考点**：分层架构、CQRS、event-driven架构、微服务划分原则、C++ 后端项目结构

## 分层架构（Layered Architecture）

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

### C++ 项目目录结构

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

## CQRS（命令查询职责分离）

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

### C++ 实现示例

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

## event-driven架构（Event-Driven）

服务之间通过事件（消息）async通信，不直接调用：

```
服务A ──publish──> Message Queue
                           │
                           ├────consume────> 服务B
                           │
                           └────consume────> 服务C
```

### C++ 事件总线实现（简化）

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

## 微服务划分原则

| 原则 | 说明 | 反面案例 |
|------|------|---------|
| **按业务边界拆分** | 一个服务负责一个完整业务域 | 一个服务做所有事（大泥球） |
| **dedicated DB** | 每个服务拥有自己的数据库 | 多个服务共享一个库 |
| **接口优先** | 先定 proto/API 再实现 | 实现完发现接口不满足调用方 |
| **无同步依赖** | 服务间尽量async解耦 | A -> B -> C 同步链调用 |
| **独立部署** | 每个服务可单独发布 | 修改一个功能需要同时部署 5 个服务 |

---

## 经典 C++ 后端项目架构参考

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

## 经典题型速查

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
