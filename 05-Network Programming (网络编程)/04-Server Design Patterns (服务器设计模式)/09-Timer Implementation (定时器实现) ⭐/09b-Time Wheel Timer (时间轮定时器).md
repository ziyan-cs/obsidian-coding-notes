---
tags:
  - network/server
status: 🌱
---

# Time Wheel Timer — 时间轮定时器

> [!important] **核心考点**：时间轮定时器分槽管理、精度与效率权衡、单轮/多轮时间轮对比

## 时间轮基本原理

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

## 核心数据结构

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

## 单轮时间轮操作

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

## 多轮时间轮

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

## 小根堆 vs 时间轮

| 特性 | 小根堆 | 时间轮 |
|------|--------|--------|
| 插入 | O(log n) | O(1) |
| 删除 | O(log n) | O(1) |
| 超时检测 | O(k log n) | O(1) 均摊 |
| 精度 | 毫秒级任意 | 固定 tick 间隔 |
| 实现复杂度 | 低 | 中（多轮时较复杂） |
| 适合场景 | 通用定时 | 大量短连接超时 |

**时间轮的核心优势：** 插入和删除都是 O(1)——在 C10K/C100K 场景下比小根堆更有优势。

## 实际使用

**Netty (Java)：** 使用 HashedWheelTimer，按 tick 粒度轮询，槽数 512
**Linux 内核：** 使用多级时间轮管理定时器（timer wheel）
**Kafka：** 使用分层时间轮（Purgatory 延迟操作）

> [!tip]- **工程要点**：时间轮是典型的"以空间换时间"——固定大小的槽数组 + 链表，插入永远 O(1)。在定时器数量巨大（百万级）且以短超时为主的场景下，时间轮优于小根堆。但时间轮精度受 tick 间隔限制（如 10ms tick，设 5ms 超时也只能在下一个 tick 触发）。实际服务器常选用小根堆（通用性好）或时间轮（极致性能）。

---

另一种定时器实现见 → [Min-Heap Timer (小根堆定时器)](</05-Network%20Programming%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/09-Timer%20Implementation%20(定时器实现)%20⭐/09a-Min-Heap%20Timer%20(小根堆定时器).md>)
