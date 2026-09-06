---
status: stable
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 01-SQL Access and Transactions (SQL 访问与事务)

> [!abstract] 一句话结论：数据访问层负责连接、查询、事务边界与错误语义；事务应围绕一个完整业务不变量，而不是围绕单条 SQL。

## 30 秒回答

**核心结论**：一句话结论：数据访问层负责连接、查询、事务边界与错误语义；事务应围绕一个完整业务不变量，而不是围绕单条 SQL。


## 实践原则

1. 通过参数化查询传递输入，杜绝字符串拼 SQL。
2. 让 context 的 deadline 传入数据库调用。
3. 在 service 层界定事务：读—校验—写必须共同成功或失败。
4. 明确重试条件；死锁或瞬态网络错误可重试，唯一键冲突通常不是。

## 事务的最小骨架

```go
tx, err := db.BeginTx(ctx, nil)
if err != nil { return err }
defer tx.Rollback() // 未提交时回滚；提交后回滚无效

if _, err := tx.ExecContext(ctx, `UPDATE account SET balance = balance - ? WHERE id = ?`, 10, from); err != nil { return err }
if _, err := tx.ExecContext(ctx, `UPDATE account SET balance = balance + ? WHERE id = ?`, 10, to); err != nil { return err }
return tx.Commit()
```

示例只说明边界：两次写必须一起成功。真实转账还要检查余额、行数、隔离级别、幂等键与审计记录；金额应使用定点整数或 decimal，而不是浮点数。

## 失败路径

| 现象 | 首先检查 | 常见处理 |
| --- | --- | --- |
| `context deadline exceeded` | SQL、连接池是否排队 | 缩小事务、设置合理 deadline、定位慢查询 |
| deadlock | 加锁顺序是否不一致 | 有上限地重试完整事务 |
| 唯一键冲突 | 是否重复创建 | 返回冲突，或按业务键查询已有结果 |

## 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
01-SQL Access and Transactions (SQL 访问与事务)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
