---
tags:
  - cs/os
status: 🌱
---

# 03-CPU Scheduling (CPU调度)

> [!abstract] 核心考点：调度算法（FCFS/SJF/RR/MLFQ）、调度时机、CFS（完全公平调度）、优先级与时间片

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

## Linux CFS（完全公平调度）

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

## 经典题型速查

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
