---
tags: [project/template, backend/design]
status: solid
verified: 2026-09-05
---

# Project Design Template — 每个项目先写这张纸

```md
---
tags: [project/backend]
status: seed
---
# 项目名
> 一句话：为谁解决什么问题；不承诺解决什么。

## 用户故事与非目标
## API / 事件契约
## 数据模型与容量假设
## 架构与请求路径
## 一致性、失败与重试
## 安全与权限边界
## 可观测性：logs / metrics / traces
## 测试、压测与结果
## 已放弃的方案与理由
## 面试表达：30 秒 / 3 分钟 / 深挖问题
```

## 规则

- “使用 Redis/Kafka”不是设计理由；先说明瓶颈或故障模型。
- 每个异步流程必须定义：谁投递、谁消费、如何重试、如何去重、最终怎样观测。
- 性能结论必须写负载、机器/环境、指标和基线。
- 未实现的设计写“计划”，已测过的写“结果”，不要混淆。
