---
tags:
  - database/innodb
status: 🌱
---

> [!important] **核心考点**：InnoDB 页结构（数据页/索引页/undo 页）、Buffer Pool 缓存机制与 LRU 管理

## InnoDB 页结构

InnoDB 以**页（Page）** 为最小存储单位，默认 16KB。

```text
InnoDB Data Page (16KB)
├── File Header (38B, page number, prev/next page pointers, LSN)
├── Page Header (56B, record count, offset array)
├── Infimum (boundary min)
├── User Records (row data)
├── Supremum (boundary max)
├── Page Directory (slot array, binary search)
└── File Trailer (8B, checksum)

Leaf Node Doubly Linked List (via File Header pointers):
  Prev Page ◄──────────────────────► Next Page
```

**页类型：**
| 页类型 | 作用 |
|--------|------|
| 数据页（INDEX） | 存储表数据和索引 |
| Undo 页（UNDO_LOG） | 存储 undo log |
| 系统页（SYSTEM） | 系统表空间元数据 |
| 事务系统页（TRX_SYS） | 事务系统信息 |
| 插入缓冲位图页（IBUF_BITMAP） | 插入缓冲管理 |
| 插入缓冲空闲列表页（IBUF_FREE_LIST） | |
| 压缩页（COMPRESSED） | 压缩后的数据页 |

## Buffer Pool 缓存池

Buffer Pool 是 InnoDB 在内存中的页缓存，所有读写操作都通过 Buffer Pool 进行。

```
磁盘 ←→ Buffer Pool ←→ CPU/查询引擎
         (内存缓存)

读：先从 BP 找 → 找不到则从磁盘加载到 BP
写：先写 BP（脏页）→ 后台线程刷到磁盘
```

**核心参数：**
```ini
# 缓冲池大小（通常设为物理内存的 60-80%）
innodb_buffer_pool_size = 8G

# 实例数（减少锁竞争，MySQL 5.5+）
innodb_buffer_pool_instances = 8

# 每个实例 ≈ 8GB / 8 = 1GB
```

## LRU 管理

InnoDB 使用改进的 LRU 算法管理 Buffer Pool，将链表分为**年轻代（new）** 和**老年代（old）**：

```
改进的 LRU 链表：

  [最新访问] ← → → → → → → → → [最久未访问]
   ┌──────────────┬──────────────┐
   │  年轻代 5/8   │ 老年代 3/8   │
   └──────────────┴──────────────┘
                  ↑
              midpoint（分界点）

读入新页时：
  1. 先插入到老年代头部（midpoint 位置）
  2. 如果页被再次访问且停留在老年代时间 > innodb_old_blocks_time
      → 晋升到年轻代头部
  3. 否则被淘汰（防止大表扫描污染缓存）
```

**innodb_old_blocks_time 的作用（默认 1000ms）：**
防止全表扫描或大量数据导入时把热点数据挤出 Buffer Pool。

## 脏页刷盘

```sql
-- 查看 Buffer Pool 状态
SHOW ENGINE INNODB STATUS\G

-- 关键指标：
-- Buffer pool hit rate: 998 / 1000    ← 命中率 > 99% 正常
-- Young making rate: 5%
-- Modified db pages: 0                ← 脏页数
```

**刷盘策略参数：**
```ini
# 脏页比例触发刷盘
innodb_max_dirty_pages_pct = 75

# 刷脏页线程数量
innodb_page_cleaners = 8

# Adaptive Flushing：根据 redo log 生成速度动态调整刷盘速率
# 由 innodb_adaptive_flushing = ON 控制
```

> [!tip]- **工程要点**：Buffer Pool 命中率是数据库性能的核心指标——命中率 > 99% 意味着内存够大，磁盘 IO 压力小。如果命中率持续低于 95%，考虑增大 innodb_buffer_pool_size。改进型 LRU 的 old_blocks_time 机制对大表扫描场景非常关键，设置为 1000ms 可以有效减少扫描对缓存的冲击。

---



B+树索引结构详解见 → [B+ Tree Index Structure (B+树索引结构)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04b-B+%20Tree%20Index%20Structure%20(B+树索引结构).md)
