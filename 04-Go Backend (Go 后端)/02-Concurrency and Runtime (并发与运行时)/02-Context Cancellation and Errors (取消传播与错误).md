---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Context Cancellation and Errors (取消传播与错误)

> [!abstract] 一句话结论：`context.Context` 用来沿调用链传递取消、截止时间与请求范围元数据；它不是任意参数包，也不是可选的全局变量。

## 30 秒回答

`context.Context` 让请求的取消和 deadline 从入口一致地传到数据库、HTTP、RPC、worker 与 goroutine。它解决的是工作何时应停止，不承担业务参数传递；每个派生 goroutine 都必须有退出路径，并在阻塞点检查 `Done()` 或使用支持 context 的 API。

## 取消传播检查表

1. handler 是否将 request context 传给 service？
2. service 是否将它传给数据库、HTTP client、消息操作？
3. worker 的 `select` 是否监听 `ctx.Done()`？
4. 超时后是否停止重试和后台写入，而不是仅向客户端返回错误？

## 常见误区

- 用 `context.Background()` 在请求链中重新开始，丢失取消信号。
- 把数据库连接、logger 或可选业务字段塞入 context。
- goroutine 启动后没有等待、取消或关闭协议，形成泄漏。

## 使用规则

1. 请求入口创建或接收 context，向下游调用继续传递。
2. I/O 调用设置 deadline，并在循环、worker 和 select 中响应取消。
3. 返回错误时保留语义与上下文；调用方据此决定重试、降级或返回状态码。
4. 不把业务可选参数塞进 context；它们应显式出现在函数签名或配置结构中。

## 自测

客户端断开后，一个正在访问数据库与消息队列的请求为何还会继续消耗资源？怎样让它停止？

## 学习闭环

### 复述

- 不看正文，说明 02-Context Cancellation and Errors (取消传播与错误) 的问题、核心机制与边界。

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
