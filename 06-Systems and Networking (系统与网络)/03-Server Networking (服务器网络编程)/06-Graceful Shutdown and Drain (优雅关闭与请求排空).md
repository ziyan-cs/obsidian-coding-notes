---
study_stage: backlog
tags: [backend/server, architecture]
---

> [!abstract] 学习目标
> 把“收到停止信号”展开为停止接流量、排空进行中请求、关闭依赖与超时强退的完整生命周期。


> [!note] 本节重点：信号驱动的关闭流程、graceful period、drain 连接、C++ 服务平滑重启

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

Kubernetes 删除 Pod 时，宽限计时先开始；若配置了 `preStop`，kubelet 会先执行它，然后由容器运行时向容器主进程发送终止信号。`terminationGracePeriodSeconds` 默认 30 秒，但它覆盖 `preStop` 与应用排空的总过程，**不是收到 SIGTERM 后再额外获得 30 秒**；期限届满可能被强制终止。

控制面并行更新 EndpointSlice：终止中的 endpoint 的 `ready` 会变为 false，但负载均衡器、客户端连接池与传播延迟并不保证瞬间停止流量。先标记应用不再接新业务、限制排空时间，再处理已有连接；同时测试老连接复用和晚到请求。具体行为按部署平台与 [Kubernetes Pod termination flow](https://kubernetes.io/docs/concepts/workloads/pods/pod-lifecycle/) 核对。

---

> [!warning]- 易错点
> | 陷阱 | 原因 | 解决 |
> |------|------|------|
> | `write()` 到已关闭连接 | 对端已关闭但本端继续发送 | 处理 `EPIPE`，Linux 可按需使用 `MSG_NOSIGNAL`；全局忽略 `SIGPIPE` 前评估整个进程 |
> | 信号处理中调用非可重入函数 | `printf`、`malloc` 在信号上下文中不安全 | handler 只设 `volatile sig_atomic_t`，其余在主循环处理 |
> | 关闭顺序错误 | 先释放资源再等待请求完成 | 先 stop accept → drain → cleanup |
> | 关闭超时未退出 | 某个环节阻塞 | 分阶段设置 deadline、记录未完成工作，让进程/平台按明确策略退出 |
>

> [!tip]- **工程要点**：关闭流程要有总期限和分阶段预算。信号 handler 内只做允许的极简通知（例如设置 `volatile sig_atomic_t` 标志或使用经核对的 async-signal-safe 机制），在主循环停止接收新工作、排空在途请求并清理依赖。`std::atomic<bool>` 不能不加条件地当作任意平台的 signal-handler 安全替代品。

下一步：[07-Backend Architecture Patterns (后端架构模式)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/07-Backend%20Architecture%20Patterns%20(后端架构模式).md)
