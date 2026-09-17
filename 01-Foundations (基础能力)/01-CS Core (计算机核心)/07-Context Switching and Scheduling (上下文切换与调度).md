---
status: stable
confidence: high
verified: 2026-09-17
tags: [cs/os, learning/foundation]
---

> [!abstract] 学习目标
> 理解上下文切换保存了什么、为什么昂贵，以及调度算法如何在响应时间、吞吐、公平与开销之间权衡。

# Context Switching (上下文切换)

> [!note] 本节重点：上下文切换的流程、切换代价、TLB 失效、切换 vs 模式切换的区别

## 上下文切换

CPU 从一个进程/线程切换到另一个进程/线程时，保存当前状态并恢复目标状态的过程。

### 切换内容

```
┌───────────── Running Process ──────────┐     ┌────────────── Ready Process ──────────┐
│ Program Counter (PC)                   │     │ Program Counter (PC)                  │
│ General Purpose Registers (EAX, EBX…)  │     │ General Purpose Registers             │
│ Stack Pointer (SP)                     │     │ Stack Pointer (SP)                    │
│ Page Table Base Register (CR3)         │     │ Page Table Base Register (CR3)        │
│ FPU / Vector Registers                 │  →  │ FPU / Vector Registers                │
│ Kernel Stack                           │     │ Kernel Stack                          │
└────────────────────────────────────────┘     └───────────────────────────────────────┘
              Save to PCB                                     Restore from PCB
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
进程上下文切换              1-10 μs      ← ★
TLB miss/latency          50-200 ns
一次内存访问 ≈ L3 访问      10-20 ns

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

```text
Process A                 Kernel (Scheduler)          Process B
    │                         │                          │
    ├── User-mode execution   │                          │
    │                         │                          │
    ├── Interrupt / Syscall ─→│                          │
    │                         │                          │
    │                         ├── Save A's registers     │
    │                         │   to PCB_A               │
    │                         │                          │
    │                         ├── Switch page table /    │
    │                         │   address space          │
    │                         │   (TLB flush)            │
    │                         │                          │
    │                         ├── Load B's registers     │
    │                         │   from PCB_B             │
    │                         │                          │
    │                         ├── Return to user mode ──→│
    │                         │                          │
    │                         │                          ├── Resume B execution
    │                         │                          │
    │                         │                          │
    │                       Context switch ≈ 1-10μs      │
    │                  (cache pollution dominates cost)  │
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

> [!tip]- **工程要点**：Redis 单线程模型高吞吐的核心原因之一就是避免了上下文切换。Nginx 事件驱动 + 少量 worker 进程大幅降低了切换开销。检测 `vmstat 1` 的 `cs`（context switch）列可知是否切换过度。

---

# CPU Scheduling (CPU 调度)

> [!note] 本节重点：调度算法（FCFS/SJF/RR/MLFQ）、调度时机、CFS（完全公平调度）、优先级与时间片

## 调度目标

| 场景 | 目标 | 策略 |
|------|------|------|
| 批处理系统 | 高吞吐量、低周转时间 | FCFS, SJF |
| 交互式系统 | 低响应时间 | RR, MLFQ |
| 实时系统 | 可预测性、满足截止时间 | 优先级调度, EDF |

### 评价指标

- **周转时间** = 完成时间 − 到达时间（关注整体效率）
- **响应时间** = 首次运行 − 到达时间（关注交互体验）
- **等待时间** = 等待 CPU 总时间（关注公平性）
- **吞吐量** = 单位时间完成进程数（关注系统能力）

---

## 经典调度算法

### FCFS（先来先服务）

```cpp
struct Process {
    int pid, arrival, burst;
};

void fcfs(vector<Process>& procs) {
    sort(procs.begin(), procs.end(),
         [](auto& a, auto& b) { return a.arrival < b.arrival; });
    int time = 0;
    for (auto& p : procs) {
        time = max(time, p.arrival);
        cout << "P" << p.pid << " runs " << p.burst
             << " [start=" << time << ", end=" << time + p.burst << "]\n";
        time += p.burst;
    }
}
```

**问题：**  convoy effect（护航效应）— 长作业在前，短作业等待过久。

### SJF（短作业优先）

- 可证明最小平均周转时间（最优）
- **问题：** 不公平，长作业可能饥饿；需要预估运行时间

### RR（时间片轮转）

