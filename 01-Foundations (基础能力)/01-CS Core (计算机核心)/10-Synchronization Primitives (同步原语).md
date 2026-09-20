---
study_stage: backlog
tags: [cs/os, cpp/concurrency, learning/foundation]
---

> [!abstract] 学习目标
> 从不变量、数据竞争与 happens-before 选择原子、互斥锁、条件变量、信号量和阶段协调工具，并能写出可证明正确的等待代码。

# 先定义要保护的不变量

同步的目标不是“让线程慢一点”，而是保证共享状态满足不变量。例如队列要求 `0 <= size <= capacity`，入队必须同时更新元素、尾指针和大小，使其他线程不能观察到半完成状态。

在 C++ 中，两个线程对同一内存位置进行冲突访问，至少一个是写，且没有 happens-before 关系时会形成 **data race**；程序行为未定义。`volatile` 不提供线程同步，普通 `bool` 也不能安全实现 Peterson 算法。Peterson 算法可用于理论推导，但映射到 C++ 必须使用原子和恰当内存序。

## happens-before 的作用

happens-before 是“某次写对某次读可见且顺序受约束”的语言级关系。典型建立方式包括：

- 同一线程内的 sequenced-before；
- mutex 的 unlock 与随后成功 lock；
- release 原子操作与读取其值的 acquire 操作；
- 线程创建、结束与 `join` 的规定同步关系。

原子只保证该原子对象操作不可撕裂并遵守所选内存序，不自动让一组业务字段成为事务。

# 互斥锁与 RAII

```cpp
std::mutex mu;
Account account;

void deposit(int amount) {
    std::lock_guard<std::mutex> lock(mu);
    account.balance += amount;
    ++account.version;
}
```

锁的粒度应覆盖完整不变量。C++ mutex 有所有权：成功加锁的执行代理负责解锁；优先使用 `lock_guard`、`unique_lock`、`scoped_lock` 让异常路径自动释放。

`std::mutex` 的具体实现由标准库决定。Linux 实现常在无竞争路径使用用户态原子，竞争时借助 futex 睡眠/唤醒，但这不是 C++ 标准保证，也不能据此背固定耗时。

## 自旋还是睡眠

自旋锁等待时占用 CPU，适合临界区极短、不可睡眠且竞争可控的低层场景；互斥锁可让等待者阻塞，适合可能较长的等待。用户态业务代码通常先用标准 mutex，再根据 profiling 证据优化。持锁期间做阻塞 I/O 会显著扩大争用。

# 条件变量：等待状态而非通知

```cpp
std::mutex mu;
std::condition_variable cv;
std::queue<Job> jobs;
bool stopping = false;

Job take() {
    std::unique_lock lock(mu);
    cv.wait(lock, [] { return stopping || !jobs.empty(); });
    if (jobs.empty()) throw Stopped{};
    Job job = std::move(jobs.front());
    jobs.pop();
    return job;
}
```

条件变量没有“保存通知”的资源计数。正确模型是：

1. 用 mutex 保护条件涉及的共享状态；
2. 在锁内检查谓词；
3. `wait` 原子地释放锁并阻塞，醒来后重新持锁；
4. 用循环/谓词重检，处理虚假唤醒和其他线程抢先改变状态。

通知通常放在状态变更后；是否解锁后再通知要依据生命周期和争用分析，不能死背单一写法。

# 其他同步工具

| 工具 | 表达的关系 | 典型用途 | 常见误用 |
|---|---|---|---|
| `std::atomic<T>` | 单对象原子访问与内存序 | 计数、状态位、无锁协议组件 | 把多个字段误当成整体原子 |
| `std::mutex` | 排他所有权 | 保护复合不变量 | 临界区过大、锁顺序混乱 |
| `std::shared_mutex` | 多读者/单写者 | 读多写少且读区足够长 | 忽略公平性与升级问题 |
| `counting_semaphore` | 可消费许可计数 | 容量限制、资源池 | 忘记归还许可、重复释放 |
| `condition_variable` | 等待受锁保护的谓词 | 队列非空、状态改变 | 用 `if` 等待、状态不受同一锁保护 |
| `latch` | 一次性倒计数关卡 | 等待一批任务完成 | 需要重复阶段却继续复用 |
| `barrier` | 多阶段会合点 | 迭代式并行算法 | 参与者退出导致永久等待 |

原子内存序先以 `seq_cst` 和锁建立正确性；只有在证明协议、建立基准并通过压力测试后，才考虑 acquire/release 或 relaxed。弱内存序错误通常无法靠代码直觉发现。

# 诊断与验证

```bash
# Clang/GCC 示例；按项目构建方式调整
c++ -fsanitize=thread -g -O1 race.cpp -pthread
./a.out
```

ThreadSanitizer 擅长发现数据竞争，不证明算法无死锁，也可能不支持某些自定义同步。并发测试还应覆盖高竞争、取消、超时、异常和进程退出。

# 检查理解

1. 为什么 `volatile bool ready` 不能用于线程间发布对象？
2. 条件变量为什么必须重检谓词？
3. 一个 atomic size 为什么不能自动保护队列内部结构？
4. semaphore 与 condition variable 在“状态由谁保存”上有什么区别？

> [!summary] 本篇结论
> 同步从共享不变量和 happens-before 出发：原子适合清晰的单对象协议，mutex 保护复合状态，条件变量等待谓词，semaphore 管理许可。先证明正确，再用数据决定是否需要更低层优化。

## 权威依据

- [C++ draft: data races and happens-before](https://eel.is/c++draft/intro.races)
- [C++ draft: concurrency support](https://eel.is/c++draft/thread)
- [Linux futex(2)](https://man7.org/linux/man-pages/man2/futex.2.html)

下一步：[11-Deadlock Analysis and Recovery (死锁分析与恢复)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/11-Deadlock%20Analysis%20and%20Recovery%20(死锁分析与恢复).md)
