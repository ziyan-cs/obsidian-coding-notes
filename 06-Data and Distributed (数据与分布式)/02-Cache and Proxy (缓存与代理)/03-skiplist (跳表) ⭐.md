---
tags:
  - redis/core
status: 🌱
---

# 03-skiplist (跳表)

> [!abstract] 核心考点：> 跳表数据结构、层高概率分布、与平衡树/B+ 树的对比、ZSet 实现

## 跳表（Skiplist）

Redis Sorted Set 的有序结构核心，基于多级索引的链表：

```text
在跳表中查找数值 31：
从最高层级开始，在每一层向后跳跃遍历。
25 往后到 55（数值过大），下降一层 → 从 25 向后找到 31
Time complexity: O(log n)

Level 3: HEAD ──────────→ 17 ──────→ 25 ─────────────→ 55 ─────────────→ ∞

Level 2: HEAD ────→ 12 ─→ 17 ──────→ 25 ─────────────→ 55 ─────→ 67 ───→ ∞

Level 1: HEAD ────→ 12 ─→ 17 → 19 ─→ 25 ─→ 31 ───────→ 55 ─────→ 67 ───→ ∞
```

### 节点结构

```c
#define ZSKIPLIST_MAXLEVEL 32  // 最大层数（2^32 个元素足够）
#define ZSKIPLIST_P 0.25       // 层高概率因子

typedef struct zskiplistNode {
    sds ele;                    // 存储的实际数据（SDS）
    double score;               // 分值（排序依据）
    struct zskiplistNode *backward; // 后退指针（仅第一层用）
    struct zskiplistLevel {
        struct zskiplistNode *forward; // 前进指针
        unsigned long span;    // 跨度（到下一节点的距离）
    } level[];                 // 柔性数组，层高随机生成
} zskiplistNode;

typedef struct zskiplist {
    struct zskiplistNode *header, *tail;
    unsigned long length;      // 节点数
    int level;                 // 当前最大层数
} zskiplist;
```

### 层高生成算法

```c
// 每次创建节点时随机生成层高
// P=0.25 意味着 75% 的节点在 level 1，逐层递减
int zslRandomLevel(void) {
    int level = 1;
    while ((random() & 0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}
// 概率分布：level 1 = 75%, level 2 ≈ 18.75%, level 3 ≈ 4.69%...
// 期望层高 = 1/(1-P) = 1.33
```

---

## 操作复杂度

| 操作 | 跳表 | 平衡树 | B+ 树 |
|------|------|--------|-------|
| 查找 | O(log n) | O(log n) | O(log n) |
| 插入 | O(log n) | O(log n) | O(log n) |
| 范围查询 | O(log n + k) | O(log n + k) | O(log n + k) ← 最优 |
| 实现难度 | 简单 | 复杂（AVL 旋转/红黑染色） | 中等 |
| 内存 | 约 1.33× 指针（avg） | 2× 指针 | 更高 |
| 缓存友好 | 差（跳跃式访问） | 差 | 好（页式存储） |

**Redis 为什么选跳表而不是平衡树做 ZSet：**
1. 范围查询简单——`ZRANGE` 只需沿第一层链表遍历
2. 插入/删除不需旋转/重平衡，代码更简洁
3. 层高随机化，平均性能稳定

---

## ZSet 使用的两种结构

```c
// ZSet 底层 = ziplist (小数据) + dict + skiplist (大数据)
typedef struct zset {
    dict *dict;         // key→score 映射（O(1) 查分值）
    zskiplist *zsl;    // score→key 排序（范围查询）
} zset;
```

| 条件 | 编码 | 说明 |
|------|------|------|
| 元素 < 128 且分值 < 64B | ziplist | 紧凑存储 |
| 超过阈值 | skiplist + dict | 双结构支持高效查询和排序 |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 跳表查找流程 | 从最高层向右&向下，类似"二分查找的链表版" |
| 层高概率 P=1/4 | 经验值的分时，指针数约 1.33n，查询性能好 |
| 跳表 vs B+ 树 | B+ 树磁盘 I/O 少（页式存储），跳表内存友好 |
| ZRANGE 实现 | 沿第一层 forward 指针遍历（已排序的双向链表） |
| dict 的作用 | 需要 O(1) 查某个元素的 score（ZSCORE） |

> [!tip]- **工程要点**
> 跳表是典型的"以空间换时间"结构，每层索引是概率性平衡而非确定性平衡。Redis 中跳表主要用于 ZSet，此外还在集群中用做内部数据结构。

---



动态字符串实现见 → [01a1-SDS：Simple Dynamic String (简单动态字符串)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md) · [01a4-dict：Hash Table Rehashing (字典与渐进式rehash)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a4-dict：Hash%20Table%20Rehashing%20(字典与渐进式rehash)%20⭐.md)
