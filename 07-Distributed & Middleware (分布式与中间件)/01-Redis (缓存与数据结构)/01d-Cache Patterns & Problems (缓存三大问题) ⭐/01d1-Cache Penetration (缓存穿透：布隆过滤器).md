---
tags:
  - redis/core
status: seed
review_due: 2026-10-10
confidence: 1
verified: stable
---

> [!important] **核心考点**
> 缓存穿透原因、布隆过滤器原理、误判率与哈希函数数、解决方案对比

## 缓存穿透

查询一个**根本不存在**的数据，请求绕过缓存直达数据库。

```
用户请求 → Redis（未命中）→ MySQL（没查到）→ 返回 null
          ↓
   大量请求重复相同路径 → MySQL 压力激增
```

### 解决方案

| 方案 | 原理 | 缺点 |
|------|------|------|
| **缓存空值** | 将 null 也缓存，短 TTL（30-60s） | 大量空 key 占内存 |
| **布隆过滤器** | 请求前先判断 key 是否存在 | 有误判率（不存在判为存在） |
| **参数校验** | 基本参数合法性检查 | 无法防止合法不存在的 ID |

---

## 布隆过滤器（Bloom Filter）

概率性数据结构，判断"一定不存在"和"可能存在"：

```
初始化：bitmap = [0, 0, 0, 0, 0, 0, 0, 0]  (m=8)
添加 "key1" → hash1(key1)=0, hash2(key1)=3, hash3(key1)=6
              bitmap[0]=1, bitmap[3]=1, bitmap[6]=1

bitmap: [1, 0, 0, 1, 0, 0, 1, 0]

查询 "key2" → hash1(key2)=0, hash2(key2)=2, hash3(key2)=7
              bitmap[0]=1  ✓, bitmap[2]=0 ✗ → "一定不存在"

查询 "key3" → hash1(key3)=0, hash2(key3)=3, hash3(key3)=6
              bitmap[0]=1 ✓, bitmap[3]=1 ✓, bitmap[6]=1 ✓ → "可能存在"
              ↳ 实际 key3 可能不存在（hash 碰撞导致的误判）
```

**特点：**
- **一定不存在（不允许 false negative）**：返回"不存在"时 100% 正确
- **可能存在（允许 false positive）**：返回"存在"时有误判率（假阳性）
- **无法删除**（标准 BF），可改用 Counting Bloom Filter

### 参数选择

```cpp
// 期望插入 n 个元素，期望误判率 p
// bitmap 大小 m = -n * ln(p) / (ln2)²
// 哈希函数数 k = (m/n) * ln2 ≈ 0.7 * (m/n)

// 示例：n=10⁶, p=1%
// m ≈ -10⁶ * ln(0.01) / (0.48) ≈ 10⁶ * 4.6 / 0.48 ≈ 9.58 × 10⁶ bits ≈ 1.14MB
// k = (9.58×10⁶ / 10⁶) * 0.693 ≈ 6.6 ≈ 7 个哈希函数
```

### Redis 中使用布隆过滤器

```bash
# Redis 4.0+ 通过 module 加载 bloom 插件
BF.ADD bloomfilter key1      # 添加元素
BF.EXISTS bloomfilter key1   # 检查是否存在 → (integer) 1
BF.EXISTS bloomfilter key99  # → (integer) 0
BF.RESERVE bloomfilter 0.01 10000  # 创建（误判率1%，容量10000）
```

---

## 其他方案

```cpp
// 1. 缓存空值
if (result == null) {
    redis.set(key, "NULL", 30);  // TTL=30s
}

// 2. 参数校验（简单防攻击）
if (id <= 0 || id > MAX_VALID_ID) {
    return "invalid param";
}
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 缓存穿透的本质 | 查不存在的数据，每次都穿到 DB |
| 布隆过滤器不能做什么 | 无法删除、有误判率、需要预知容量 |
| 误判率怎么控制 | 增大 bitmap 或增加哈希函数 |
| 缓存空值的风险 | 空 key 过多可设短 TTL + 定期清理 |

> [!tip]- **工程要点**
> 布隆过滤器适合"大集合 + 允许少量误判"的场景。误判的代价是多一次 DB 查询，在大部分业务中可接受。Redis module 方式省去在应用层自行实现。

---

缓存三大问题系列详解见 → [01d2-Cache Breakdown (缓存击穿：热点key失效)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d2-Cache%20Breakdown%20(缓存击穿：热点key失效).md) · [01d3-Cache Avalanche (缓存雪崩：大量key同时失效)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d3-Cache%20Avalanche%20(缓存雪崩：大量key同时失效).md)
