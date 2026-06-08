---
tags:
  - network/server
status: 🌱
---

> [!important] **核心考点**：小根堆定时器实现原理、定时器节点管理、超时检测与回调处理

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

另一种定时器实现见 → [Time Wheel Timer (时间轮定时器)](</05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/09-Timer%20Implementation%20(定时器实现)%20⭐/09b-Time%20Wheel%20Timer%20(时间轮定时器).md>)
