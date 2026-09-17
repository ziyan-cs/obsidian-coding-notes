---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 一句话结论：HTTP handler 的职责是把协议输入转换为明确的应用命令，并把可预期失败稳定地映射为响应；业务规则不要散落在 handler 内。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：handler 只做解析、校验、调用服务与映射响应；领域服务维护业务不变量；用统一错误码和响应体让客户端得到可预测结果。

# 请求路径

1. 路由匹配与认证。
2. 解码并校验路径、查询和 body。
3. 调用 application/service 层。
4. 把领域结果或错误映射为稳定的 HTTP status 和错误 body。
5. 记录 request ID、延迟和必要上下文。

# 错误边界

- 客户端输入错误：4xx，给调用者可修正的信息。
- 资源不存在或冲突：使用稳定语义，不暴露内部实现。
- 下游超时、数据库失败：5xx，日志保留根因与 request ID。

# 一个可测试的分层

```text
HTTP request
    -> handler: decode and validate transport input
    -> service: enforce business rules
    -> repository: read or write storage
    -> handler: map known errors to response
```

例如“标题不能为空”是业务或输入规则；`json.Decoder` 失败是协议输入错误；数据库连接断开是基础设施错误。三者不能都返回同一段模糊的 `internal error`，也不能把数据库错误原文直接交给客户端。

# 最小响应约定

```json
{"code":"invalid_argument","message":"title is required","request_id":"..."}
```

错误 `code` 应稳定、可枚举；`message` 面向调用方；日志记录根因和堆栈或错误链。先为一条成功路径、一个校验失败和一个依赖超时写 `httptest`，再接真实 router。

> [!info]- 延伸阅读
> - 下一步：[03-API Lifecycle and Graceful Shutdown (服务生命周期)](/04-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/03-API%20Lifecycle%20and%20Graceful%20Shutdown%20(服务生命周期).md)
