---
tags:
  - redis/core
status: 🌱
---

> [!important] **核心考点**
> ziplist 内存布局、连锁更新问题、listpack 的改进、何时使用

## ziplist（压缩列表）

Redis 为小数据量设计的紧凑型双向链表结构，连续内存存储：

```
ziplist 整体布局：
┌─────────┬────────┬───────┬───────┬───────┬─────┬───────┐
│ zlbytes │ zltail │ zllen │ entry │ entry │ ... │ zlend │
│ 4B      │ 4B     │ 2B    │  ...  │  ...  │     │ 1B    │
└─────────┴────────┴───────┴───────┴───────┴─────┴───────┘
```

### 每个 entry 的结构

```
┌────────────────┬────────────────┬────────────────┐
│ prev_entry_len │ encoding       │ content        │
│ 1/5 B          │ variable len   │ variable len   │
└────────────────┴────────────────┴────────────────┘
```

- **prev_entry_len**：前一个 entry 的长度（1 字节 < 254，否则 5 字节）
- **encoding**：编码类型（整数/字符串长度）
- **content**：实际数据

### 连锁更新

```c
// 问题场景：连续多个 entry 长度都在 250-253 之间
// 每个 entry 的 prev_entry_len 仅用 1 字节存储（够用）
// 突然修改第一个 entry 使其变长为 255+
// → 第二个 entry 的 prev_entry_len 需从 1 字节扩为 5 字节
// → 第二个 entry 变长 → 第三个 entry 也要扩展...
// → 连锁反应！时间复杂度 O(n²)

// 不影响——概率极低且 entry 数量少（默认 list-max-ziplist-size)
```

---

## listpack（紧凑列表）

Redis 5.0 引入，旨在替代 ziplist，解决连锁更新问题：

```
listpack entry（与 ziplist 关键区别）：

ziplist:  [prev_len] [encoding] [content]
listpack: [encoding] [content]  [backlen]
                                 ↑ 冗余的自身长度（从后向前解析）
```

**listpack 的改进：**
- 去掉 `prev_entry_len`，每个 entry 不依赖前一个节点
- 增加 `backlen`：从尾部向前解析时知道当前 entry 的长度
- **连锁更新完全消除**——一个 entry 修改不影响其他 entry

**适用场景：** List/Hash/ZSet 中元素较少时使用（默认阈值：512 个或 64B）。

---

## 使用条件

```c
// Redis 配置
hash-max-ziplist-entries 512   // 元素数 ≤ 512 用 ziplist
hash-max-ziplist-value 64      // 单元素大小 ≤ 64 字节
list-max-ziplist-size -2       // -2 表示每个节点 ≤ 8KB

// 超过阈值 → 升级为 hashtable / linkedlist / skiplist
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| ziplist 连锁更新 | 修改一个 entry 导致后续 entry 的 prev_len 扩张，概率极低 |
| listpack 解决了什么 | 去掉 prev_len 字段，消除连锁更新 |
| ziplist 优点 | 内存连续，CPU 缓存友好，小数据时比 hashtable 省内存 |
| 什么时候升级 | 元素超限或某元素超 64B → 升级为 hashtable/quicklist |

> [!tip]- **工程要点**
> ziplist/listpack 的关键价值是内存紧凑性——一个小 hash 用 ziplist 比 hashtable 省数十倍内存。`DEBUG OBJECT key` 可查看内部编码（`encoding:ziplist`）。

---

Redis 底层数据结构详解见 → [01a1-SDS：Simple Dynamic String (简单动态字符串)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md) · [01a3-skiplist：Sorted Set Internals (跳表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)
