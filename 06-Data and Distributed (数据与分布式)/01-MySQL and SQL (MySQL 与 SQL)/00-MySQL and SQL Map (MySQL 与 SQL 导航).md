---
tags: [vault/navigation, database/mysql]
status: seed
verified: 2026-09-05
---

# 00-MySQL and SQL Map (MySQL 与 SQL 导航)

> [!abstract] 阅读定位
>
> SQL 是入口，InnoDB 存储引擎与事务才是后端面试与排查的核心。

## 主题分组
- SQL 基础：[[06-Database (数据库)/01-SQL Fundamentals (SQL 基础)/01-DDL, DML, DQL (SQL基础语法)|SQL 语法]] · [[06-Database (数据库)/01-SQL Fundamentals (SQL 基础)/02-Joins & Subqueries (多表查询与子查询)|多表查询]] · [[06-Database (数据库)/01-SQL Fundamentals (SQL 基础)/03-Aggregate Functions & Group By (聚合与分组)|聚合分组]]
- InnoDB 存储引擎：[[06-Database (数据库)/02-InnoDB Storage Engine (InnoDB 存储引擎)/04-InnoDB Storage Engine (InnoDB存储引擎) ⭐/04b-B+ Tree Index Structure (B+树索引结构)|索引]] · [[06-Database (数据库)/02-InnoDB Storage Engine (InnoDB 存储引擎)/05-Transaction & ACID (事务与ACID) ⭐/05a-Isolation Levels：RU, RC, RR, Serializable (四种隔离级别)|事务/隔离]] · [[06-Database (数据库)/02-InnoDB Storage Engine (InnoDB 存储引擎)/05-Transaction & ACID (事务与ACID) ⭐/05c-MVCC Internals：undo log & read view (MVCC底层实现)|MVCC]] · [[06-Database (数据库)/02-InnoDB Storage Engine (InnoDB 存储引擎)/06-Locks In MySQL (MySQL锁机制) ⭐/06b-Gap Lock & Next-Key Lock (间隙锁与临键锁)|锁]] · [[06-Database (数据库)/02-InnoDB Storage Engine (InnoDB 存储引擎)/07-Redo Log & Undo Log & Binlog (三大日志) ⭐/07a-WAL：Write-Ahead Logging (WAL机制)|三大日志]]
- 查询优化：[[06-Database (数据库)/03-Query Optimization (查询优化)/08-Query Optimization (查询优化) ⭐/08a-EXPLAIN & Execution Plan Analysis (执行计划分析)|查询优化]] · [[06-Database (数据库)/03-Query Optimization (查询优化)/09-Connection Pool：C API & Integration (连接池实现) ⭐|连接池]]
- 高可用：[[06-Database (数据库)/04-High Availability & Architecture (高可用与架构)/10-Master-Slave Replication (主从复制原理)|主从复制]] · [[06-Database (数据库)/04-High Availability & Architecture (高可用与架构)/11-Sharding & Partitioning Overview (分库分表概念)|分库分表]] · [[06-Database (数据库)/04-High Availability & Architecture (高可用与架构)/12-MySQL vs Redis：Caching Strategy (缓存策略对比)|缓存策略]]

## 学习顺序
1. SQL 语法 → 2. 索引（B+ 树）→ 3. 事务/隔离级别 → 4. MVCC → 5. 锁 → 6. 三大日志 → 7. 查询优化 → 8. 主从/分库分表。

## 与 C++ / 项目入口
- 缓存：[[07-Distributed & Middleware (分布式与中间件)/01-Redis (缓存与数据结构)/01d-Cache Patterns & Problems (缓存三大问题) ⭐/01d1-Cache Penetration (缓存穿透：布隆过滤器)|Redis 缓存]]
- 项目：[[12-Backend Projects (后端项目)/02-URL Shortener (短链接服务)|URL Shortener]] · [[12-Backend Projects (后端项目)/04-Flash Sale (秒杀系统)|Flash Sale]]
