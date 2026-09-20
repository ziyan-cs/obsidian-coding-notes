---
study_stage: backlog
---

> [!abstract] 学习定位
> `sql.DB` 是并发安全的连接池句柄；service 定义业务事务，repository 负责查询、扫描与数据库错误边界。

> [!summary] 核心摘要
>
> 参数化 SQL 防注入，context 控制等待预算，事务围绕一个业务不变量；所有 rows、提交失败、连接池排队和可重试错误都必须显式处理。

# 连接池与调用生命周期

`sql.Open` 通常只建立句柄并初始化池，不保证数据库已可达；启动阶段需要连通性验证时使用 `PingContext`。为池配置最大打开连接、空闲连接、连接寿命与空闲时间时，要结合数据库容量和服务实例数，不能每个实例都占满数据库上限。

一次查询大致经历：等待池中连接、发送 SQL、等待数据库执行、读取结果、归还连接。`QueryContext` 等 API 会传递取消与 deadline，但具体驱动、网络和数据库对正在执行操作的中断能力仍需集成测试；不要仅凭函数名断言每一步都能立即停止。

# 查询与资源释放

```go
rows, err := db.QueryContext(ctx,
    `SELECT id, name FROM user WHERE status = ?`, "active")
if err != nil { return err }
defer rows.Close()

for rows.Next() {
    var id int64
    var name string
    if err := rows.Scan(&id, &name); err != nil { return err }
}
if err := rows.Err(); err != nil { return err }
```

始终参数化查询，参数占位符语法由 driver 决定。多行查询要关闭 rows 并检查迭代后的 `rows.Err()`；单行查询的错误通常在 `Scan` 时出现。数据库的 NULL 与 Go 零值不同，应使用 nullable 类型或在 schema/SQL 中消除歧义。

# 事务围绕业务不变量

```go
func transfer(ctx context.Context, db *sql.DB, from, to int64, amount int64) error {
    tx, err := db.BeginTx(ctx, nil)
    if err != nil { return err }
    defer tx.Rollback()

    // 锁定并校验余额；检查每次写入影响行数。
    // 金额使用最小货币单位或 decimal，不使用 float。
    if err := applyTransfer(ctx, tx, from, to, amount); err != nil {
        return err
    }
    return tx.Commit()
}
```

`defer tx.Rollback()` 用于覆盖提前返回；提交成功后再次回滚不会撤销提交。不能在同一事务中混用 `db` 查询，否则可能拿到另一连接并破坏原子边界。隔离级别、锁和重试策略必须与数据库实际语义配合。

# 错误分类与重试

| 现象 | 先检查 | 处理方向 |
| --- | --- | --- |
| deadline exceeded | 慢 SQL、锁等待、池排队 | 缩小事务、优化查询或容量 |
| deadlock/序列化失败 | 加锁顺序与冲突事务 | 有上限地重试整个事务 |
| 唯一键冲突 | 是否重复业务操作 | 转成业务冲突或返回既有结果 |
| 连接错误 | 数据库与网络状态 | 仅在幂等边界内重试 |

重试单条 SQL 可能破坏业务不变量；若错误使事务失效，应回滚并重跑完整事务。使用业务幂等键和唯一约束处理“提交成功但响应丢失”的不确定结果。

# 测试与观察

repository 单元测试可验证 SQL 参数与错误转换，集成测试应使用与生产一致的数据库类型验证事务、隔离和约束。运行时监控池等待次数、等待时长、使用中/空闲连接、查询延迟与数据库错误分类。

> [!info]- 官方参考
> - [Accessing relational databases](https://go.dev/doc/database/)
> - [Canceling in-progress database operations](https://go.dev/doc/database/cancel-operations)
