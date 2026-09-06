---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 一句话结论：HTTP handler 的职责是把协议输入转换为明确的应用命令，并把可预期失败稳定地映射为响应；业务规则不要散落在 handler 内。

# 30 秒回答

**核心结论**：一句话结论：HTTP handler 的职责是把协议输入转换为明确的应用命令，并把可预期失败稳定地映射为响应；业务规则不要散落在 handler 内。

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

# 从零建立模型

本页主题是 **02-Routing Validation and Errors (路由校验与错误)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 渐进练习

1. **第一步 · 理解**：读：标出本页代码中错误向上返回、资源释放和 goroutine 退出的位置。
2. **第二步 · 实现**：写：为一个纯业务函数补 table-driven test；若有并发，写一个取消或关闭案例。
3. **第三步 · 验证**：测：运行 `go test`，并在适用时运行 `go test -race` 或 benchmark，记录结论与环境。

# 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Routing Validation and Errors (路由校验与错误)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
