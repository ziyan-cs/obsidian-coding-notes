---
tags: [project/flash-sale, distributed/consistency]
status: seed
review_due: 2026-10-05
confidence: 1
verified: 2026-09-05
---

# Flash Sale — 高峰流量下的正确性

> **目标**：宁可明确失败或排队，也不能超卖、重复下单或把数据库打穿。

> 面试/系统设计参考：[[13-Career Prep (面试与成长)/02-System Design Cases (系统设计案例)/98-Flash-Sale (秒杀系统设计)|98 秒杀系统设计]]。

## 关键路径

```text
request → auth / rate limit → stock decision → order event → consumer → durable order
```

## 设计问题

| 风险 | 第一原则 |
| --- | --- |
| 超卖 | 库存扣减必须有原子不变量 |
| 重复请求 | 幂等 key 和唯一订单约束 |
| 瞬时高峰 | 限流、队列、快速失败或排队语义 |
| 消息丢失/重复 | 定义投递语义，消费者可去重 |
| 缓存与 DB 不一致 | 明确 source of truth 与恢复路径 |

## 验收

- [ ] 写出“库存永不小于零”的不变量和测试。
- [ ] 用压测观察限流前后 p95、错误率与数据库负载。
- [ ] 故意注入 consumer 失败，记录恢复与重复消费结果。
