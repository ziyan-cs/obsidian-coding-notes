---
status: stable
confidence: high
content_verified: 2026-09-19
---

> [!abstract] 学习目标：辨别 CAS 示意代码与可用无锁结构，分别审查线性化点、ABA、回收和进展保证。

> [!summary] 核心摘要
>
> lock-free 表示系统级进展保证，不等于单次操作更快；CAS 仍要处理 ABA、内存序、回收与高竞争，普通业务优先选更易证明正确的锁方案。

# Lock-free Structures Overview (无锁结构概念)

> [!note] 本节重点：无锁编程的基本思想、ABA 问题、CAS 实现、适用与不适用场景

## 什么是无锁（Lock-Free）

```cpp
// 有锁版本
std::mutex mtx;
void push(int val) {
    std::lock_guard lock(mtx);
    // 操作共享数据
}

// 仅示意 CAS 更新头指针；分配器和整条操作的进展保证尚未证明。
std::atomic<Node*> head{nullptr};
void push(int val) {
    Node* new_node = new Node(val);
    Node* old_head = head.load();
    do {
        new_node->next = old_head;
    } while (!head.compare_exchange_weak(old_head, new_node));
}
```

**Lock-Free 的定义**：
- 任意线程挂起不会阻塞其他线程的进度
- 系统中至少有一个线程能在有限步内完成操作

## 无锁栈：先证明回收，再写 pop

常见示意代码会先读取 `head`，再访问 `head->next`，CAS 成功后立刻 `delete head`。这**不是可用的并发栈**：另一个线程可能仍持有旧指针，甚至在 CAS 前就已访问释放后的 `next`。给示例加一句“这里不安全”不足以防止照抄，因此本笔记不提供假装完整的 `pop` 实现。

设计真正的无锁栈需要依次证明：

1. **线性化点（linearization point）**：成功的 CAS 在逻辑上何时完成操作。
2. **对象仍存活**：任何线程解引用节点期间，回收机制必须阻止其释放；可选 hazard pointers、epoch-based reclamation 等，且各有前提。
3. **ABA**：即使地址重新变成旧值，CAS 的成功是否仍代表正确状态。
4. **进展保证**：包含分配、回收、回调与使用的原子类型后，整个操作是否真的满足 lock-free。

学习顺序是先写带互斥锁的正确版本，再对照成熟实现和证明材料；不要把仅有 CAS 的结构直接放进项目。

## ABA 问题

```cpp
// ABA 问题场景：
// 线程 1: 读取 head → Node A
// 线程 2: pop A → push B → push A（内存地址相同，但内容不同）
// 线程 1: CAS 比较 head == A → 成功！但此时 head 指向的是新的 A

// 一种思路：比较时连版本号一起比较，但仍需单独解决安全回收。
struct TaggedPointer {
    Node* ptr;
    uintptr_t tag;  // 递增版本号
};

std::atomic<TaggedPointer> head_;

// 指针可用位数与地址规范会随架构和配置变化，不能假设“高 16 位空闲”。
// std::atomic<std::shared_ptr<T>> 可管理对象生命周期，但不保证 lock-free，
// 也不能代替整个数据结构的 ABA 和进展证明。
```

## 内存管理难题

```cpp
// 无锁结构的最大问题：何时释放内存？

// 线程 A 准备删除 Node
// 线程 B 正持有指向同一个 Node 的指针
// 线程 A delete → 线程 B 访问已释放内存 → 未定义行为

// 解决方案：
// 1. 风险指针（Hazard Pointer）：线程声明正在使用的指针
// 2. RCU（Read-Copy-Update）：延迟回收
// 3. 引用计数 std::shared_ptr 的原子版本
// 4. Epoch-Based Reclamation (EBR)
```

## 何时用无锁？

| 适合无锁 | 不适合无锁 |
|---------|-----------|
| 极高并发，锁成为瓶颈 | 实现复杂度低时 |
| 细粒度操作（push/pop） | 复合操作（需要同时改多个变量）|
| 已有正确性证明与可验证实现 | 缺乏安全回收方案 |
| 操作足够独立，进展要求明确 | 需要多对象原子更新 |

```cpp
// 实际工程中：优先用锁
// 基准测试证实锁是瓶颈后，再考虑无锁
// "Lock-free programming is like a sharp knife — useful but easy to cut yourself"
```

## C++ 中的无锁设施

| 设施 | 说明 |
|------|------|
| `std::atomic<T>` | 原子类型基础 |
| `atomic<T*>::compare_exchange_*` | CAS 操作 |
| `atomic_signal_fence` / `atomic_thread_fence` | 内存栅栏 |
| `std::atomic<std::shared_ptr<T>>` (C++20) | 原子访问 shared_ptr；是否 lock-free 要查询实现 |
| `std::atomic_ref<T>` (C++20) | 非原子对象的原子操作 |

