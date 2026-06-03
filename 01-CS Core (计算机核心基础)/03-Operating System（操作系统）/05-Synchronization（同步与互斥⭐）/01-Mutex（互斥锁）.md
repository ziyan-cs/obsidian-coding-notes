---
tags:
  - cs
  - os
  - synchronization
---

> **核心考点**：临界区与竞态条件、锁的实现（Peterson/硬件原子/自旋锁）、互斥锁 vs 自旋锁

## 临界区与竞态条件

```cpp
// 竞态条件示例：两个线程同时增加计数器
int counter = 0;

void thread_func() {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // 非原子操作！
        // 实际是三条指令：
        // LOAD counter → R1
        // ADD  R1, #1
        // STORE R1 → counter
    }
}

// 预期结果：2000000
// 实际结果：≈ 1074521（竞态条件导致更新丢失）
```

**临界区（Critical Section）：** 访问共享资源的代码段，同一时间只允许一个线程进入。

### 临界区三条件

1. **互斥（Mutual Exclusion）**：同时最多一个线程在临界区
2. **前进（Progress）**：无线程在临界区时，想进的线程应能进
3. **有限等待（Bounded Waiting）**：线程不能无限等待

---

## Peterson 算法（软件解）

```cpp
// Peterson 算法（两个线程的互斥，无硬件原子指令）
class PetersonMutex {
    bool flag[2] = {false, false};
    int turn = 0;

public:
    void lock(int id) {
        int other = 1 - id;
        flag[id] = true;           // 表示想进入
        turn = other;              // 让对方优先
        while (flag[other] && turn == other) {
            // 忙等待
        }
    }

    void unlock(int id) {
        flag[id] = false;
    }
};
```

**正确性验证：** 满足互斥、前进、有限等待。但现代 CPU 的乱序执行可能破坏 Peterson 算法（需要 memory barrier）。

---

## 硬件原子操作

### 硬件锁（TSL / XCHG）

```asm
; x86 LOCK 前缀 + XCHG 指令实现互斥
; lock = 0 表示空闲，1 表示占用

acquire_lock:
    mov    eax, 1           ; 设置 1
    xchg   eax, [lock]      ; 原子交换：eax ↔ lock
    test   eax, eax         ; 检查旧值
    jnz    acquire_lock     ; 非 0 说明锁被占用，重试
    ret                     ; 获得锁

release_lock:
    mov    [lock], 0        ; 释放
    ret
```

### C++ 原子操作

```cpp
#include <atomic>
#include <thread>
#include <iostream>

class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // 忙等待
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

int counter = 0;
SpinLock lock;

void safe_increment() {
    for (int i = 0; i < 1000000; i++) {
        std::lock_guard<SpinLock> guard(lock);
        counter++;
    }
}
```

---

## 互斥锁 vs 自旋锁

| 特性 | 互斥锁（Mutex） | 自旋锁（Spinlock） |
|------|----------------|-------------------|
| 等待时 | 线程睡眠（上下文切换） | CPU 循环忙等 |
| 适用场景 | 锁持有时间长 | 锁持有时间极短 |
| 开销 | 切换重（≈μs级），但不占CPU | 无切换，但占 CPU |
| 中断上下文 | 不可用（可能睡眠） | 可用（需关中断） |
| 实现基础 | futex（Linux） | atomic_flag / TSL |

### Linux futex

```c
// futex（Fast Userspace Mutex）—— Linux 互斥锁核心
// 用户态先尝试原子减（无竞争时不进内核）
// 有竞争时才系统调用睡眠

// 简化实现：
void mutex_lock(int *futex) {
    // 尝试在用户态获取锁
    if (atomic_dec_if_positive(futex) >= 0)
        return;  // 获得锁，无需内核调用
    
    // 竞争发生：进入内核等待
    syscall(SYS_futex, futex, FUTEX_WAIT, 0, ...);
}

// mutex 的 FUTEX_WAIT/FUTEX_WAKE 仅在有竞争时执行系统调用 —— 快速路径（无竞争）≈ 用户态原子操作
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 临界区三条件 | 互斥 + 前进 + 有限等待 |
| Peterson 算法的限制 | 不处理乱序执行，需要 memory barrier |
| 自旋锁何时用 | 锁持有时间 < 上下文切换代价（≈ 2 次） |
| 互斥锁的快速路径 | futex 无竞争时仅用户态原子操作，无系统调用 |
| 可重入锁 | 同一线程可多次获取同一锁（需计数） |
| 死锁与锁顺序 | 固定锁获取顺序可避免死锁 |
| `LOCK` 前缀作用 | 锁总线/缓存行，确保多核原子性 |

> **工程要点**：临界区应尽可能小——只保护共享数据，不要在锁内做 I/O。优先用标准库 `std::mutex`（内部已优化，快速路径 ≈ 用户态原子操作，无需自旋锁除非性能分析证明必要）。`lock_guard`/`unique_lock` 确保异常安全（RAII）。
