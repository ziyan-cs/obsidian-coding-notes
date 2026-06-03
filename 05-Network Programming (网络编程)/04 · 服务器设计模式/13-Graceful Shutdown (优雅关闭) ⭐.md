---
tags:
  - network
  - server-design
---

> **核心考点**：信号驱动的关闭流程、graceful period、drain 连接、C++ 服务平滑重启

## 为什么需要优雅关闭

线上服务直接 `kill -9` 会导致：
- 正在处理的请求被中断，客户端收到连接重置
- 数据写一半（日志、DB、消息队列）处于不一致状态
- 连接未正常关闭，对端 TIME_WAIT 堆积
- 注册中心未及时摘除节点，流量仍分发到此实例

**优雅关闭的目标：** 停止接受新请求 → 处理完正在进行的请求 → 释放资源 → 退出。

---

## 信号驱动的关闭流程

### 信号选择

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

## C++ 优雅关闭的实现

### 基本模式：事件循环 + 退出标志

```cpp
#include <csignal>
#include <atomic>

std::atomic<bool> g_stop{false};

extern "C" void signalHandler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        g_stop.store(true, std::memory_order_relaxed);
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
    while (!g_stop.load(std::memory_order_relaxed)) {
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

### 带 graceful timeout 的 RAII 封装

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
        sa.sa_handler = [](int) { instance().stop(); };
        sigemptyset(&sa.sa_mask);
        sigaction(SIGTERM, &sa, nullptr);
        sigaction(SIGINT, &sa, nullptr);
        signal(SIGPIPE, SIG_IGN);
    }

    void onShutdown(Callback cb) { cb_ = std::move(cb); }

    bool isStopping() const { return stopping_.load(std::memory_order_relaxed); }

    void stop() {
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
    std::atomic<bool> stopping_{false};
    std::chrono::seconds timeout_{10s};
    Callback cb_;
};

// 使用
int main() {
    auto& gs = GracefulShutdown::instance();
    gs.init(15s);
    gs.onShutdown([] { /* 强制清理 */ });
    // ... 事件循环中检查 gs.isStopping()
}
```

---

## 服务注册中心的优雅摘除

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

## k8s 环境中的优雅关闭

Kubernetes 删除 Pod 时：

1. 发送 `SIGTERM` 给进程
2. 等待 `terminationGracePeriodSeconds`（默认 30s）
3. 超时未退出 → `SIGKILL`

**Pod 同时从 Service Endpoint 中摘除**（与 SIGTERM 近乎同时），摘除和优雅关闭之间有短暂窗口，需要客户端侧重试逻辑配合。

---

## 常见陷阱

| 陷阱 | 原因 | 解决 |
|------|------|------|
| `write()` 到已关闭连接 | 客户端在对端关闭后继续写 | 检查 `EPIPE`/`SIGPIPE`，忽略 SIGPIPE |
| 信号处理中调用非可重入函数 | `printf`、`malloc` 在信号上下文中不安全 | 信号 handler 只设 `atomic<bool>`，其余在主循环处理 |
| 关闭顺序错误 | 先释放资源再等待请求完成 | 先 stop accept → drain → cleanup |
| 关闭超时未退出 | 某个环节阻塞 | 启动 watchdog 线程，超时强制 `exit()` |

> **工程要点**：优雅关闭是生产级服务的基本要求。核心三原则：1）收到信号后立即停 listen（不接受新连接）；2）给存量请求一个 deadline（通常 10-30s）；3）超时未完成也要强制退出（比无限等待好）。k8s 环境中配合 readiness probe 和 terminationGracePeriodSeconds 一起使用。
