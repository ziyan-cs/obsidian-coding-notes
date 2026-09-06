---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

atomic 保证单个原子对象的读改写不发生 data race，但不自动让多个变量组成的业务状态保持一致。memory order 描述跨线程可见性与重排约束：默认 `seq_cst` 最容易推理；只有先证明同步关系并测到瓶颈，才考虑 acquire/release 或 relaxed。

# 选择原则

| 需求 | 首选 |
| --- | --- |
| 复杂共享状态 | mutex，先保证不变量 |
| 简单计数或状态位 | atomic，明确读写语义 |
| 发布数据给读线程 | release store + acquire load |
| 纯统计且不参与同步 | relaxed，仍需确认不会依赖顺序 |

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- atomic 不等于 lock-free；用 `is_lock_free()` 查询，不把实现特性当保证。
- `memory_order_relaxed` 不建立跨变量的同步关系。
- 把 memory order 当性能开关；错的同步比慢的锁危险得多。

# 自测

1. 为什么两个 atomic 变量仍可能无法表达一个一致的业务状态？
2. release/acquire 如何发布一段已初始化的数据？
3. 何时宁可使用 mutex？

# Atomic & Memory Order (原子操作与内存序)

> [!note] 本节重点：核心考点：原子操作 vs 锁的性能差异、内存序（Memory Order）控制可见性、无锁编程基础

# std::atomic 基础

```cpp
#include <atomic>

std::atomic<int> counter{0};
// 普通 int 的 read-modify-write 不是线程安全的：
// ++counter 在多线程中会出错（load → inc → store 不是原子的）

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

# 为什么 atomic 比 mutex 快？

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
    ++atomic_shared;  // 编译为 lock add 或 CAS 循环，纯用户态指令
}
```

**性能量级**（粗略对比）：
- mutex：无竞争时，常见实现通常在用户态完成；发生竞争时才可能通过 futex 等机制等待或唤醒。实际成本受平台、实现、竞争和缓存状态影响，必须测量。
- atomic：通常在用户态完成，具体开销需以本机测量为准。

# 内存序（Memory Order）— 核心难点

```cpp
// 默认是 std::memory_order_seq_cst（最严格，最慢）
std::atomic<int> a{0}, b{0};
int x = 0, y = 0;

// 六种内存序：
std::memory_order_relaxed;   // 无顺序约束
std::memory_order_consume;   // 规范中的依赖序；主流编译器通常按 acquire 处理，一般不推荐使用
std::memory_order_acquire;   // 保证之后的读取不会重排到此操作之前
std::memory_order_release;   // 保证之前的写入不会重排到此操作之后
std::memory_order_acq_rel;   // acquire + release（用于 read-modify-write）
std::memory_order_seq_cst;   // 顺序一致性（默认，最严格）
```

## 常见场景

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

// 场景 3：全同步 → seq_cst（默认，最易理解但最慢）
flag.store(true);  // 等价于 seq_cst
```

## Acquire-Release 语义图

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
// strong: 不会虚假失败，更贵
// 通常 CAS 循环中用 weak，单次用 strong

// CAS 循环实现无锁栈
void atomic_push(Node* new_head) {
    Node* old_head = head_.load();
    do {
        new_head->next = old_head;
    } while (!head_.compare_exchange_weak(old_head, new_head));
}
```

# atomic 的局限性

```cpp
// ❌ atomic 类型不一定是 lock-free
// 可以通过 is_lock_free() 检查
std::atomic<LargeStruct> big;
if (big.is_lock_free()) {
    // 硬件级别原子操作
} else {
    // 内部用了 mutex！
}

// ❌ atomic 不支持复合操作（除非用 CAS 循环）
// 不能同时修改两个 atomic 变量
// atomic 不能用于 std::vector 等容器（不可拷贝/移动）
```

> **面试重点**：为什么需要内存序？现代 CPU 和编译器会重排指令。`release` 保证之前的写不会被重排到该操作之后；`acquire` 保证之后的读不会被重排到该操作之前。两者配合形成 **happens-before** 关系。

---

无锁结构基于原子操作实现，详见 → [Lock-free Structures Overview (无锁结构概念)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/06-Lock-free%20Structures%20Overview%20(无锁结构概念).md)

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Atomics and Memory Order (原子与内存序)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
