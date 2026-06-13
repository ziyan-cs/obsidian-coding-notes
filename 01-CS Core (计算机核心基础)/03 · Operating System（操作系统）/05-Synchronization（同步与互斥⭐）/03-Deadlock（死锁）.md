---
tags:
  - cs/os
status: 🌱
---

> [!important] **核心考点**：死锁四条件、死锁预防/避免/检测/恢复、银行家算法、资源分配图

## 死锁定义

多个进程因竞争资源而互相等待，导致所有进程无法继续执行的状态。

---

## 死锁四必要条件

**四个条件必须同时满足才会死锁：**

| 条件 | 说明 | 破坏方法 |
|------|------|---------|
| 1. **互斥**（Mutual Exclusion） | 资源一次只能被一个进程使用 | 允许多个进程共享资源（不总是可行） |
| 2. **持有并等待**（Hold and Wait） | 进程持有资源的同时等待其他资源 | 一次性申请所有资源 / 先释放再申请 |
| 3. **不可剥夺**（No Preemption） | 已分配的资源不能强制剥夺 | 可剥夺资源（如 CPU） |
| 4. **循环等待**（Circular Wait） | 存在进程-资源循环等待链 | 按固定顺序申请资源 |

```cpp
// 死锁示例：两个线程以不同顺序获取锁
std::mutex lock1, lock2;

void thread_a() {
    lock1.lock();
    std::this_thread::sleep_for(10ms);  // 增加死锁概率
    lock2.lock();  // 可能死锁！
    lock2.unlock();
    lock1.unlock();
}

void thread_b() {
    lock2.lock();
    std::this_thread::sleep_for(10ms);
    lock1.lock();  // 可能死锁！
    lock1.unlock();
    lock2.unlock();
}

// 修复：固定锁获取顺序（都先 lock1 再 lock2）
```

---

## 死锁处理策略

### 1. 预防（Prevention）

破坏四个条件之一。常用方法—**破坏循环等待**：

```cpp
// 按地址顺序获取锁（避免死锁的通用技巧）
void safe_lock(std::mutex& a, std::mutex& b) {
    if (&a < &b) {  // 按锁地址排序
        a.lock(); b.lock();
    } else {
        b.lock(); a.lock();
    }
}
```

**其他预防方法：**
- 破坏"持有并等待"：`std::lock(a, b)` 一次性锁多个

### 2. 避免（Avoidance）

动态判断，避免进入不安全状态。

### 3. 检测（Detection）

允许死锁发生，定期检测并恢复。

### 4. 恢复（Recovery）

- **终止进程**：杀掉死锁进程（或逐个终止直到环消失）
- **资源抢占**：回滚检查点，剥夺资源

---

## 银行家算法（Banker's Algorithm）

判断系统分配资源后是否处于安全状态。

```cpp
// 银行家算法核心：判断是否存在安全序列
#include <vector>
#include <iostream>

bool is_safe(const std::vector<int>& available,
             const std::vector<std::vector<int>>& allocation,
             const std::vector<std::vector<int>>& need) {
    int n = allocation.size();  // 进程数
    int m = available.size();   // 资源类型数
    
    std::vector<int> work = available;
    std::vector<bool> finish(n, false);
    
    for (int k = 0; k < n; k++) {  // 最多尝试 n 轮
        for (int i = 0; i < n; i++) {
            if (finish[i]) continue;
            
            bool can_alloc = true;
            for (int j = 0; j < m; j++) {
                if (need[i][j] > work[j]) {
                    can_alloc = false;
                    break;
                }
            }
            
            if (can_alloc) {
                // 假设分配并回收
                for (int j = 0; j < m; j++)
                    work[j] += allocation[i][j];
                finish[i] = true;
            }
        }
    }
    
    // 检查是否全部完成
    for (bool f : finish)
        if (!f) return false;
    return true;
}

int main() {
    // 示例：5 个进程，3 种资源
    std::vector<int> available = {3, 3, 2};
    std::vector<std::vector<int>> allocation = {
        {0, 1, 0},  // P0
        {2, 0, 0},  // P1
        {3, 0, 2},  // P2
        {2, 1, 1},  // P3
        {0, 0, 2}   // P4
    };
    std::vector<std::vector<int>> need = {
        {7, 4, 3},  // P0
        {1, 2, 2},  // P1
        {6, 0, 0},  // P2
        {0, 1, 1},  // P3
        {4, 3, 1}   // P4
    };
    
    std::cout << (is_safe(available, allocation, need)
                  ? "Safe" : "Unsafe") << std::endl;
    return 0;
}
```

**安全状态**：存在一个安全序列（进程按此顺序执行都能完成）。

---

## 资源分配图

```text
┌──────────────────────────────────────────────────────┐
│  RESOURCE ALLOCATION GRAPH (DEADLOCK CYCLE)          │
├──────────────────────────────────────────────────────┤
│                                                       │
│   Thread A ────(holds)────→ Resource 1 (Lock)        │
│       ↑                               │              │
│       │                               │              │
│  (waited by)                    (waited by)           │
│       │                               │              │
│       │                               ▼              │
│   Resource 2 (Lock) ←──(holds)──── Thread B          │
│                                                       │
│  Cycle detected: A waits for R2, B waits for R1      │
│  → Deadlock                                           │
└──────────────────────────────────────────────────────┘
```

**检测死锁：** 资源分配图中有环 ⇔ 可能存在死锁（每种资源只有一个实例时，有环 = 死锁；多种实例时需进一步判断）。

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 死锁四条件 | 互斥 + 持有等待 + 不可剥夺 + 循环等待 |
| 预防 vs 避免 | 预防静态破坏条件；避免动态判断安全性 |
| 银行家算法前提 | 需预先知道每个进程的最大需求量（实际中难以满足） |
| 死锁检测 | RAG 有环 + 环上资源均只有一个实例 = 死锁 |
| 活锁 vs 死锁 | 活锁：进程在运行但无进展（如 ELB 退避重试） |
| 饥饿 vs 死锁 | 饥饿：某个进程长时间得不到资源，但其他进程仍可运行 |
| 死锁恢复代价 | 终止进程可能丢失数据，资源抢占需回滚 |

> [!tip]- **工程要点**：实际开发中最实用的死锁预防就是**固定锁顺序**和 `std::lock(a, b)` 批量获取。Linux 内核有 `lockdep` 检测潜在死锁。分布式系统中的死锁更难检测，常用超时 + 重试策略。C++ 中 RAII 包装的 `lock_guard` 在异常时自动解锁，可避免忘记释放导致的隐式死锁。

---


互斥锁与信号量详见 → [Mutex（互斥锁）](/01-CS%20Core%20(计算机核心基础)/03%20·%20Operating%20System（操作系统）/05-Synchronization（同步与互斥⭐）/01-Mutex（互斥锁）.md) · [Semaphore（信号量）](/01-CS%20Core%20(计算机核心基础)/03%20·%20Operating%20System（操作系统）/05-Synchronization（同步与互斥⭐）/02-Semaphore（信号量）.md)
