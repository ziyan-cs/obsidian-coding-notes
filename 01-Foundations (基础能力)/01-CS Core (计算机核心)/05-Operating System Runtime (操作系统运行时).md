---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题将同一条学习链上的基础概念整合为一篇：先建立整体模型，再阅读机制、边界和例子。

# 30 秒回答

**核心结论**：本专题将同一条学习链上的基础概念整合为一篇：先建立整体模型，再阅读机制、边界和例子。

# Operating System Overview (操作系统总览)

> [!note] 本节重点：核心考点：OS 四大功能、内核态 vs 用户态、系统调用、操作系统类型与架构

# 操作系统定义

操作系统是管理计算机硬件和软件资源的系统软件，提供用户与计算机之间的接口。

## 四大功能

| 功能 | 说明 |
|------|------|
| **进程管理** | 创建/调度/同步/通信进程 |
| **内存管理** | 地址空间分配、虚拟内存、页表管理 |
| **文件系统** | 文件存储、目录结构、权限控制 |
| **I/O 管理** | 设备驱动、中断处理、DMA |

## 内核态 vs 用户态

```cpp
// 系统调用示例（C++ 中调用 Linux 系统调用）
#include <unistd.h>
#include <iostream>

int main() {
    // 用户态 → 系统调用 → 内核态
    write(STDOUT_FILENO, "Hello\n", 6);  // write 是系统调用
    // 另一种形式
    syscall(SYS_write, STDOUT_FILENO, "Hello\n", 6);
    return 0;
}
```

- **内核态**：可执行特权指令，访问所有硬件资源
- **用户态**：受限执行，通过系统调用（`int 0x80` / `syscall` 指令）陷入内核

## 系统调用流程

```
用户程序 → write() (libc 封装) → syscall 指令 → 内核 sys_write → 返回用户态
```

---

# 操作系统类型

| 类型 | 特点 | 代表 |
|------|------|------|
| 批处理 OS | 批量运行作业，无交互 | IBM OS/360 |
| 分时 OS | 时间片轮转，交互式 | Unix, Linux |
| 实时 OS | 严格时间约束，确定性 | RT-Linux, FreeRTOS |
| 分布式 OS | 多机统一资源管理 | Amoeba |
| 嵌入式 OS | 资源受限，专用性强 | VxWorks, µC/OS |

## 宏内核 vs 微内核

```cpp
// 宏内核（Linux）：驱动在内核空间，性能好但崩溃影响大
// 微内核（Minix, QNX）：驱动在用户空间，稳定但 IPC 开销大
```

```text
┌──────────────────────────────────────────────┐
│                USER SPACE                    │
├──────────────────────────────────────────────┤
│  Applications (shell, browser, server)       │
│  System Libraries / Runtime (libc, syscall)  │
└──────────────────────┬───────────────────────┘
                       │ syscall
                       ▼
┌──────────────────────────────────────────────┐
│               KERNEL SPACE                   │
├──────────────────────────────────────────────┤
│  System Call Interface (syscall entry)       │
├──────────┬──────────┬──────────┬─────────────┤
│ Process  │ Memory   │ File Sys │ Network     │
│ Mgmt     │ Mgmt     │ VFS, FS  │ TCP/IP,Sock │
├──────────┴──────────┴──────────┴─────────────┤
│  Device Drivers (char, block, network)       │
└──────────────────┬───────────────────────────┘
                   │
         ┌─────────┼──────────┬──────────┐
         ▼         ▼          ▼          ▼
┌──────────────────────────────────────────────┐
│              HARDWARE LAYER                  │
├──────────────────────────────────────────────┤
│  CPU     Main Memory    Disk/SSD      NIC    │
│ (MMU)                                        │
└──────────────────────────────────────────────┘
```

---

# 操作系统启动流程（x86）

```
BIOS/UEFI → 引导加载程序（GRUB）→ 内核解压 → start_kernel → init 进程
```

1. **BIOS/UEFI**：POST 自检，加载引导扇区
2. **Bootloader（GRUB）**：选择内核，加载到内存
3. **内核初始化**：`start_kernel()` 初始化调度器、内存管理、中断
4. **init 进程**：PID=1，启动用户态服务（systemd）

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| 系统调用开销 | 上下文切换（保存用户态寄存器→恢复内核态→切换页表） |
| 内核态 vs 用户态切换代价 | ≈ 1-10μs，比函数调用慢 100-1000 倍 |
| Linux 的 `copy_on_write` | fork() 时父子共享页，写时复制（COW） |
| 中断与系统调用的区别 | 中断异步（硬件触发），系统调用同步（主动触发） |
| 操作系统的设计哲学 | 机制（mechanism）与策略（policy）分离 |

> [!tip]- **工程要点**：系统调用是用户态与内核态的唯一入口，每次 syscall 涉及特权级切换（ring 3 → ring 0），是性能关键路径。批量系统调用（如 readv/writev）比多次单调用明显更快。

---

