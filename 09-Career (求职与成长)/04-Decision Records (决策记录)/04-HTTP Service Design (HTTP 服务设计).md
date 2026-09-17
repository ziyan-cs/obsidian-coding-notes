---
status: learning
confidence: low
verified: 2026-09-17
tags: [comparison, backend/http]
---

> [!abstract] 学习定位：结论：无论实现语言，handler 处理协议，service 处理业务，repository 隔离外部系统；请求必须有 timeout、错误语义和可观测性。

> [!summary]- 复述检查：学完后再展开
> **回答**：handler 负责协议解析与响应映射，service 维护业务不变量，repository 隔离外部存储。请求链路还必须具有超时、取消、幂等、错误语义和可观测性。

# 必须统一的约定

- request ID、结构化日志、timeout / cancellation。
- 输入大小和 schema 校验；认证/授权边界。
- 成功与失败 JSON 结构、HTTP status、幂等语义。
- health/readiness、优雅关闭和依赖超时。

语言的差异（C++ event loop、Go `net/http`、Python client/tool）应服务于该边界，不改变这些基本责任。

# 一条请求的最小契约

| 阶段 | 必须回答的问题 |
| --- | --- |
| 输入 | schema、大小、认证和授权是否有效？ |
| 业务 | 哪个 service 拥有规则与事务边界？ |
| 依赖 | timeout、取消、重试、fallback 是否明确？ |
| 输出 | status、error code、request ID 是否稳定且安全？ |

> [!warning] 常见误区
> 把 SQL、缓存细节或第三方 SDK 直接写进 handler，会让协议层和业务层一起失去可测试性；“返回 200 + 错误字符串”也会破坏客户端语义。

> [!summary]- 复述检查：学完后再展开
> **回答**：HTTP handler 做协议转换，service 承载业务规则，repository 封装外部存储。每一层都应能单独测试；timeout、取消和可观测性从请求入口贯穿到依赖调用。
>
> - 自测：创建资源遇到重复键、依赖超时、参数非法，分别应返回什么类别的 HTTP 响应？
> - 自测：graceful shutdown 时，哪些新请求必须停止接收，哪些在途请求允许完成？

# 渐进练习

1. **第一步 · 理解**：读：先在 30 秒内给出结论和适用边界。
2. **第二步 · 实现**：写：用自己的项目或最小实验补一个真实证据，不能只引用概念。
3. **第三步 · 验证**：追问：回答“为什么不用另一种方案”，并说出代价而非只说优点。

> [!info]- 延伸阅读
> - 下一步：[05-Observability and Performance (可观测性与性能)](/09-Career%20(求职与成长)/04-Decision%20Records%20(决策记录)/05-Observability%20and%20Performance%20(可观测性与性能).md)
