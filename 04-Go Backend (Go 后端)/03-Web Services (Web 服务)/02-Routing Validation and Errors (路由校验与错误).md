---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Routing Validation and Errors (路由校验与错误)

> [!abstract] 一句话结论：HTTP handler 的职责是把协议输入转换为明确的应用命令，并把可预期失败稳定地映射为响应；业务规则不要散落在 handler 内。

## 请求路径

1. 路由匹配与认证。
2. 解码并校验路径、查询和 body。
3. 调用 application/service 层。
4. 把领域结果或错误映射为稳定的 HTTP status 和错误 body。
5. 记录 request ID、延迟和必要上下文。

## 错误边界

- 客户端输入错误：4xx，给调用者可修正的信息。
- 资源不存在或冲突：使用稳定语义，不暴露内部实现。
- 下游超时、数据库失败：5xx，日志保留根因与 request ID。
