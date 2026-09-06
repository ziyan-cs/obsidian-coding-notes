---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 04-Lock Free and Performance (无锁与性能)

> [!abstract] 阅读定位
>
> 本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## 06-Lock-free Structures Overview (无锁结构概念)

> [!abstract] 核心考点：无锁编程的基本思想、ABA 问题、CAS 实现、适用与不适用场景

## 什么是无锁（Lock-Free）

```cpp
// 有锁版本
std::mutex mtx;
void push(int val) {
    std::lock_guard lock(mtx);
    // 操作共享数据
}

// 无锁版本：用 CAS 原子操作，不依赖锁
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

## 无锁栈（Lock-Free Stack）

```cpp
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
    };
    std::atomic<Node*> head_{nullptr};

public:
    void push(const T& val) {
        Node* node = new Node{val, nullptr};
        node->next = head_.load();
        while (!head_.compare_exchange_weak(node->next, node))
            ;  // CAS 循环
    }

    bool pop(T& result) {
        Node* old_head = head_.load();
        while (old_head && 
               !head_.compare_exchange_weak(old_head, old_head->next))
            ;
        if (!old_head) return false;
        result = old_head->data;
        delete old_head;  // 注意：这里不安全（后面讲 ABA）
        return true;
    }
};
```

## ABA 问题

```cpp
// ABA 问题场景：
// 线程 1: 读取 head → Node A
// 线程 2: pop A → push B → push A（内存地址相同，但内容不同）
// 线程 1: CAS 比较 head == A → 成功！但此时 head 指向的是新的 A

// 解决方案：带上版本号（Double-width CAS / tagged pointer）
struct TaggedPointer {
    Node* ptr;
    uintptr_t tag;  // 递增版本号
};

std::atomic<TaggedPointer> head_;

// 实际代码中常用：
// - 在 x86_64 上利用指针的高 16 位存 tag（指针只有 48 位有效）
// - 或用 std::atomic<std::shared_ptr<T>> (C++20)
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
| 实时系统（不能容忍等待） | T 的拷贝/移动开销大 |
| 设计简单清晰 | 需要严格的内存序保证 |

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
| `std::atomic<shared_ptr<T>>` (C++20) | 无锁引用计数（可能）|
| `std::atomic_ref<T>` (C++20) | 非原子对象的原子操作 |

> **面试重点**：ABA 问题是必考题。说出 ABA 的含义 + 版本号方案 = 加分。不用深入 hazard pointer 细节，但要能说出"无锁编程最大的挑战是内存回收"。

---

原子操作与内存序是无锁编程的基础，详见 → [Atomic & Memory Order (原子操作与内存序)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/04-Atomic%20&%20Memory%20Order%20(原子操作与内存序)%20⭐.md)

---

## 07-C++ Concurrency and Performance Optimization (C++ 并发性能优化)

> [!abstract] 核心考点：锁竞争优化、cache line 伪共享、内存序选择、NUMA 感知、perf 性能分析

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

## Cache Line 与伪共享（False Sharing）

### 问题

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

### 解决方案：对齐填充

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

> 实测：伪共享的代码在 8 核并发下比对齐版本慢 5-15 倍。排查工具：`perf c2c`（Linux 5.0+）。

---

## 内存序选择

C++ 内存序不是"越强越安全"，越强意味着越多的 CPU 屏障：

| 内存序 | CPU 开销 | 保证 |
|--------|---------|------|
| `relaxed` | 0（无 barrier） | 只保证原子性，不保证顺序 |
| `acquire`/`release` | 轻量 | 成对使用保证 happens-before |
| `acq_rel` | 中等 | acquire + release |
| `seq_cst`（默认） | 最重 | 全局顺序一致（x86 上 ≈ acq_rel） |

```cpp
// 95% 场景：用 acquire/release 就够了
// 不需要默认的 seq_cst

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

**经验法则：** 除非你是并发库作者，否则用 `acq_rel`/`seq_cst` 通常不会错，性能差异在高竞争下才明显。先跑对，再优化。

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

现代多路服务器（如 Intel 双路/四路）中，访问本地内存 vs 远端内存延迟差异可达 **1.5-2 倍**。

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

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 伪共享是什么 | 多线程修改同一 cache line 的不同变量 → 缓存颠簸 |
| 如何避免伪共享 | `alignas(64)` 对齐到 cache line |
| 内存序如何选择 | 95% 场景 `acq_rel` 够用，只有队列/计数器才用 `relaxed` |
| Work Stealing 好处 | 解决线程间负载不均，提高 CPU 利用率 |
| NUMA 对性能的影响 | 跨 socket 内存访问慢 1.5x，亲和性绑定可缓解 |
| 性能优化的第一原则 | **先测量，再优化。** 不要凭感觉优化。 |

> [!tip]- **工程要点**：大多数性能问题出在锁竞争和 IO 上，不是 CPU。用 `perf` 找到真正的瓶颈再动手。伪共享在 C++ 后端的高并发场景中常见，排查方法：性能计数器下降明显但 CPU 没跑满 → `perf c2c` 检查 cache 冲突。

---

原子操作与内存序选择详见 → [Atomic & Memory Order (原子操作与内存序)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/04-Atomic%20&%20Memory%20Order%20(原子操作与内存序)%20⭐.md)