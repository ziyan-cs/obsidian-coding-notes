---
status: stable
confidence: high
content_verified: 2026-09-19
---

> [!abstract] 学习目标：能区分 data race、原子性和跨线程同步，并为 release/acquire 写出完整的读写配对条件。

> [!summary] 核心摘要
>
> atomic 保证单个原子对象的读改写不发生 data race，但不自动让多个变量组成的业务状态保持一致。memory order 描述跨线程可见性与重排约束：默认 `seq_cst` 最容易推理；只有先证明同步关系并测到瓶颈，才考虑 acquire/release 或 relaxed。

# 选择原则

| 需求 | 首选 |
| --- | --- |
| 复杂共享状态 | mutex，先保证不变量 |
| 简单计数或状态位 | atomic，明确读写语义 |
| 发布数据给读线程 | release store + acquire load |
| 纯统计且不参与同步 | relaxed，仍需确认不会依赖顺序 |

> [!warning]- 易错点
> - atomic 不等于 lock-free；用 `is_lock_free()` 查询，不把实现特性当保证。
> - `memory_order_relaxed` 不建立跨变量的同步关系。
> - 把 memory order 当性能开关；错的同步比慢的锁危险得多。

> [!question]- 自测：先回答再展开
> 1. 为什么两个 atomic 变量仍可能无法表达一个一致的业务状态？
> 2. release/acquire 如何发布一段已初始化的数据？
> 3. 何时宁可使用 mutex？

# Atomic & Memory Order (原子操作与内存序)

> [!note] 本节重点：原子操作 vs 锁的性能差异、内存序（Memory Order）控制可见性、无锁编程基础

## std::atomic 基础

```cpp
#include <atomic>

std::atomic<int> counter{0};
// 对共享的普通 int 做未同步的 ++ 会产生 data race；
// 但这里的 counter 是 atomic，fetch_add 和 ++ 都是原子读改写。

// ✅ atomic 的 ++ 是原子的
counter.fetch_add(1);      // 原子自增
counter.fetch_sub(1);      // 原子自减
counter.load();            // 原子加载
counter.store(42);         // 原子存储
counter.exchange(10);      // 原子交换（返回旧值）
counter.compare_exchange_weak(old, new);  // CAS

// 简写运算符：
counter++;                 // 等价于 fetch_add(1)
counter += 5;
```

## atomic 与 mutex 的成本不能一概而论

```cpp
// mutex 保护
std::mutex mtx;
int shared = 0;
void inc_mutex() {
    std::lock_guard lock(mtx);
    ++shared;  // 无竞争时通常在用户态完成；竞争时才可能经 futex 等待/唤醒
}

// atomic：std::atomic 不保证 lock-free，可用 is_lock_free() 查询
std::atomic<int> atomic_shared{0};
void inc_atomic() {
    ++atomic_shared;  // 具体实现依类型、平台和标准库而异
}
```

**测量时区分**：
- mutex：无竞争时，常见实现通常在用户态完成；发生竞争时才可能通过 futex 等机制等待或唤醒。实际成本受平台、实现、竞争和缓存状态影响，必须测量。
- atomic：单个操作省去了显式临界区，但竞争同一缓存行也可能很贵，且某些类型不保证 lock-free。两者的正确性模型不同，不能只比较一次增量的耗时。

## 内存序（Memory Order）— 核心难点

```cpp
// 默认是 std::memory_order_seq_cst；先用它建立正确性基线。
std::atomic<int> a{0}, b{0};
int x = 0, y = 0;

// 六种内存序：
std::memory_order_relaxed;   // 原子性与同一对象修改顺序，非跨线程同步
std::memory_order_consume;   // 规范中的依赖序；主流编译器通常按 acquire 处理，一般不推荐使用
std::memory_order_acquire;   // 保证之后的读取不会重排到此操作之前
std::memory_order_release;   // 保证之前的写入不会重排到此操作之后
std::memory_order_acq_rel;   // acquire + release（用于 read-modify-write）
std::memory_order_seq_cst;   // 顺序一致性（默认，最严格）
```

### 常见场景

```cpp
// 场景 1：只要求原子性，不要求顺序 → relaxed（计数器）
std::atomic<long> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);

// 场景 2：生产者-消费者 → release/acquire（传递数据）
std::atomic<bool> ready{false};
Data data;

// 线程 1（生产者）
data.prepare();                     // 普通写
ready.store(true, std::memory_order_release);  // 释放语义

// 线程 2（消费者）
while (!ready.load(std::memory_order_acquire));  // 获取语义
// ✅ 保证：线程 2 看到 data.prepare() 的所有副作用
process(data);

// 场景 3：默认 seq_cst 便于推理；不保证在所有平台都最慢
flag.store(true);  // 等价于 seq_cst
```

### Acquire-Release 语义图

```text
线程 A（Release）:
  A.write1
  A.write2
  flag.store(true, release)  ← 之前的写不能重排到后面
    
线程 B（Acquire）:
  while (!flag.load(acquire));  ← 之后的读不能重排到前面
  B.read1     ← 保证看到 A.write1/A.write2
  B.read2
```

# CAS 操作 (Compare-Exchange)

```cpp
std::atomic<int> value{0};

// 期望值 passed by reference
int expected = 0;
int desired = 42;

if (value.compare_exchange_weak(expected, desired)) {
    // value == expected(=0) → value 被设置为 desired(=42)
} else {
    // value != expected → expected 被更新为 value 的当前值
}

// compare_exchange_weak vs strong：
// weak: 可能虚假失败（硬件原因），需要循环重试
// strong: 不会虚假失败；成本差异依平台与操作而定
// 通常 CAS 循环中用 weak，单次用 strong

// 以下仅示意 CAS 更新头指针，不构成完整无锁栈：
// 真正的 pop 还需证明 ABA、节点生命周期与进展保证。
void atomic_push(Node* new_head) {
    Node* old_head = head_.load();
    do {
        new_head->next = old_head;
    } while (!head_.compare_exchange_weak(old_head, new_head));
}
```

## atomic 的局限性

```cpp
// ❌ atomic 类型不一定是 lock-free
// 可以通过 is_lock_free() 检查
std::atomic<LargeStruct> big;
if (big.is_lock_free()) {
    // 此 atomic 对象的操作满足该实现的 lock-free 条件
} else {
    // 不满足 lock-free；实现可能使用锁或其他机制，不能指定内部一定是 mutex
}

// ❌ atomic 不支持复合操作（除非用 CAS 循环）
// 不能同时修改两个 atomic 变量
// atomic 不能用于 std::vector 等容器（不可拷贝/移动）
```

> **关键边界**：release 与 acquire 不是任意配对就能同步。acquire 必须在同一原子对象上读到相应 release（或其 release sequence）的值，才建立 `synchronizes-with`，进而让前序普通写对后续普通读可见。

---
