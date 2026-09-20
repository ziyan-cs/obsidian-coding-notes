---
study_stage: backlog
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

> [!summary] 核心摘要
>
> 服务器定时器把超时事件按到期时间组织起来，常见实现有最小堆、时间轮和有序结构。选型取决于定时器数量、精度、更新频率和取消成本，回调必须避免阻塞事件循环。

# Min Heap Timer (小根堆定时器)

> [!note] 本节重点：小根堆定时器实现原理、定时器节点管理、超时检测与回调处理

## 定时器需求

高并发服务器中需要管理大量定时任务：
- 空闲连接超时关闭（避免资源泄漏）
- 延迟任务调度（如重试队列）
- 心跳检测保活

## 小根堆定时器原理

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

## 核心数据结构

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

## 核心操作实现

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

## 超时检测（tick）

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

## 延迟删除技巧

直接删除堆中非堆顶元素需要 O(log n) 的下沉/上浮操作。优化方案——**惰性删除**：

```c
typedef struct timer_node {
    // ... 原有字段 ...
    int canceled;   // 标记位：设为 1 表示已取消
} timer_node;

// 概念示意：有有效 id → 节点索引映射时，标记取消是 O(1)
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

> [!tip]- **工程要点**：小根堆适合通用到期排序，插入和真正删除通常为 O(log n)。惰性取消能让“标记”很快，但取消节点仍占内存，最终清理仍有代价；必须处理无效 id、重复取消和索引在交换节点后的更新。少量定时器可以先用简单结构，是否改为时间轮应以负载和测量决定。

---

# Time Wheel Timer (时间轮定时器)

> [!note] 本节重点：时间轮定时器分槽管理、精度与效率权衡、单轮/多轮时间轮对比

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

## 槽、游标与轮转次数

下面的 C 结构只展示状态关系，`timer_callback` 与节点所有权需在实际实现中定义；它不是可直接复制编译的完整程序。
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
| 插入 | O(log n) | 通常 O(1)，不含扩容/级联 |
| 删除 | O(log n)；惰性标记可 O(1) | 已知槽内节点位置时可 O(1) |
| 超时检测 | 弹出 k 个到期项约 O(k log n) | 与当前槽任务数及级联量有关，不能固定 O(1) |
| 精度 | 毫秒级任意 | 固定 tick 间隔 |
| 实现复杂度 | 低 | 中（多轮时较复杂） |
| 适合场景 | 通用定时 | 大量短连接超时 |

时间轮的插入和定位删除可以很便宜，但精度受 tick 约束，当前槽任务集中到期时仍需逐项处理；“连接数多”不自动说明时间轮更快。

## 实际使用

Netty 的 HashedWheelTimer 是可参考的实现之一；具体槽数、tick 与版本配置应以所用版本源码/文档为准。Linux 内核和消息系统的定时器实现也会演进，不把某个历史实现当成通用 API 契约。

> [!tip]- **工程要点**：时间轮以时间量化和槽管理换取低成本调度；大量同槽到期任务、跨层级级联和取消残留都可能形成尖峰。先估算定时器数量、精度、取消比例和超时分布，再用压测比较小根堆与时间轮。

---

> [!info]- 延伸阅读
> - 下一步：[04-Connections Buffers and Framing (连接缓冲与分帧)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/04-Connections%20Buffers%20and%20Framing%20(连接缓冲与分帧).md)

