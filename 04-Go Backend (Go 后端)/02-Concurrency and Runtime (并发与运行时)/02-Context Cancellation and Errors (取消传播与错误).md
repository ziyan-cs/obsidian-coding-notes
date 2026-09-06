---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Context Cancellation and Errors (取消传播与错误)

> [!abstract] 一句话结论：`context.Context` 用来沿调用链传递取消、截止时间与请求范围元数据；它不是任意参数包，也不是可选的全局变量。

## 使用规则

1. 请求入口创建或接收 context，向下游调用继续传递。
2. I/O 调用设置 deadline，并在循环、worker 和 select 中响应取消。
3. 返回错误时保留语义与上下文；调用方据此决定重试、降级或返回状态码。
4. 不把业务可选参数塞进 context；它们应显式出现在函数签名或配置结构中。

## 自测

客户端断开后，一个正在访问数据库与消息队列的请求为何还会继续消耗资源？怎样让它停止？
