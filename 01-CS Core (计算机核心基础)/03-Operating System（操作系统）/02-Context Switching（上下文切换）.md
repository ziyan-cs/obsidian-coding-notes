---
tags:
  - cs
  - os
---

> **核心考点**：上下文切换的流程、切换代价、TLB 失效、切换 vs 模式切换的区别

## 上下文切换

CPU 从一个进程/线程切换到另一个进程/线程时，保存当前状态并恢复目标状态的过程。

### 切换内容

```
┌─ 当前进程（Running）──┐      ┌─ 目标进程（Ready）───┐
│  PC（程序计数器）       │      │  PC                   │
│  通用寄存器（EAX,EBX..）│      │  通用寄存器           │
│  栈指针（SP）           │      │  栈指针               │
│  页表基址（CR3）        │      │  页表基址             │
│  FPU/向量寄存器         │  →   │  FPU/向量寄存器       │
│  内核栈                 │      │  内核栈               │
└────────────────────────┘      └──────────────────────┘
         保存到 PCB                         恢复
```

**关键区别：**
- **进程切换**：需要切换页表（CR3），TLB 全部失效
- **线程切换**：同进程内切换不需要换页表
- **模式切换（系统调用）**：不切换进程，仅切换 ring 级别

### 触发场景

1. **时间片耗尽**（时钟中断 → scheduler_tick → schedule）
2. **阻塞操作**（I/O、sleep、锁等待 → 主动调用 schedule）
3. **高优先级进程就绪**（抢占式调度）
4. **系统调用返回时**检查 `need_resched` 标志

---

## 切换代价

```
操作                    ≈ 延迟（现代 CPU）
────────────────────────────────────────
函数调用                   1-2 ns
系统调用                   50-200 ns
进程上下文切换            1-10 μs      ← ★
TLB miss/latency          50-200 ns
一次内存访问 ≈ L3 访问     10-20 ns

进程切换 ≈ 10000-100000 条指令
```

### 直接代价（显性）

```cpp
// 模拟上下文切换开销的测试
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    const int N = 100000;
    
    for (int i = 0; i < N; i++) {
        std::this_thread::yield();  // 主动让出 CPU
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << "Average yield cost: " << ns / N << " ns" << std::endl;
    return 0;
}
```

### 间接代价（隐性）

1. **TLB 失效**：切换页表后 TLB 需重新填充，导致后续内存访问变慢
2. **Cache 污染**：当前缓存的热数据被目标进程的冷数据覆盖
3. **分支预测器失效**：BTB（分支目标缓冲）中的历史记录失效

---

## 上下文切换流程（Linux）

```
进程 A 运行中
     │
     ▼
时钟中断（硬件）→ CPU 进入中断处理
     │
     ▼
保存 A 的寄存器到 A 的 PCB（内核栈）
     │
     ▼
switch_to (汇编):
    ① 保存当前 CPU 寄存器 (push)
    ② 切换内核栈指针 (SP = B's kernel stack)
    ③ 加载 CR3（B 的页表）  ← TLB flush
    ④ 恢复 B 的寄存器 (pop)
     │
     ▼
进程 B 恢复运行
```

**`switch_to` 汇编核心：**

```asm
; x86-64 上下文切换简化
switch_to:
    pushq   %rbp, %rbx, %r12-r15   ; 保存被调用者保存的寄存器
    movq    %rsp, TASK_threadsp(%rdi) ; 保存当前 SP 到 prev PCB
    movq    TASK_threadsp(%rsi), %rsp ; 加载 next SP
    
    ; 切换页表（如果需要）
    movq    TASK_mm(%rsi), %rcx
    movq    %rcx, CR3              ; TLB 全部失效！
    
    popq    %rbp, %rbx, %r12-r15  ; 恢复 next 的寄存器
    ret
```

---

## 减少上下文切换的方法

| 方法 | 原理 | 适用场景 |
|------|------|---------|
| 减少线程数 | 避免过多线程竞争 CPU | CPU 密集型 |
| 异步 I/O（epoll/io_uring） | 用一个线程处理大量事件 | 网络服务器 |
| 协程 | 用户态调度，无需内核切换 | 高并发 I/O |
| 大页（Huge Pages） | 减少 TLB miss | 内存密集型 |
| CPU 亲和性 | 绑定进程到固定核，cache 更热 | 性能敏感路径 |

> **工程要点**：Redis 单线程模型高吞吐的核心原因之一就是避免了上下文切换。Nginx 事件驱动 + 少量 worker 进程大幅降低了切换开销。检测 `vmstat 1` 的 `cs`（context switch）列可知是否切换过度。

---

## 关联笔记

- [OS Overview（操作系统总览）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/00-OS%20Overview（操作系统总览）.md)
- [Process vs Thread（进程与线程）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/01-Process%20vs%20Thread（进程与线程⭐）.md)
- [CPU Scheduling（CPU调度）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/03-CPU%20Scheduling（CPU调度）.md)
- [File System Basics（文件系统基础）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/06-File%20System%20Basics（文件系统基础）.md)
- [What is a Computer（计算机本质）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/01-What%20is%20a%20Computer（计算机本质）.md)
