---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 00-MySQL and SQL Map (MySQL 与 SQL 导航)

> [!abstract] 阅读定位
>
> MySQL 学习从“写出查询”升级到“解释数据如何读写、事务如何隔离、慢查询如何被证据定位”。

## 学习顺序

1. `01–04`：SQL、连接查询、聚合和 MySQL 基础。
2. `05–08`：InnoDB 页、Buffer Pool 与索引。
3. `09–17`：隔离级别、MVCC、锁、死锁和日志恢复。
4. `18–24`：执行计划、索引设计、慢查询、连接池、复制、分片与缓存。

## 实战原则

先用 `EXPLAIN`、慢日志和真实数据规模定位查询问题；事务与索引设计必须从业务不变量和并发读写路径出发。
