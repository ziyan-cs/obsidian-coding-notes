---
tags:
  - database/innodb
status: 🌱
---

> [!important] **核心考点**：redo log 物理日志记录页修改、崩溃恢复前滚、checkpoint 机制与循环写

## Redo Log 的物理结构

Redo Log 是**物理日志**——记录的是"在某个页的某个偏移量写入了什么数据"，而非 SQL 语句。

```
redo log 记录格式：
  ┌─────────┬─────────┬──────────┬──────────┐
  │  type   │ space_id│ page_no  │  data    │
  ├─────────┼─────────┼──────────┼──────────┤
  │ MLOCK_1 │    5    │   100    │ ...      │
  │ MLOCK_1 │    5    │   100    │ ...      │
  │ MLOCK_1 │    5    │   101    │ ...      │
  └─────────┴─────────┴──────────┴──────────┘

type = MLOCK_1: 写入不大于 512 字节的数据
        MLOCK_2: 写入不大于 1024 字节的数据
        ... 多种类型适配不同大小的修改

记录内容示例：
  "在 space_id=5 的 表空间，page_no=100 的页，偏移量 812 处，写入 8 字节数据"
```

**Redo Log 文件配置：**
```ini
# 日志文件大小（循环使用的一组文件）
innodb_log_file_size = 512M

# 日志文件数量
innodb_log_files_in_group = 3

# 总 redo log 空间 = 512M × 3 = 1.5GB
```

## Redo Log 的循环写入

Redo log 文件不是无限增长的——它使用**固定大小的循环缓冲区**：

```
redo log 文件组（3 个文件，循环使用）：

  File 0    File 1    File 2
  ┌──────────────────────────┐  ┌──────┐  ┌──────┐
  │      │  │      │  │      │
  │      │  │      │  │      │
  │      │  │      │  │      │
  └──────────────────────────┘  └──────┘  └──────┘
   ↑                        ↑
  write_pos (当前写入位置)    checkpoint（已刷盘的安全点位）

  当 write_pos 追到 checkpoint 时 → 强制刷脏页 → 推进 checkpoint
```

## Checkpoint 机制

Checkpoint 解决了两个问题：
1. **缩减恢复时间**：不需要重放所有 redo log，只需重放 checkpoint 之后的
2. **重用 redo log 空间**：checkpoint 之前的 redo log 可以被覆盖

```
崩溃恢复范围：
  ┌─────────────────────────────────────────────┐
  │  checkpoint →→→→→ crash →→→→→→→→→→→→→       │
  │  │                    │                     │
  │  脏页已全部刷盘        需要重放的 redo log  │
  │  （无需恢复）           （前滚恢复）        │
  └─────────────────────────────────────────────┘
```

**InnoDB 的 Fuzzy Checkpoint：**
- 不需要每次 checkpoint 刷所有脏页（不像 Oracle 的完全 checkpoint）
- 渐进式推进 checkpoint LSN（Log Sequence Number）
- Page Cleaner 线程后台持续刷脏页
- 当 redo log 空间即将用尽时，加速刷脏页

## 崩溃恢复流程

```
MySQL 崩溃后重启的恢复流程：

1. 定位 checkpoint
   └→ 从 redo log 文件头读取最近一次 checkpoint 的 LSN

2. 前滚（Redo Phase）
   └→ 从 checkpoint LSN 开始，按顺序重放 redo log
   └→ 将未刷盘的修改重新应用到 Buffer Pool
   └→ 这个过程是幂等的——重复应用相同 redo log 不影响正确性

3. 回滚（Undo Phase）
   └→ 检查所有事务，查找 crash 时未提交的事务
   └→ 通过 undo log 回滚这些未完成的事务
   └→ 释放锁等资源

4. 完成
   └→ Buffer Pool 中的脏页在恢复完成后刷新到磁盘
```

**恢复状态机：**
```
MySQL 启动 → 检查是否需要恢复 → 否 → 正常启动
                                ↓
                              是
                                ↓
                   扫描 redo log → log_applied
                                ↓
                          执行前滚
                                ↓
                   扫描 undo → 查找未提交事务
                                ↓
                          执行回滚
                                ↓
                          启动完成
```

## LSN（Log Sequence Number）

LSN 是 redo log 的唯一标识，单调递增：

```
LSN 的作用：
  - 标识 redo log 中的位置
  - 每个数据页记录最近修改的 LSN（page_lsn）
  - 恢复时：page_lsn < redo_log_lsn → 需要重放
  - Checkpoint 位置由 LSN 记录

查看 LSN：
  SHOW ENGINE INNODB STATUS\G
  ---
  LOG
  Log sequence number          123456789    ← 当前写入位置
  Log flushed up to            123456000    ← 已刷盘位置
  Last checkpoint at           123400000    ← checkpoint 位置
```

> [!tip]- **工程要点**：redo log 太小会导致频繁 checkpoint（强制刷脏页），表现为 IO 尖刺和性能抖动。经验公式：redo log 总大小应能容纳 1-2 小时的写入量。监控指标：`SHOW ENGINE INNODB STATUS` 中的 `Log sequence number` 与 `Last checkpoint at` 的差距不应持续超过 redo log 总大小的 70%。恢复时间与 redo log 总大小成正比——512×3=1.5GB 的 redo log 恢复时间通常在 5 分钟内。

---



WAL机制详解见 → [WAL：Write-Ahead Logging (WAL机制)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07a-WAL：Write-Ahead%20Logging%20(WAL机制).md) · [Binlog vs Redo Log：Differences (两者区别)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07c-Binlog%20vs%20Redo%20Log：Differences%20(两者区别).md)
