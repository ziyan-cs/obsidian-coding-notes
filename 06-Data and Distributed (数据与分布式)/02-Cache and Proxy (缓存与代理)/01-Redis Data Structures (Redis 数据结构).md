---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 01-Redis Data Structures (Redis 数据结构)

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

## 30 秒回答

**核心结论**：学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。


## SDS Strings (简单动态字符串)

> [!note] 本节重点心考点：> SDS 设计原理、相比 C 字符串的优势、内存预分配策略、二进制安全

## SDS 结构

Redis 自定义的字符串类型，替代 C 字符串（`char*`）：

```c
// Redis 3.2+ 的 SDS 实现（5 种长度类型）
struct sdshdr8 {
    uint8_t len;      // 已使用字节数
    uint8_t alloc;    // 总分配字节数（不含头和 null 终止符）
    unsigned char flags; // 低 3 位存类型（sdshdr5/8/16/32/64）
    char buf[];       // 数据数组（柔性数组）
};
// sdshdr16/32/64 同理，len/alloc 类型不同
```

**为什么设计 5 种类型：** 根据字符串长度选择最小的 header（sdshdr5/sdshdr8/sdshdr16/sdshdr32/sdshdr64），节省内存。

---

## SDS 相比 C 字符串的优势

| 问题 | C 字符串 | SDS |
|------|---------|-----|
| **获取长度** | O(n) 遍历 | O(1) 读 len 字段 |
| **缓冲区溢出** | `strcat` 可能覆盖邻接内存 | API 自动检查 alloc 并扩容 |
| **内存重分配** | 每次修改都需要 | 预分配 + 惰性释放 |
| **二进制安全** | 遇 `\0` 截断（空字符结尾） | 用 len 记录长度，`\0` 可存储 |
| **兼容 C 函数** | 天生兼容 | 也兼容（末尾保留 `\0`） |

---

## 内存预分配策略

```c
// SDS 扩容策略（sds.c 中的 sdsMakeRoomFor）
if (newlen < SDS_MAX_PREALLOC)     // 1MB 以内
    newlen *= 2;                    // 加倍（翻倍增长）
else
    newlen += SDS_MAX_PREALLOC;     // 超过 1MB 后每次加 1MB

// 例：当前 "abc"（len=3）
// append "defghij" → newlen = 10 → 实际分配 20（预分配）
// 下次 append 只需抄一次，避免频繁 realloc
```

**惰性释放：** `sdstrim(s, "X")` 仅修改指针位置，不释放内存；`sdsclear(s)` 将 len 置 0 但保留 alloc。真正释放用 `sdsfree()`。

---

## SDS API 速查

```c
sds s = sdsnew("hello");        // 创建，O(len)
sds s = sdsempty();             // 创建空串
s = sdscat(s, " world");        // 拼接（可能 realloc）
s = sdscpy(s, "new");           // 覆盖
s = sdstrim(s, "abc");          // 去除两端指定字符
sdstoupper(s);                  // 转大写
sdsrange(s, 0, 2);             // 取子串 [0,2]
sdsfree(s);                     // 释放
```

**API 设计特点：** 修改 SDS 的 API 都返回 `sds*`（可能因 realloc 改变地址），调用者必须用返回值。

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| SDS 如何 O(1) 获取长度 | header 中存 len 字段 |
| 二进制安全含义 | 可存 `\0`、图片等非文本数据 |
| 预分配阈值 1MB | 小字符串加倍，大字符串线性增长 |
| SDS 相比 std::string | Redis C 实现，内存紧凑；std::string SSO 且有 RAII |

> [!tip]- **工程要点**
> SDS 的预分配策略是"空间换时间"的典型——大多数 append 操作无需重新分配。`SDS_MAX_PREALLOC` 设为 1MB 避免大字符串翻倍浪费。

---



压缩列表实现见 → [01a2-ziplist & listpack (压缩列表)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a2-ziplist%20&%20listpack%20(压缩列表).md) · [01a3-skiplist：Sorted Set Internals (跳表)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)

---

## ziplist and listpack (压缩列表)

> [!note] 本节重点心考点：> ziplist 内存布局、连锁更新问题、listpack 的改进、何时使用

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

## 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| ziplist 连锁更新 | 修改一个 entry 导致后续 entry 的 prev_len 扩张，概率极低 |
| listpack 解决了什么 | 去掉 prev_len 字段，消除连锁更新 |
| ziplist 优点 | 内存连续，CPU 缓存友好，小数据时比 hashtable 省内存 |
| 什么时候升级 | 元素超限或某元素超 64B → 升级为 hashtable/quicklist |

> [!tip]- **工程要点**
> ziplist/listpack 的关键价值是内存紧凑性——一个小 hash 用 ziplist 比 hashtable 省数十倍内存。`DEBUG OBJECT key` 可查看内部编码（`encoding:ziplist`）。

---

Redis 底层数据结构详解见 → [01a1-SDS：Simple Dynamic String (简单动态字符串)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md) · [01a3-skiplist：Sorted Set Internals (跳表)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)

---

## skiplist (跳表)

> [!note] 本节重点心考点：> 跳表数据结构、层高概率分布、与平衡树/B+ 树的对比、ZSet 实现

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

