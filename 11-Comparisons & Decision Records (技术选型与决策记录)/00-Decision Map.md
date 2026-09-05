---
tags: [comparison, architecture/decision]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Decision Map — 把“会什么”变成“为什么这样选”

> **一句话结论**：技术选型没有脱离约束的正确答案；决策记录应写清上下文、候选项、权衡、结论和可逆性。

## 使用场景

- C++、Go、Python 如何分工。
- mutex 还是 channel，缓存还是数据库直读，同步还是异步。
- 什么时候引入 Redis、MQ、gRPC、分库分表。

## 决策模板（ADR-lite）

```md
# Decision: <标题>
- Date:
- Context: 当前约束、量级、团队、已有系统。
- Options: A / B / C。
- Decision: 选择什么。
- Consequences: 得到什么、失去什么、后续成本。
- Revisit trigger: 什么变化后需要重审。
```

## 规则

- 没有真实约束时，写“学习实验”，不要伪装成生产决策。
- 区分事实、推断和个人偏好。
- 对版本、框架、性能数据写来源与验证日期。

## 关联

- [[01-Resource Lifetime]]
- [[02-Concurrency Models]]
- [[03-Error Handling]]
- [[04-HTTP Service Design]]
- [[05-Observability & Performance]]
