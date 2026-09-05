---
tags:
  - redis/core
status: seed
review_due: 2026-10-10
confidence: 1
verified: stable
---

> [!important] **核心考点**
> dict 结构、rehash 触发条件、渐进式 rehash 如何避免阻塞、与 Java HashMap 区别

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

## 经典题型速查

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



压缩列表实现见 → [01a2-ziplist & listpack (压缩列表)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a2-ziplist%20&%20listpack%20(压缩列表).md) · [01a3-skiplist：Sorted Set Internals (跳表)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)
