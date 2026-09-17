---
status: stable
confidence: medium
verified: 2026-09-17
---

> [!abstract] 一句话结论：可靠消息系统通常提供至少一次投递，因此消费者必须把重复、延迟与乱序当成正常输入。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：生产端用可靠投递或 outbox 缩小丢失窗口；消费者用业务唯一键、去重记录或幂等写入抵抗重复；按业务定义乱序和重试的处理规则。

# 消费者基线

- 使用唯一业务键或幂等记录识别重复请求。
- 成功处理后再确认消费；失败要区分可重试和不可重试。
- 将数据库状态变更与待发布事件的关系显式设计，避免“写库成功、发消息失败”的双写漏洞。
- 监控积压、失败率、重试次数和死信。

# 幂等的实现位置

```text
message(id, business_key)
          |
          v
transaction: record processed key + apply state change
          |
          +-- duplicate key -> already processed -> acknowledge safely
```

不要用“消息内容看起来一样”判断重复，应选择稳定的 message ID 或业务唯一键。幂等记录与业务写入要在同一事务中提交；否则进程在两步之间崩溃，仍会造成重复副作用。

# 你需要主动问的问题

1. producer 重试后，消费者会看到什么？
2. consumer 提交数据库但还未来得及 ack 时崩溃，会怎样？
3. 同一业务键乱序到达时，状态机允许哪些转换？