## 经典题型速查 · 延伸要点 3
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

---

## dict Hash Table (字典与渐进式 rehash)

> [!note] 本节重点心考点：> dict 结构、rehash 触发条件、渐进式 rehash 如何避免阻塞、与 Java HashMap 区别

## Redis dict 结构

```c
// Redis 字典核心结构
typedef struct dict {
    dictType *type;          // 类型特定函数（hash/compare/keydup...）
    void *privdata;          // 私有数据
    dictht ht[2];            // 两张哈希表（ht[0] 在用，ht[1] 在 rehash）
    long rehashidx;          // rehash 进度（-1 表示无进行中 rehash）
    unsigned long iterators; // 正在迭代的迭代器数
} dict;

typedef struct dictht {
    dictEntry **table;       // 哈希表数组
    unsigned long size;      // 表大小（2^n）
    unsigned long sizemask;  // 掩码 = size - 1（计算索引用）
    unsigned long used;      // 已用节点数
} dictht;

typedef struct dictEntry {
    void *key;               // 键
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
    } v;                     // 值（节省内存的 union）
    struct dictEntry *next;  // 链地址法解决冲突
} dictEntry;
```

---

## Rehash 机制

### 触发条件

```c
// 扩容条件（负载因子）
// load_factor = ht[0].used / ht[0].size
// 主动扩容：load_factor >= 1（BGSAVE/BGREWRITEAOF 时 >= 5）
// 被动扩容：load_factor >= 5
// 缩容条件：load_factor < 0.1

// 扩容后新 size = 第一个 >= ht[0].used * 2 的 2^n
```

### 渐进式 Rehash

```c
// 核心思想：分批迁移，避免一次 rehash 阻塞服务
void dictRehash(dict *d, int n) {
    int empty_visits = n * 10;  // 最多遍历的空桶数
    while (n-- && d->ht[0].used != 0) {
        // 找到下一个非空桶
        while (d->ht[0].table[d->rehashidx] == NULL) {
            d->rehashidx++;
            if (--empty_visits <= 0) return; // 空桶太多，提前退出
        }
        // 迁移该桶的所有 entry 到 ht[1]
        dictEntry *de = d->ht[0].table[d->rehashidx];
        while (de) {
            dictEntry *next = de->next;
            int idx = hash(de->key) & d->ht[1].sizemask;
            de->next = d->ht[1].table[idx];
            d->ht[1].table[idx] = de;
            de = next;
            d->ht[0].used--;
            d->ht[1].used++;
        }
        d->ht[0].table[d->rehashidx] = NULL;
        d->rehashidx++;
    }
    // 全部迁移完成 → 交换 ht[0] 和 ht[1]，重置
    if (d->ht[0].used == 0) {
        free(d->ht[0].table);
        d->ht[0] = d->ht[1];
        memset(&d->ht[1], 0, sizeof(dictht));
        d->rehashidx = -1;
    }
}
```

**渐进式 rehash 的关键设计：**
1. **分步执行**：每次增删改查操作都会顺带迁移一小批（`_dictRehashStep`）
2. **空闲时主动迁移**：`serverCron` 定时调用 `databasesCron` → `incrementallyRehash`
3. **读写同时查两张表**：rehash 期间增删改查先在 ht[0] 操作，同时迁移到 ht[1]
4. **新增只进 ht[1]**：rehash 期间新插入的数据直接放 ht[1]，ht[0] 只减不增

---

## 对比 Java HashMap

| 特性 | Redis dict | Java HashMap |
|------|-----------|-------------|
| 冲突解决 | 链地址法（头插） | 链地址 + 红黑树（≥8 转树） |
| rehash 方式 | 渐进式（不阻塞） | 一次性 rehash |
| 扩容因子 | 负载因子 ≥ 1 | 负载因子 ≥ 0.75 |
| 容量 | 2^n（用 & 取模） | 2^n |
| 缩容 | 支持（< 0.1） | 不支持（`trimToSize` 手动） |

---

## 经典题型速查 · 延伸要点 4
| 题型 | 要点 |
|------|------|
| 渐进式 rehash 如何不阻塞 | 每次操作只迁移 1 个 bucket，穿插在正常请求中 |
| rehash 期间查询流程 | 先查 ht[0]，没找到再查 ht[1] |
| 新数据写入 | rehash 期间新增到 ht[1] |
| 扩容后容量 | `>= used * 2` 的最小 2^n |
| 缩容条件 | 负载因子 < 0.1 |
| 为什么用链地址 | 不支持 O(1) 内存分配（节点独立分配），比开放地址更灵活 |

> [!tip]- **工程要点**
> dict 是 Redis 全局键空间的基础——`redisDb->dict` 存所有 key-value。渐进式 rehash 的设计思想（分批迁移、读写查两张表、新增只进新表）被许多需要在线扩容的系统借鉴。

---



压缩列表实现见 → [01a2-ziplist & listpack (压缩列表)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a2-ziplist%20&%20listpack%20(压缩列表).md) · [01a3-skiplist：Sorted Set Internals (跳表)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)



## 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

## 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Redis Data Structures (Redis 数据结构)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