```cpp
void rr(vector<Process>& procs, int quantum) {
    queue<Process> q;
    int time = 0, idx = 0;
    sort(procs.begin(), procs.end(),
         [](auto& a, auto& b) { return a.arrival < b.arrival; });
    q.push(procs[idx++]);
    
    while (!q.empty()) {
        auto p = q.front(); q.pop();
        int run = min(p.burst, quantum);
        time += run;
        p.burst -= run;
        while (idx < procs.size() && procs[idx].arrival <= time)
            q.push(procs[idx++]);
        if (p.burst > 0) q.push(p);
        else cout << "P" << p.pid << " done at " << time << "\n";
    }
}
```

**时间片选择：**
- 太大 → 退化为 FCFS
- 太小 → 上下文切换开销过大
- 典型值：10-100ms（Linux 默认 100ms）

### MLFQ（多级反馈队列）

```text
┌────────────────────────────────────────────┐
│  READY QUEUE                               │
├────────────────────────────────────────────┤
│  Process P1 (Priority 5)                   │
│  Process P2 (Priority 3)                   │
│  Process P3 (Priority 5)                   │
└──────────────┬─────────────────────────────┘
               │ dequeue
               ▼
┌──────────────┴─────────────────────────────┐
│  SCHEDULER (Scheduling Algorithm)          │
└──────────────┬─────────────────────────────┘
               │ dispatch
               ▼
┌──────────────┴─────────────────────────────┐
│  CPU EXECUTION                             │
└──┬──────────────────────────────────────┬──┘
   │ timeslice expired                    │ wait I/O
   ▼                                      ▼
┌──────────────┐                ┌──────────────────┐
│ RE-ENQUEUE   │                │ BLOCKED QUEUE    │
└──────┬───────┘                └────────┬─────────┘
       │ re-enqueue                      │ I/O complete
       └───────────────┬─────────────────┘
                       ▼
              ┌────────┴────────┐
              │  READY QUEUE    │
              └─────────────────┘
```

---

# Linux CFS（完全公平调度）

Linux 默认调度器（CFS, Completely Fair Scheduler）：

```c
// CFS 核心：红黑树维护进程，键值为 vruntime
struct sched_entity {
    struct rb_node run_node;    // 红黑树节点
    u64 vruntime;               // 虚拟运行时间（核心指标）
    u64 sum_exec_runtime;       // 总实际运行时间
    unsigned int slice;         // 时间片
};

// vruntime 计算
// vruntime += 实际运行时间 * (NICE_0_LOAD / 进程权重)
// 实际选择：红黑树最左节点（vruntime 最小）

// nice 值与权重的映射
static const int prio_to_weight[40] = {
    /* -20 */ 88761, 71755, 56483, 46273, 36291,
    /* -15 */ 29154, 23254, 18705, 14949, 11916,
    /* -10 */  9548,  7620,  6100,  4904,  3906,
    /*  -5 */  3121,  2501,  1991,  1586,  1277,
    /*   0 */  1024,   820,   655,   526,   423,
    /*   5 */   335,   272,   215,   172,   137,
    /*  10 */   110,    87,    70,    56,    45,
    /*  15 */    36,    29,    23,    18,    15,
};
```

**CFS 特点：**
- 近似完美公平，保证每个进程获得 proportional 的 CPU 时间
- 不是固定时间片，而是根据负载动态调整
- O(log n) 选择（红黑树），现代 O(1) 通过 `min_vruntime` 缓存优化

---

> [!example]- 题型索引
> | 题型 | 要点 |
> |------|------|
> | FCFS convoy effect | 长作业先到导致短作业等待时间过长 |
> | SJF 的预测 | 指数平均法：τₙ₊₁ = α·tₙ + (1−α)·τₙ |
> | RR 时间片选择 | 兼顾响应时间和切换开销（≈ 上下文切换 < 5%） |
> | MLFQ 防饥饿 | 优先级重置或老化（Aging）机制 |
> | CFS vruntime | 权重越大，vruntime 增长越慢，获得更多 CPU |
> | 实时调度 | Linux: SCHED_FIFO / SCHED_RR（优先级 1-99） |
>
> > [!tip]- **工程要点**：交互式任务（I/O 密集）优先级应高于 CPU 密集型——这是 MLFQ 的设计基础，CFS 通过 sleeper fairness 也实现了类似效果。生产环境可通过 `chrt` 设置实时优先级，但需谨慎避免 CPU 密集型实时任务锁死系统。
>
> ---
>

> [!warning]- 易错点
> - 把 **05-Operating System Overview and Boot (操作系统总览与启动)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。

下一步：[08-Virtual Memory Paging and Allocation (虚拟内存、分页与分配)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)