进程线程与上下文切换详见 → [Process vs Thread（进程与线程）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/01-Process%20vs%20Thread%20(进程与线程)%20⭐.md) · [Context Switching（上下文切换）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/02-Context%20Switching%20(上下文切换).md)

---

# Processes and Threads (进程与线程)

> [!note] 本节重点：核心考点：进程与线程的区别、PCB/TCB、进程状态转换、进程创建（fork）、协程

# 进程 vs 线程

| 维度 | 进程（Process） | 线程（Thread） |
|------|----------------|----------------|
| 资源拥有 | 独立地址空间、文件描述符 | 共享进程资源 |
| 切换代价 | 高（切换页表/TLB/地址空间） | 低（仅保存寄存器） |
| 通信方式 | IPC（pipe, shm, socket） | 直接读写共享内存 |
| 健壮性 | 进程间隔离，一个崩溃不影响其他 | 一个线程崩溃可能影响整个进程 |
| 创建开销 | 高（fork 需要复制页表） | 低（pthread_create 轻量） |

## 进程控制块（PCB）

内核为每个进程维护的 PCB（Linux 中为 `task_struct`）：

```c
// Linux task_struct 关键字段（简化）
struct task_struct {
    pid_t pid;                      // 进程 ID
    long state;                     // 进程状态（TASK_RUNNING, TASK_INTERRUPTIBLE...）
    struct mm_struct *mm;           // 地址空间（页表）
    struct files_struct *files;     // 打开的文件描述符表
    struct thread_info *thread;     // 线程信息（寄存器上下文）
    struct list_head children;      // 子进程链表
    unsigned int policy;            // 调度策略
    int prio;                       // 动态优先级
    // ...
};
```

---

# 进程状态

```text
[*]
  └── create → NEW

NEW
  └── admit/ready → READY

READY
  └── scheduler dispatch → RUNNING

RUNNING
  ├── timeslice expired / preempted → READY
  ├── wait I/O / lock → BLOCKED
  └── exit → TERMINATED

BLOCKED
  └── I/O complete / lock released → READY

TERMINATED
  └── → [*]

Note for RUNNING:
  PCB saves execution context
  Each switch requires address space switch (high overhead)
```

**Linux 特有状态：**
- `TASK_INTERRUPTIBLE`：可中断睡眠（收到信号可唤醒）
- `TASK_UNINTERRUPTIBLE`：不可中断睡眠（如等待 I/O 完成）
- `TASK_STOPPED`：收到 SIGSTOP 信号
- `TASK_DEAD`：已退出（`exit()`），等待父进程 `wait()`

---

# 进程创建

```cpp
#include <unistd.h>
#include <iostream>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程
        std::cout << "Child: PID=" << getpid()
                  << ", Parent=" << getppid() << std::endl;
        execlp("/bin/ls", "ls", "-l", nullptr);  // 替换进程映像
    } else if (pid > 0) {
        // 父进程
        std::cout << "Parent: child PID=" << pid << std::endl;
        wait(nullptr);  // 等待子进程结束
    } else {
        perror("fork failed");
    }
    return 0;
}
```

**fork 的写时复制（COW）：** fork 时子进程共享父进程的页，仅置为只读。任一进程写入时触发缺页异常，内核复制该页。避免了 fork 时复制整个地址空间的开销。

---

# 线程与协程

```cpp
// POSIX 线程创建
#include <pthread.h>
#include <iostream>

void* thread_func(void* arg) {
    int* id = (int*)arg;
    std::cout << "Thread " << *id << " running" << std::endl;
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    pthread_create(&t1, nullptr, thread_func, &id1);
    pthread_create(&t2, nullptr, thread_func, &id2);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    return 0;
}
```

**协程（Coroutine）：** 用户态轻量级线程，由程序员显式 yield/schedule，无需内核参与。C++20 引入 `co_await` / `co_yield` / `co_return`。

| 线程 | 协程 |
|------|------|
| 内核调度，抢占式 | 用户调度，协作式 |
| 栈大小 ≈ 8MB（固定） | 栈大小 ≈ KB 级（可动态） |
| 上下文切换 ≈ 1-3μs | 上下文切换 ≈ 0.1-0.3μs |
| 适合 CPU 密集型 | 适合 I/O 密集型 |

---

# 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| fork 返回值 | 父进程返回子 PID，子进程返回 0，错误返回 -1 |
| 孤儿进程 vs 僵尸进程 | 孤儿被 init 收养；僵尸已退出且未 wait，占用 PCB |
| 多线程共享什么 | 堆、全局变量、文件描述符；不共享栈、寄存器 |
| 线程安全 | 用互斥锁 / 原子操作 / TLS（线程本地存储）保证 |
| fork 后子进程获得的资源 | 文件描述符表复制（共享偏移量）、信号处理函数继承 |

> [!tip]- **工程要点**：进程用于隔离，线程用于并行。现代高性能服务器常用**多进程 + 事件驱动**（Nginx）或**多线程 + 异步 I/O**（Redis）。创建线程/进程后必须 join/detach/wait，否则资源泄漏。

---

