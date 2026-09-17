---
status: learning
confidence: low
verified: 2026-09-17
tags: [project/flash-sale, distributed/consistency]
---

> [!abstract] 一句话结论：秒杀的首要目标是守住库存与订单幂等不变量；限流和异步化用于把不可承受的高峰变成可控的拒绝、排队和恢复过程。

> [!summary]- 复述检查：学完后再展开
>
> **核心结论**：任何库存决策必须有一个可证明的原子真相来源；缓存预扣、数据库订单和消息投递的失败窗口必须设计补偿与去重。

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

# 验收

- [ ] 写出“库存永不小于零”的不变量和测试。
- [ ] 用压测观察限流前后 p95、错误率与数据库负载。
- [ ] 故意注入 consumer 失败，记录恢复与重复消费结果。

> [!summary]- 项目表达检查：学完后再展开
>
> 秒杀系统的首要目标是守住库存不变量，而不是让所有请求成功。入口限流保护依赖，库存决策必须可证明原子，订单异步化后要用幂等消费和可恢复的事件路径处理重复与失败。
>
> > [!warning] 常见误区
> > Redis 预扣库存、数据库扣减、消息投递不是三件独立操作；必须说明它们的顺序、失败补偿和最终 source of truth。

> [!info]- 延伸阅读
> - 下一步：[04-Config Center (配置中心)](/08-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/04-Config%20Center%20(配置中心).md)
