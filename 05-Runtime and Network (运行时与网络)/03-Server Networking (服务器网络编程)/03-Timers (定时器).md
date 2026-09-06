---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# 30 秒回答

**核心结论**：学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# Min Heap Timer (小根堆定时器)

> [!note] 本节重点：核心考点：小根堆定时器实现原理、定时器节点管理、超时检测与回调处理

# 定时器需求

高并发服务器中需要管理大量定时任务：
- 空闲连接超时关闭（避免资源泄漏）
- 延迟任务调度（如重试队列）
- 心跳检测保活

# 小根堆定时器原理

小根堆（Min-Heap）以**超时时间**为键值，堆顶永远是最近要超时的定时器：

```
          100ms
         /    \
      200ms   300ms
      /   \
   500ms  600ms

堆顶 = 100ms → 最早到期
```

**三个核心操作：**
- **插入** O(log n)：新定时器加入堆尾，上浮调整
- **删除** O(log n)：将目标节点与堆尾交换，删除堆尾，下沉调整
- **获取最早到期** O(1)：直接取堆顶

# 核心数据结构

```c
typedef struct timer_node {
    int id;
    uint64_t expire;        // 绝对到期时间（ms）
    timer_callback cb;       // 超时回调函数
    void *user_data;         // 用户数据
    int period;              // 周期执行间隔（0 = 一次性）
} timer_node;

typedef struct min_heap_timer {
    timer_node **nodes;      // 指针数组，连续存储
    int capacity;
    int size;
    // 辅助：id → index 映射，支持 O(1) 删除
    int *id_to_idx;
} min_heap_timer;
```

# 核心操作实现

```c
void min_heap_add(min_heap_timer *heap, timer_node *node) {
    // 1. 插入堆尾
    int i = heap->size++;
    heap->nodes[i] = node;
    heap->id_to_idx[node->id] = i;

    // 2. 上浮调整
    while (i > 0) {
        int p = (i - 1) / 2;       // 父节点
        if (heap->nodes[i]->expire >= heap->nodes[p]->expire)
            break;                  // 已满足堆性质
        swap(heap, i, p);          // 交换父子
        i = p;
    }
}

void min_heap_pop(min_heap_timer *heap) {
    // 1. 堆顶与堆尾交换
    swap(heap, 0, --heap->size);

    // 2. 从堆顶下沉
    int i = 0;
    while (1) {
        int smallest = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;

        if (l < heap->size && heap->nodes[l]->expire < heap->nodes[smallest]->expire)
            smallest = l;
        if (r < heap->size && heap->nodes[r]->expire < heap->nodes[smallest]->expire)
            smallest = r;
        if (smallest == i) break;
        swap(heap, i, smallest);
        i = smallest;
    }
}
```

# 超时检测（tick）

在事件循环的每次迭代中检查超时：

```c
uint64_t now = get_current_ms();

while (heap->size > 0 && heap->nodes[0]->expire <= now) {
    timer_node *node = heap->nodes[0];
    node->cb(node->user_data);         // 执行回调

    if (node->period > 0) {
        // 周期定时器：重新计算到期时间，重新插入
        node->expire = now + node->period;
        min_heap_add(heap, node);
    } else {
        // 一次性定时器：移除并释放
        min_heap_pop(heap);
        free(node);
    }
}
```

**性能边界：**
```
定时器数量  |  每次 tick 最坏情况
   100     |  O(log 100) ≈ 7 次比较
   10000   |  O(log 10000) ≈ 14 次比较
   1000000 |  O(log 1000000) ≈ 20 次比较
```

# 延迟删除技巧

直接删除堆中非堆顶元素需要 O(log n) 的下沉/上浮操作。优化方案——**惰性删除**：

```c
typedef struct timer_node {
    // ... 原有字段 ...
    int canceled;   // 标记位：设为 1 表示已取消
} timer_node;

// 取消定时器：O(1)
void cancel_timer(min_heap_timer *heap, int id) {
    timer_node *node = heap->nodes[heap->id_to_idx[id]];
    node->canceled = 1;
}

// tick 时跳过已取消的节点
while (heap->size > 0 && (heap->nodes[0]->expire <= now || heap->nodes[0]->canceled)) {
    if (heap->nodes[0]->canceled) {
        // 丢弃已取消的定时器
    } else {
        // 正常超时处理
    }
    min_heap_pop(heap);
}
```

> [!tip]- **工程要点**：小根堆定时器适合**大量定时器（数万级）** 且有频繁插入/删除的场景。堆的实现简单、性能稳定。如果定时器数量较少（<100），使用有序链表遍历即可，无需堆这种复杂结构。Redis 的定时器也是基于小根堆实现的。

---

另一种定时器实现见 → [Time Wheel Timer (时间轮定时器)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/09-Timer%20Implementation%20(定时器实现)%20⭐/09b-Time%20Wheel%20Timer%20(时间轮定时器).md>)

---

# Time Wheel Timer (时间轮定时器)

> [!note] 本节重点：核心考点：时间轮定时器分槽管理、精度与效率权衡、单轮/多轮时间轮对比

# 时间轮基本原理

时间轮（Time Wheel）将时间划分为固定大小的槽（slot），每个槽对应一个时间间隔，槽内挂载该时刻到期的定时器列表。

