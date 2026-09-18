---
status: learning
confidence: low
content_verified: 2026-09-17
tags: [project/flash-sale, distributed/consistency]
---

> [!abstract] 学习定位
> 秒杀的首要目标是守住库存与订单幂等不变量；限流和异步化用于把不可承受的高峰变成可控的拒绝、排队和恢复过程。

> [!summary] 核心摘要
>
> 任何库存决策必须有一个可证明的原子真相来源；缓存预扣、数据库订单和消息投递的失败窗口必须设计补偿与去重。

# 关键路径

```text
request → auth / rate limit → stock decision → order event → consumer → durable order
```

# 设计问题

| 风险 | 第一原则 |
| --- | --- |
| 超卖 | 库存扣减必须有原子不变量 |
| 重复请求 | 幂等 key 和唯一订单约束 |
| 瞬时高峰 | 限流、队列、快速失败或排队语义 |
| 消息丢失/重复 | 定义投递语义，消费者可去重 |
| 缓存与 DB 不一致 | 明确 source of truth 与恢复路径 |

# 原子库存扣减示例

```sql
UPDATE sku_stock
SET available = available - 1
WHERE sku_id = ? AND available > 0;
```

受影响行数为 1 才代表本次扣减成功；为 0 表示售罄或 SKU 不存在，业务层需要区分并返回稳定结果。这个最小做法能守住不为负的数据库不变量，但高峰下仍需要限流、连接保护和异步后续流程。

# 状态机与事实来源

第一版先用数据库条件更新守住库存，再验证瓶颈是否需要 Redis 预扣。核心表至少有活动库存和订单：

```text
order: pending -> confirmed
             \-> failed / canceled

stock: available >= 0
unique(user_id, activity_id, sku_id)  // 每人限购一件的示例
```

幂等键、用户与活动唯一约束共同防重复下单。仅在入口用 Redis `SETNX` 不足以作为最终正确性，因为缓存丢失、过期或切换后仍可能重复进入数据库。

# 从数据库基线演进

阶段一在单个数据库事务内创建订单并执行条件扣减，记录连接池等待、锁等待和吞吐上限。只有证据显示数据库无法承受目标峰值后，才引入排队或 Redis 原子预扣。

```text
request -> auth / per-user limit / global admission
        -> atomic stock decision
        -> durable order intent or event
        -> asynchronous order creation
        -> query result by request/order id
```

如果 Redis 负责高峰库存决策，必须说明它与数据库库存如何对账。预扣成功但事件未发布，可用库存操作与 outbox/stream 写入的原子脚本或可恢复记录缩小窗口；consumer 创建订单失败时，补偿不能简单无条件加库存，否则重复补偿会产生虚假库存。补偿本身也要幂等并关联原操作 ID。

# 限流与排队语义

入口先过滤活动未开始、无权限、重复请求和明显非法参数，再使用令牌桶或固定容量队列保护下游。限流返回明确结果，不让客户端无限立即重试；可提供带抖动的重试建议或异步查询订单状态。

排队不等于无限接收。队列达到上限时快速失败，消费者速率按数据库承载能力配置。热点活动需要隔离队列、连接与指标，避免一个 SKU 占满所有资源。

# 验证不变量而不是只看 QPS

压测结束后执行数据库断言：库存不小于零；成功订单数与扣减一致；同一用户没有重复订单；所有已接受请求最终有确定状态。再检查 P95/P99、拒绝率、队列深度、consumer lag、数据库锁等待和恢复时间。

故障矩阵：库存决策后进程崩溃、消息重复、consumer 提交后 ack 丢失、数据库慢、Redis 切换、补偿重复执行。每个实验先写预期状态，再比对实际记录与指标。

性能报告应分别展示无限制基线、加入 admission control 后的稳定吞吐，以及异步化后的用户可见延迟。拒绝大量请求换来的高吞吐不能只报成功路径 QPS。

# 验收

- [ ] 写出“库存永不小于零”的不变量和测试。
- [ ] 用压测观察限流前后 p95、错误率与数据库负载。
- [ ] 故意注入 consumer 失败，记录恢复与重复消费结果。

> [!summary]- 项目表达检查：学完后再展开
>
> 秒杀系统的首要目标是守住库存不变量，而不是让所有请求成功。入口限流保护依赖，库存决策必须可证明原子，订单异步化后要用幂等消费和可恢复的事件路径处理重复与失败。
>

> [!warning] 常见误区
> Redis 预扣库存、数据库扣减、消息投递不是三件独立操作；必须说明它们的顺序、失败补偿和最终 source of truth。

> [!info]- 延伸阅读
> - 下一步：[04-Config Center (配置中心)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/04-Config%20Center%20Project%20(配置中心项目).md)

