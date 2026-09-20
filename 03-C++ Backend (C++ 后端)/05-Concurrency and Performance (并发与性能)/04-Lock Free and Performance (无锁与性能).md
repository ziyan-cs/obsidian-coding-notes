---
study_stage: backlog
---

> [!abstract] 学习目标
> 能区分数据竞争、锁竞争和伪共享；能解释 lock-free 的进展保证、CAS 线性化点与内存回收；能用测量证据选择优化方式。

# 先定位瓶颈，再选并发结构

并发性能不是“锁越少越快”。一次请求可能受 I/O、队列排队、串行临界区、内存分配、缓存一致性或 NUMA 远端访存限制。先固定代表性负载，记录吞吐量、p95/p99 延迟、CPU 利用率与错误率，再做单变量对照实验。低 CPU 利用率也不能直接推断为锁竞争：线程可能在等 I/O。

| 现象 | 优先核查 | 可能的下一步 |
| --- | --- | --- |
| 多线程下吞吐不升、线程阻塞多 | 临界区时长、锁等待与持锁栈 | 缩短临界区、分片、减少共享状态 |
| CPU 忙而有效工作少 | 热点函数、CAS 失败、上下文切换 | 降低争用，必要时重新设计数据布局 |
| 不同线程频繁写相邻字段 | 缓存行争用、对象布局 | 分离写热点并复测 |
| 多路机器出现不稳定尾延迟 | CPU/内存所在 NUMA 节点 | 测量亲和性与内存策略 |

`std::shared_mutex` 允许并发读取，但不保证读多写少就一定更快，也不承诺公平；读锁、写锁的成本和饥饿表现需按实现与负载测试。将计算移出临界区时，必须先确认该计算不依赖正在变化的共享数据。移动对象入队后，不应再把移后对象当作原始通知内容使用。

# CAS 是原子步骤，不是完整算法

`compare_exchange_weak(expected, desired)` 比较原子对象与 `expected`；失败时把当前值写回 `expected`。弱版本允许伪失败，通常放进重试循环。成功的 CAS 可作为某次栈头更新的**线性化点**，但并不自动证明整个 `push`/`pop` 都是正确或 lock-free 的。

```cpp
#include <atomic>

struct Node {
    int value;
    Node* next;
};

std::atomic<Node*> head{nullptr};

// 只展示头指针发布；new 可能阻塞，且没有实现安全的并发 pop。
void illustrative_push(int value) {
    Node* node = new Node{value, nullptr};
    Node* observed = head.load(std::memory_order_relaxed);
    do {
        node->next = observed;
    } while (!head.compare_exchange_weak(
        observed, node,
        std::memory_order_release,
        std::memory_order_relaxed));
}
```

这里的 release 让成功发布前写入的 `node->value` 和 `node->next` 可被匹配的 acquire 读取观察到；但它没有解决并发删除。**不要把这段示意当成可部署的栈**：缺少销毁、回收、异常与进展保证。消费者一旦要解引用旧 `head`，另一个线程就可能已经移除并释放它。

## ABA 与安全回收是两道不同的关

假设线程 A 读到头地址 `P`，线程 B 移走该节点，又让头地址重新变成 `P`。A 的 CAS 可在“值仍等于 `P`”时成功，但中间状态已变化，这就是 ABA。给指针加版本计数可帮助识别某些状态变化，**不**会令悬空指针恢复有效；计数也可能回绕，且双字原子操作是否 lock-free 取决于平台。

即使没有 ABA，只要线程 A 在检查 `head` 后读取节点字段，线程 B 便不能提前 `delete` 该节点。Hazard pointer、epoch-based reclamation（EBR）或适用场景下的引用计数可以推迟回收，但每种方案都需证明线程退出、延迟回收、内存上界及进展性质。`std::atomic<std::shared_ptr<T>>` 能使智能指针的访问原子化，却不保证整个数据结构 lock-free；通过 `is_lock_free()` 查询具体实现。

**工程顺序**：先实现和压测有锁的正确基线，再明确线性化点、回收策略和内存序证明。缺少这三项时，“用了 atomic”不是采用无锁结构的理由。

## 进展保证与内存序

- **Wait-free**：每个操作都在有限的自身步骤内结束。
- **Lock-free**：整体系统保证至少有操作持续完成；单个线程仍可能一直重试。
- **Obstruction-free**：某操作独占执行足够久时才保证完成。

这些是算法性质，不等于 `atomic<T>::is_lock_free()` 的单个对象性质。实际路径若包含可能阻塞的分配、回调或回收步骤，不能只凭一次 CAS 宣称整个操作 lock-free。

`memory_order_relaxed` 保留原子性与该对象的修改顺序，却不发布普通数据；release/acquire 用于建立特定发布—读取同步；默认 `seq_cst` 还对 seq_cst 操作提供一致总序。先写出 happens-before 关系，再考虑弱化内存序。`acq_rel` 只适用于读改写操作，绝不是“多数场景通用”的口诀。

# 缓存行争用：伪共享

两个线程各写不同原子变量仍可能争用同一缓存行；这不是 data race，而是缓存一致性层面的性能问题。缓存行大小、相邻对象布局、调度方式都可能改变结果，不能固定写“慢十倍”。

```cpp
#include <atomic>
#include <new>

struct Counters {
    alignas(std::hardware_destructive_interference_size)
        std::atomic<unsigned long long> left{0};
    alignas(std::hardware_destructive_interference_size)
        std::atomic<unsigned long long> right{0};
};
```

C++17 提供 `std::hardware_destructive_interference_size`，但具体编译器/标准库对它的支持和取值需要在目标平台确认。对 ABI 或二进制布局敏感的类型不要盲目使用可变的编译器常量；可改用显式平台配置。先用同负载基准与采样工具（如 Linux 上可用时的 `perf c2c`）验证“字段分离确有收益”。

# 工作窃取与 NUMA：先知道代价

工作窃取（work stealing）让空闲 worker 从其他 worker 的队列取得任务，主要解决负载不均。设计时要定义本地取任务与远程窃取的方向、队列同步、停止协议、异常边界和任务所有权。所谓“随机窃取”不能用固定轮询代码冒充。对当前学习项目，先完成本模块的有界线程池，再以真实任务时长分布判断是否值得增加窃取。

NUMA 机器上，线程访问其他节点的内存可能付出额外成本；但绑核也可能降低调度灵活性或产生负载倾斜。Linux 下先查看 `numactl --hardware` 与 `numastat`，再对比默认策略、亲和性及内存绑定下的尾延迟和吞吐量。不要套用固定的“远端内存慢几倍”。

# 可复现的优化实验

1. 写明假设：例如“锁等待而非业务计算限制吞吐”。
2. 固定输入分布、并发度、编译选项和机器拓扑；先做正确性测试。
3. 采集基线与变更后的多次数据，保留方差和 p95/p99；记录 CPU、等待和分配证据。
4. 若收益不稳定，回到指标与热点，勿为了“无锁”牺牲可维护性或安全性。

规范核对：[原子智能指针的 lock-free 查询](https://eel.is/c++draft/util.smartptr.atomic)、[进展保证](https://eel.is/c++draft/intro.progress)、[缓存干扰常量](https://eel.is/c++draft/hardware.interference)。