上下文切换与CPU调度详见 → [Context Switching（上下文切换）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/02-Context%20Switching%20(上下文切换).md) · [CPU Scheduling（CPU调度）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/03-CPU%20Scheduling%20(CPU调度).md)

---

# Context Switching (上下文切换)

> [!note] 本节重点：核心考点：上下文切换的流程、切换代价、TLB 失效、切换 vs 模式切换的区别

# 上下文切换

CPU 从一个进程/线程切换到另一个进程/线程时，保存当前状态并恢复目标状态的过程。

## 切换内容

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

## 触发场景

1. **时间片耗尽**（时钟中断 → scheduler_tick → schedule）
2. **阻塞操作**（I/O、sleep、锁等待 → 主动调用 schedule）
3. **高优先级进程就绪**（抢占式调度）
4. **系统调用返回时**检查 `need_resched` 标志

---

# 切换代价

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

## 直接代价（显性）

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

## 间接代价（隐性）

1. **TLB 失效**：切换页表后 TLB 需重新填充，导致后续内存访问变慢
2. **Cache 污染**：当前缓存的热数据被目标进程的冷数据覆盖
3. **分支预测器失效**：BTB（分支目标缓冲）中的历史记录失效

---

# 上下文切换流程（Linux）

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

# 减少上下文切换的方法

| 方法 | 原理 | 适用场景 |
|------|------|---------|
| 减少线程数 | 避免过多线程竞争 CPU | CPU 密集型 |
| 异步 I/O（epoll/io_uring） | 用一个线程处理大量事件 | 网络服务器 |
| 协程 | 用户态调度，无需内核切换 | 高并发 I/O |
| 大页（Huge Pages） | 减少 TLB miss | 内存密集型 |
| CPU 亲和性 | 绑定进程到固定核，cache 更热 | 性能敏感路径 |

> [!tip]- **工程要点**：Redis 单线程模型高吞吐的核心原因之一就是避免了上下文切换。Nginx 事件驱动 + 少量 worker 进程大幅降低了切换开销。检测 `vmstat 1` 的 `cs`（context switch）列可知是否切换过度。

---

进程线程与CPU调度详见 → [Process vs Thread（进程与线程）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/01-Process%20vs%20Thread%20(进程与线程)%20⭐.md) · [CPU Scheduling（CPU调度）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/03-CPU%20Scheduling%20(CPU调度).md)

---

# CPU Scheduling (CPU 调度)

> [!note] 本节重点：核心考点：调度算法（FCFS/SJF/RR/MLFQ）、调度时机、CFS（完全公平调度）、优先级与时间片

# 调度目标

| 场景 | 目标 | 策略 |
|------|------|------|
| 批处理系统 | 高吞吐量、低周转时间 | FCFS, SJF |
| 交互式系统 | 低响应时间 | RR, MLFQ |
| 实时系统 | 可预测性、满足截止时间 | 优先级调度, EDF |

## 评价指标

- **周转时间** = 完成时间 − 到达时间（关注整体效率）
- **响应时间** = 首次运行 − 到达时间（关注交互体验）
- **等待时间** = 等待 CPU 总时间（关注公平性）
- **吞吐量** = 单位时间完成进程数（关注系统能力）

---

# 经典调度算法

## FCFS（先来先服务）

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

## SJF（短作业优先）

- 可证明最小平均周转时间（最优）
- **问题：** 不公平，长作业可能饥饿；需要预估运行时间

## RR（时间片轮转）

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

## MLFQ（多级反馈队列）

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

# 经典题型速查 · 延伸要点 3
| 题型 | 要点 |
|------|------|
| FCFS convoy effect | 长作业先到导致短作业等待时间过长 |
| SJF 的预测 | 指数平均法：τₙ₊₁ = α·tₙ + (1−α)·τₙ |
| RR 时间片选择 | 兼顾响应时间和切换开销（≈ 上下文切换 < 5%） |
| MLFQ 防饥饿 | 优先级重置或老化（Aging）机制 |
| CFS vruntime | 权重越大，vruntime 增长越慢，获得更多 CPU |
| 实时调度 | Linux: SCHED_FIFO / SCHED_RR（优先级 1-99） |

> [!tip]- **工程要点**：交互式任务（I/O 密集）优先级应高于 CPU 密集型——这是 MLFQ 的设计基础，CFS 通过 sleeper fairness 也实现了类似效果。生产环境可通过 `chrt` 设置实时优先级，但需谨慎避免 CPU 密集型实时任务锁死系统。

---

进程线程与上下文切换详见 → [Process vs Thread（进程与线程）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/01-Process%20vs%20Thread%20(进程与线程)%20⭐.md) · [Context Switching（上下文切换）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/02-Context%20Switching%20(上下文切换).md)

# 零基础阅读路径

先读本页的总览与术语，再沿“数据/指令 → 硬件状态 → 操作系统抽象 → 可见结果”追踪一个例子。遇到性能数字先跳过，等能解释状态流转后再回来比较。

# 常见误区

- 把 **05-Operating System Runtime (操作系统运行时)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **05-Operating System Runtime (操作系统运行时)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
