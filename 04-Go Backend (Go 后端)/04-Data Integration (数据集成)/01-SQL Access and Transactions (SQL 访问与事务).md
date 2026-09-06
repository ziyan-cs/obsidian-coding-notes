---
status: stable
confidence: medium
verified: 2026-09-06
---

> [!abstract] 一句话结论：数据访问层负责连接、查询、事务边界与错误语义；事务应围绕一个完整业务不变量，而不是围绕单条 SQL。

# 30 秒回答

**核心结论**：一句话结论：数据访问层负责连接、查询、事务边界与错误语义；事务应围绕一个完整业务不变量，而不是围绕单条 SQL。

# 实践原则

1. 通过参数化查询传递输入，杜绝字符串拼 SQL。
2. 让 context 的 deadline 传入数据库调用。
3. 在 service 层界定事务：读—校验—写必须共同成功或失败。
4. 明确重试条件；死锁或瞬态网络错误可重试，唯一键冲突通常不是。

# 事务的最小骨架

```go
tx, err := db.BeginTx(ctx, nil)
if err != nil { return err }
defer tx.Rollback() // 未提交时回滚；提交后回滚无效

if _, err := tx.ExecContext(ctx, `UPDATE account SET balance = balance - ? WHERE id = ?`, 10, from); err != nil { return err }
if _, err := tx.ExecContext(ctx, `UPDATE account SET balance = balance + ? WHERE id = ?`, 10, to); err != nil { return err }
return tx.Commit()
```

示例只说明边界：两次写必须一起成功。真实转账还要检查余额、行数、隔离级别、幂等键与审计记录；金额应使用定点整数或 decimal，而不是浮点数。

# 失败路径

| 现象 | 首先检查 | 常见处理 |
| --- | --- | --- |
| `context deadline exceeded` | SQL、连接池是否排队 | 缩小事务、设置合理 deadline、定位慢查询 |
| deadlock | 加锁顺序是否不一致 | 有上限地重试完整事务 |
| 唯一键冲突 | 是否重复创建 | 返回冲突，或按业务键查询已有结果 |

# 从零建立模型

本页主题是 **01-SQL Access and Transactions (SQL 访问与事务)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 渐进练习

1. **第一步 · 理解**：读：标出本页代码中错误向上返回、资源释放和 goroutine 退出的位置。
2. **第二步 · 实现**：写：为一个纯业务函数补 table-driven test；若有并发，写一个取消或关闭案例。
3. **第三步 · 验证**：测：运行 `go test`，并在适用时运行 `go test -race` 或 benchmark，记录结论与环境。

# 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-SQL Access and Transactions (SQL 访问与事务)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Data Integration Map (数据集成导航)](/04-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/00-Data%20Integration%20Map%20(数据集成导航).md)
- 下一步：[02-Redis Cache Patterns (Redis 缓存模式)](/04-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/02-Redis%20Cache%20Patterns%20(Redis%20缓存模式).md)
