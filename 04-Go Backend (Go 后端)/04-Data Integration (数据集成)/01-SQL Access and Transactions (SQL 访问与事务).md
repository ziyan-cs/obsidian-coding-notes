---
status: stable
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 01-SQL Access and Transactions (SQL 访问与事务)

> [!abstract] 一句话结论：数据访问层负责连接、查询、事务边界与错误语义；事务应围绕一个完整业务不变量，而不是围绕单条 SQL。

## 实践原则

1. 通过参数化查询传递输入，杜绝字符串拼 SQL。
2. 让 context 的 deadline 传入数据库调用。
3. 在 service 层界定事务：读—校验—写必须共同成功或失败。
4. 明确重试条件；死锁或瞬态网络错误可重试，唯一键冲突通常不是。