```
单层时间轮（N 个槽，每个槽间隔 T 毫秒）：

    槽 0    槽 1    槽 2    槽 3    ...    槽 N-1
    │       │       │       │              │
    ├───────┼───────┼───────┼───────...────┤
    ↑
    当前指针 (cursor)

每个 tick（T 毫秒），指针移动到下一个槽
  槽内的所有定时器到期 → 执行回调
  新定时器插入到 (cursor + delay/T) % N 槽
```

# 核心数据结构 · 延伸要点 2
```c
#define WHEEL_SIZE 256   // 256 个槽

typedef struct timer_node {
    struct timer_node *next;
    int rotation;            // 剩余轮转次数（多轮时间轮）
    uint64_t expire;         // 绝对到期时间
    timer_callback cb;
    void *user_data;
} timer_node;

typedef struct time_wheel {
    timer_node *slots[WHEEL_SIZE];   // 每个槽是链表头
    int cursor;                       // 当前槽位置
    uint64_t tick_ms;                 // 每个槽的时间间隔（ms）
} time_wheel;
```

# 单轮时间轮操作

```c
// 添加定时器（假设延迟不超过一圈）
void wheel_add(time_wheel *wheel, timer_node *node, uint64_t delay_ms) {
    int ticks = delay_ms / wheel->tick_ms;
    int slot = (wheel->cursor + ticks) % WHEEL_SIZE;

    // 头插到对应槽
    node->next = wheel->slots[slot];
    wheel->slots[slot] = node;
}

// Tick 一次
void wheel_tick(time_wheel *wheel) {
    wheel->cursor = (wheel->cursor + 1) % WHEEL_SIZE;
    timer_node *node = wheel->slots[wheel->cursor];

    while (node) {
        timer_node *cur = node;
        node = node->next;
        cur->cb(cur->user_data);
        free(cur);
    }
    wheel->slots[wheel->cursor] = NULL;  // 清空槽
}
```

**单轮局限：** 最大延迟 = WHEEL_SIZE × tick_ms。如 tick_ms=10ms, WHEEL_SIZE=256，最大延迟 2.56s。

# 多轮时间轮

多轮时间轮用**多层轮**解决大延迟问题，类似水表进位：

```
多轮时间轮（5 层，每层 256 个槽）：

  第 1 层:  0-255 ticks × 10ms = 0~2.55s
  第 2 层:  0-255 ticks × 2.56s = 0~10.9min
  第 3 层:  0-255 ticks × 10.9min = 0~46.5h
  第 4 层:  0-255 ticks × 46.5h = 0~496天
  第 5 层:  0-255 ticks × 496天 = 0~348年

每层 tick_ms 是上一层的 WHEEL_SIZE 倍。
```

**添加定时器（延迟大时降级到高层）：**
```c
void wheel_add_multi(time_wheel_multi *wheels, timer_node *node, uint64_t delay_ms) {
    uint64_t ticks = delay_ms / BASE_TICK_MS;
    uint64_t rotation = 0;
    int slot;

    for (int level = 0; level < MAX_LEVEL; level++) {
        slot = (cursor[level] + ticks) % WHEEL_SIZE;
        if (slot != cursor[level] || ticks < WHEEL_SIZE) {
            // 在当前层即可容纳
            node->rotation = rotation;
            // 插入 whells[level].slots[slot]
            return;
        }
        ticks /= WHEEL_SIZE;
        rotation |= (1 << level);  // 标记轮转
    }
}
```

**Tick 时降级：**
```c
void wheel_tick_multi(time_wheel_multi *wheels) {
    int level = 0;
    cursor[0] = (cursor[0] + 1) % WHEEL_SIZE;

    // 如果第 0 层转完一圈 → 降级第 1 层的一个槽
    if (cursor[0] == 0) {
        cursor[1] = (cursor[1] + 1) % WHEEL_SIZE;
        // 将第 1 层当前槽的定时器重新插入到第 0 层
        redistribute(wheels, 1, 0);
    }

    // 执行第 0 层当前槽的定时器
    execute_slot(wheels, 0);
}
```

# 小根堆 vs 时间轮

| 特性 | 小根堆 | 时间轮 |
|------|--------|--------|
| 插入 | O(log n) | O(1) |
| 删除 | O(log n) | O(1) |
| 超时检测 | O(k log n) | O(1) 均摊 |
| 精度 | 毫秒级任意 | 固定 tick 间隔 |
| 实现复杂度 | 低 | 中（多轮时较复杂） |
| 适合场景 | 通用定时 | 大量短连接超时 |

**时间轮的核心优势：** 插入和删除都是 O(1)——在 C10K/C100K 场景下比小根堆更有优势。

# 实际使用

**Netty (Java)：** 使用 HashedWheelTimer，按 tick 粒度轮询，槽数 512
**Linux 内核：** 使用多级时间轮管理定时器（timer wheel）
**Kafka：** 使用分层时间轮（Purgatory 延迟操作）

> [!tip]- **工程要点**：时间轮是典型的"以空间换时间"——固定大小的槽数组 + 链表，插入永远 O(1)。在定时器数量巨大（百万级）且以短超时为主的场景下，时间轮优于小根堆。但时间轮精度受 tick 间隔限制（如 10ms tick，设 5ms 超时也只能在下一个 tick 触发）。实际服务器常选用小根堆（通用性好）或时间轮（极致性能）。

---

另一种定时器实现见 → [Min-Heap Timer (小根堆定时器)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/09-Timer%20Implementation%20(定时器实现)%20⭐/09a-Min-Heap%20Timer%20(小根堆定时器).md>)

# 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

# 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Timers (定时器)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
