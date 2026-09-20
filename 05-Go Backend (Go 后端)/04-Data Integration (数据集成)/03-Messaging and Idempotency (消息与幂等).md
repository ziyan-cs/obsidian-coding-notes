---
study_stage: backlog
---

> [!abstract] 学习定位
> 至少一次投递下，重复、延迟和局部乱序都是正常输入；正确性来自业务幂等、事务边界与可恢复处理。

> [!summary] 核心摘要
>
> producer 解决“业务写入后事件不能丢”，consumer 解决“重复处理不能产生重复副作用”；ack、重试、死信和顺序都要围绕业务状态机设计。

# 从故障窗口理解投递

producer 可能“数据库提交成功、消息发送失败”，consumer 可能“业务提交成功、ack 前崩溃”。网络只能让调用方不知道结果，无法保证远端一定没执行。因此常见 broker 的可靠模式是至少一次，而不是端到端恰好一次。

```text
producer transaction
  business row + outbox event
              |
              v
publisher retries -> broker -> consumer
                                |
                       idempotency + business update
                       in one local transaction
                                |
                               ack
```

Outbox 将业务状态与待发送事件写入同一数据库事务，再由发布器反复投递未发送事件。它缩小双写漏洞，但仍要处理重复发布、表增长、轮询延迟和发布状态竞争。

# 消费者幂等

稳定的 message ID 适合去重，业务唯一键适合约束“同一业务只能发生一次”。幂等记录与业务变更必须在同一本地事务中提交：

```text
BEGIN transaction
  INSERT message_id into a table with UNIQUE(message_id)
  if duplicate-key: skip business mutation, then acknowledge according to contract
  else: apply business mutation in this same transaction
COMMIT transaction
```

这是事务步骤，不是可直接执行的 SQL。MySQL、PostgreSQL 与 SQLite 的冲突语法和返回语义不同；实现时用当前数据库的唯一约束与驱动错误分类验证“只让首次消息改变业务状态”，不要直接照搬另一种方言的 `ON CONFLICT`。

仅在内存 set 中去重，进程重启后会失效；先写幂等表、再单独改业务，仍存在中间崩溃窗口。对于支付、库存等状态机，还要验证当前状态是否允许转换，不能只依靠 message ID。

# 重试、死信与毒消息

错误应分为：

- 瞬态：网络失败、短暂限流，可退避重试。
- 永久：schema 不兼容、业务校验失败，不应无限重试。
- 未知：先保留上下文与原始消息引用，人工或隔离流程判断。

重试必须有次数或时间预算、指数退避与随机抖动。超过预算后进入死信或隔离队列；死信不是终点，必须有查看、修复、重放和审计流程。重放仍要经过同一幂等边界。

# 顺序与并发

全局有序代价很高，通常只要求同一业务键有序。选择订单 ID、用户 ID 等 partition key 可让相关事件落入同一分区，但热点键会造成倾斜。即使 broker 保序，重试与多个消费者仍可能让业务观察到旧事件，因此事件中可携带版本号，并由状态机拒绝倒退。

ack 时机决定语义：处理前 ack 可能丢失；提交后 ack 可能重复。通常选择后者，再用幂等抵抗重复。

# 运行与验证

至少监控生产失败、发布延迟、consumer lag、处理延迟、重试、死信、重复命中与每个分区的积压。部署前主动验证：

1. producer 在写库后、发消息前崩溃，事件能否恢复发布。
2. consumer 提交后、ack 前崩溃，重复消息是否安全。
3. 下游持续失败时，重试是否形成风暴。
4. 旧版本事件晚到时，状态是否会倒退。
5. 死信修复后能否受控重放。

> [!question]- 理解检查
> “broker 支持 exactly-once”为什么不自动等于数据库副作用恰好一次？请指出跨越了哪些事务边界。
