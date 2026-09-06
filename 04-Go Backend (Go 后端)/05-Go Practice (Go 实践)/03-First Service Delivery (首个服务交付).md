---
status: learning
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 03-First Service Delivery (首个服务交付)

> [!abstract] 一句话结论：第一个 Go 服务的目标不是堆功能，而是完整走通 API、数据、测试、观测、部署与复盘。

## 推荐范围

实现一个带用户资源或短链接资源的 HTTP 服务：CRUD、参数校验、MySQL 持久化、Redis 缓存、结构化日志、优雅关闭和基础压测。

## 验收清单

- `go test` 可运行，至少覆盖一个正常与一个失败路径。
- 一个请求能在日志中通过 request ID 被定位。
- 服务被停止时不再接收新请求，并在限定时间处理已接收请求。
- README 写清启动、配置、测试和一次性能测量方法。
