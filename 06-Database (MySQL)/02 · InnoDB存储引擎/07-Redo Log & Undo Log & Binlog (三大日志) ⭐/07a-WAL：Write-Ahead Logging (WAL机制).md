> **核心考点**：WAL 预写日志保证持久性、先写日志再写数据、redo log 崩溃恢复能力

## WAL 的核心思想

**WAL（Write-Ahead Logging）：** 在将数据写入磁盘之前，先确保日志已经写入磁盘。

```
常规写入（无 WAL）：
  内存修改 → 刷脏页到磁盘 → 如果崩溃，未刷盘的数据丢失

WAL 写入：
  内存修改 → 写 redo log（顺序 IO，很快）→ 保证已落盘 → 后续刷脏页
  如果崩溃 → 从 redo log 恢复（前滚）
```

**为什么 WAL 快：**
| 操作 | redo log 写入 | 数据页写入 |
|------|-------------|-----------|
| IO 类型 | 顺序写入 | 随机写入 |
| 单次 IO 量 | 小（KB 级） | 大（16KB 页） |
| 写入位置 | 固定文件末尾 | 分散在不同位置 |
| 写入效率 | 极高 | 低（需要寻道） |

## Redo Log 的写入流程

```
UPDATE user SET balance=100 WHERE id=1;

1. Buffer Pool 中找到 id=1 的页（如果不在则从磁盘加载）
2. 修改页中 balance=100（脏页）
3. 生成 redo log 记录（记录的是"页 5 的偏移量 100 处写入 100"）
4. 将 redo log 写入 redo log buffer
5. redo log buffer → 刷入 redo log file（磁盘）
   ↓ 至此，数据已保证不丢失
6. 后台线程或其他时机刷脏页到磁盘
```

**关键点：** 第 5 步完成后，即使系统崩溃，数据也不会丢失（可以从 redo log 恢复）。第 6 步何时执行不影响数据安全性。

## Redo Log Buffer 与刷盘时机

```ini
# redo log buffer 大小
innodb_log_buffer_size = 16M

# 刷盘策略（关键参数！）
innodb_flush_log_at_trx_commit = 1
```

**innodb_flush_log_at_trx_commit 取值：**
| 值 | 行为 | 安全性 | 性能 |
|----|------|-------|------|
| 1（默认） | 每次事务提交都将 redo log 刷到磁盘 | 最高（不会丢任何事务） | 最慢 |
| 2 | 每次事务提交写入 OS cache，每秒刷盘 | 次高（OS 崩溃丢 1s 数据） | 快 |
| 0 | 每秒写入 OS cache + 刷盘 | 最低（MySQL 崩溃丢 1s 数据） | 最快 |

**工程权衡：**
```
金融/支付场景 → innodb_flush_log_at_trx_commit = 1
日志/非关键数据 → innodb_flush_log_at_trx_commit = 2
批量导入 → innodb_flush_log_at_trx_commit = 0（然后设为 1）
```

## Undo Log 的作用

Undo Log 是 WAL 的另一面——记录**修改前的数据**，用于事务回滚和 MVCC：

```
UPDATE user SET balance=100 WHERE id=1;

redo log: 记录"页 5 偏移 100 → 写入 100"（重做）
undo log: 记录"id=1 的 balance 原来 = 50"（撤销）

事务回滚时：
  读取 undo log → 将数据恢复到修改前的状态

MVCC 快照读时：
  如果需要读取旧版本 → 通过 undo log 版本链找到
```

## WAL 与 ACID 的对应

```
A（原子性）← undo log：事务回滚
C（一致性）← undo + redo：事务要么全部完成要么全部回滚
I（隔离性）← undo log：MVCC 快照读
D（持久性）← redo log：WAL 保证即使崩溃也不丢数据
```

> **工程要点**：WAL 是现代数据库的基石——通过将随机写转化为顺序写来大幅提升写入性能。`innodb_flush_log_at_trx_commit=1` 是唯一保证不丢数据的设置，对应核心交易场景。非核心场景可设为 2（容灾 1 秒数据丢失换取 5-10 倍写入性能提升）。redo log 是物理日志（记录"页上改了啥"），undo log 是逻辑日志（记录"SQL 反操作"）——两者相辅相成。
