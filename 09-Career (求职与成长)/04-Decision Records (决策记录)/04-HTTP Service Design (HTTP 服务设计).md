---
tags: [comparison, backend/http]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 04-HTTP Service Design (HTTP 服务设计)

> [!abstract] 阅读定位
>
> 结论：无论实现语言，handler 处理协议，service 处理业务，repository 隔离外部系统；请求必须有 timeout、错误语义和可观测性。

## 30 秒回答

**04-HTTP Service Design (HTTP 服务设计)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## 必须统一的约定

- request ID、结构化日志、timeout / cancellation。
- 输入大小和 schema 校验；认证/授权边界。
- 成功与失败 JSON 结构、HTTP status、幂等语义。
- health/readiness、优雅关闭和依赖超时。

语言的差异（C++ event loop、Go `net/http`、Python client/tool）应服务于该边界，不改变这些基本责任。

## 一条请求的最小契约

| 阶段 | 必须回答的问题 |
| --- | --- |
| 输入 | schema、大小、认证和授权是否有效？ |
| 业务 | 哪个 service 拥有规则与事务边界？ |
| 依赖 | timeout、取消、重试、fallback 是否明确？ |
| 输出 | status、error code、request ID 是否稳定且安全？ |

> [!warning] 常见误区
> 把 SQL、缓存细节或第三方 SDK 直接写进 handler，会让协议层和业务层一起失去可测试性；“返回 200 + 错误字符串”也会破坏客户端语义。

## 30 秒回答与自测

**回答**：HTTP handler 做协议转换，service 承载业务规则，repository 封装外部存储。每一层都应能单独测试；timeout、取消和可观测性从请求入口贯穿到依赖调用。

- 自测：创建资源遇到重复键、依赖超时、参数非法，分别应返回什么类别的 HTTP 响应？
- 自测：graceful shutdown 时，哪些新请求必须停止接收，哪些在途请求允许完成？

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 04-HTTP Service Design (HTTP 服务设计) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
