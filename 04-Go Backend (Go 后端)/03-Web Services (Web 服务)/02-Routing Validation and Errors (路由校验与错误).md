---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Routing Validation and Errors (路由校验与错误)

> [!abstract] 一句话结论：HTTP handler 的职责是把协议输入转换为明确的应用命令，并把可预期失败稳定地映射为响应；业务规则不要散落在 handler 内。

## 30 秒回答

**核心结论**：一句话结论：HTTP handler 的职责是把协议输入转换为明确的应用命令，并把可预期失败稳定地映射为响应；业务规则不要散落在 handler 内。


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

## 一个可测试的分层

```text
HTTP request
    -> handler: decode and validate transport input
    -> service: enforce business rules
    -> repository: read or write storage
    -> handler: map known errors to response
```

例如“标题不能为空”是业务或输入规则；`json.Decoder` 失败是协议输入错误；数据库连接断开是基础设施错误。三者不能都返回同一段模糊的 `internal error`，也不能把数据库错误原文直接交给客户端。

## 最小响应约定

```json
{"code":"invalid_argument","message":"title is required","request_id":"..."}
```

错误 `code` 应稳定、可枚举；`message` 面向调用方；日志记录根因和堆栈或错误链。先为一条成功路径、一个校验失败和一个依赖超时写 `httptest`，再接真实 router。

## 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
02-Routing Validation and Errors (路由校验与错误)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