> [!warning] 面试与工程都要把 ABA 和安全回收分开回答：版本计数只能帮助识别状态变化，不能让悬空指针重新安全。先说明不变量和进展保证，再讨论具体实现。

---

# C++ Concurrency and Performance Optimization (C++ 并发性能优化)

> [!note] 本节重点：锁竞争优化、cache line 伪共享、内存序选择、NUMA 感知、perf 性能分析

## 锁竞争优化

高并发场景下锁竞争是最大的性能杀手。下面是优化思路，按性价比排序。

### 1. 缩小临界区

```cpp
// ❌ 差：整个函数加锁
void processOrder(Order& order) {
    lock_guard lock(mtx_);
    order.validate();           // 纯计算，不需锁
    order.calculatePrice();     // 纯计算，不需锁
    orders_.push_back(order);   // 只有这个需要锁
    notifyWatchers(order);      // 通知可能加锁，别嵌套
}

// ✅ 好：只锁必要操作
void processOrder(Order& order) {
    order.validate();
    order.calculatePrice();
    {
        lock_guard lock(mtx_);
        orders_.push_back(std::move(order));
    }
    notifyWatchers(order);
}
```

### 2. 读写锁（shared_mutex）

读多写少的场景用 `shared_mutex`，读不互斥：

```cpp
#include <shared_mutex>

class Cache {
    std::map<int, string> data_;
    mutable std::shared_mutex mtx_;

public:
    string get(int key) const {
        std::shared_lock lock(mtx_);  // 共享锁：多个读可同时进入
        auto it = data_.find(key);
        return it != data_.end() ? it->second : "";
    }

    void set(int key, string val) {
        std::unique_lock lock(mtx_);  // 独占锁：写时阻塞所有读
        data_[key] = std::move(val);
    }
};
```

### 3. 无锁数据结构

只在确实成为瓶颈时使用。参考 `folly::ConcurrentHashMap`。

```cpp
// std::atomic_flag 自旋锁（轻量，适合极短临界区）
class SpinLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire))
            ;  // 自旋等待
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};
```

---

# Cache Line 与伪共享（False Sharing）

## 问题

CPU 缓存以 cache line（通常 64 字节）为单位加载。两个线程修改同一 cache line 中的不同变量 → 各自的缓存行反复失效 → 性能骤降。

```cpp
// ❌ 伪共享：a 和 b 很可能在同一 cache line
struct Data {
    int a;        // 线程 1 频繁写
    int b;        // 线程 2 频繁写
    // ... padding
};
// 线程 1 写 a → 线程 2 的缓存行失效 → 重新加载 → 性能下降 10 倍+
```

## 解决方案：对齐填充

```cpp
// ✅ 对齐到 cache line
struct alignas(64) Data {
    int a;        // 线程 1 写
    char pad[60]; // 填充到 64 字节
};

// 或使用 C++17 的硬编码填充
struct Data {
    alignas(64) std::atomic<int> a;
    alignas(64) std::atomic<int> b;
};
```

> 伪共享损耗取决于写入频率、CPU、缓存行布局与调度，不存在通用倍数。先用基准测试对照，再按平台可用性尝试 `perf c2c` 定位缓存行竞争。

---

## 内存序选择

C++ 内存序规定可依赖的跨线程顺序；它不是“每种内存序固定对应几条 CPU 屏障”。成本取决于架构、编译器和操作类型。先证明正确性，再测量性能。

| 内存序 | 主要保证 | 常见用途 |
|--------|----------|----------|
| `relaxed` | 原子性及同一对象的修改顺序；不建立跨线程同步 | 独立统计计数 |
| `release`/`acquire` | 同一原子对象上，acquire 读到 release 的值或其 release sequence 时建立同步 | 发布数据 |
| `acq_rel` | 在一次读改写操作中兼有两侧约束 | 需要双向同步的 RMW |
| `seq_cst`（默认） | 额外参与所有 seq_cst 操作的单一总序 | 清晰的正确性基线 |

```cpp
// 先明确“发布数据”的协议，再决定是否需要比默认 seq_cst 更弱的内存序。

std::atomic<bool> ready{false};
std::string data;

// 生产者线程
void producer() {
    data = "hello";                   // 普通写
    ready.store(true, std::memory_order_release);  // release：保证之前的写对其他线程可见
}

// 消费者线程
void consumer() {
    while (!ready.load(std::memory_order_acquire)) // acquire：保证看到 release 前的所有写
        ;
    print(data);  // 安全：data 一定已被写入
}
```

**经验法则：** 非必要时使用默认 `seq_cst` 或锁；`acq_rel` 不是所有原子操作都合法或足够的万能选项。放宽内存序前，用 happens-before 证明和并发测试支撑，再测量收益。

---

## 线程池与 task 窃取（Work Stealing）

均匀分配任务可能导致负载不均——某个线程空闲而其他线程繁忙。Work Stealing 允许空闲线程"偷取"其他线程队列尾部的任务。

```cpp
// Work Stealing 线程池核心思想（简化）
class WorkStealingPool {
    struct ThreadQueue {
        std::deque<Task> tasks;
        std::mutex mtx;
    };
    std::vector<ThreadQueue> queues_;
    std::vector<std::thread> threads_;

    bool steal(int tid, Task& t) {
        for (size_t i = 0; i < queues_.size(); i++) {
            int target = (tid + 1 + i) % queues_.size();  // 随机选目标
            std::lock_guard lk(queues_[target].mtx);
            auto& q = queues_[target].tasks;
            if (!q.empty()) {
                t = std::move(q.front());
                q.pop_front();  // 从队列头部偷
                return true;
            }
        }
        return false;
    }
};
```

> C++ 后端项目直接用 `folly::ThreadPoolExecutor`（Meta 出品，生产验证）或 `boost::asio::thread_pool`，不自己造。

---

## NUMA 感知

在多路 NUMA 服务器中，远端内存访问成本可能高于本地；差异受机器拓扑、工作集与测量方式影响。先用 `numactl --hardware` 查看拓扑，再以实际负载测量，不套固定倍数。

```
Socket 0            Socket 1
┌──────────────────────────────────┐        ┌──────────┐
│ Core 0-7 │        │ Core 8-15    │
│ 本地内存   │        │ 本地内存   │
└────┬─────┘        └────┬─────┘
     │                   │
     └───────互联总线─────┘
    访问远端内存 ≈ 1.5x 延迟
```

**C++ NUMA 优化：**
- **线程绑定**：`pthread_setaffinity_np` 绑定线程到特定核心
- **内存分配**：`libnuma` 的 `numa_alloc_local` 分配本地内存
- **分配策略**：`numactl --membind=0 ./server` 只使用 socket 0 内存

```cpp
#include <sched.h>

// 绑定线程到指定 CPU 核心
void bindToCore(int coreId) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreId, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// 每个线程绑定到不同核心
for (int i = 0; i < numThreads; i++) {
    threads_.emplace_back([i] {
        bindToCore(i);  // 线程 i 绑定到核心 i
        // ... 运行
    });
}
```

---

## 性能分析清单

当你的 C++ 后端服务性能不达标，按这个顺序排查：

| 步骤 | 工具 | 做什么 |
|------|------|--------|
| 1. 系统级 | `top`/`htop` | CPU 是否跑满？哪个进程？ |
| 2. CPU 热点 | `perf top` / `perf record` | 哪些函数最耗 CPU？ |
| 3. 锁竞争 | `perf lock` / `heaptrack` | 锁等待时间占比？ |
| 4. 内存 | `valgrind` / `asan` | 有无内存泄漏？ |
| 5. 上下文切换 | `/proc/stat` / `vmstat` | 上下文切换频繁？可能锁竞争 |
| 6. 网络 | `ss -s` / `netstat` | 连接数、重传率 |
| 7. 磁盘 IO | `iostat -x 1` | await 是否过高？ |

---

> [!example]- 题型索引
> | 题型 | 要点 |
> |------|------|
> | 伪共享是什么 | 多线程修改同一 cache line 的不同变量 → 缓存颠簸 |
> | 如何避免伪共享 | `alignas(64)` 对齐到 cache line |
> | 内存序如何选择 | 95% 场景 `acq_rel` 够用，只有队列/计数器才用 `relaxed` |
> | Work Stealing 好处 | 解决线程间负载不均，提高 CPU 利用率 |
> | NUMA 对性能的影响 | 跨 socket 内存访问慢 1.5x，亲和性绑定可缓解 |
> | 性能优化的第一原则 | **先测量，再优化。** 不要凭感觉优化。 |
>

> [!tip]- **工程要点**：大多数性能问题出在锁竞争和 IO 上，不是 CPU。用 `perf` 找到真正的瓶颈再动手。伪共享在 C++ 后端的高并发场景中常见，排查方法：性能计数器下降明显但 CPU 没跑满 → `perf c2c` 检查 cache 冲突。

>
> ---
>
