---
status: learning
confidence: high
content_verified: 2026-09-18
tags: [language/go, backend/workflow]
---

> [!abstract] 学习定位
> 本篇描述 Go 服务从业务规则到生产运行的长期交付路径，不规定首月周次。当前任务由 Hub 的 Study Cycle 选择。

> [!summary] 核心摘要
>
> Go 实践先把业务规则与协议分离，再逐层加入取消、并发、数据、测试、观测和关闭语义；每一层都要用故障实验说明系统如何失败和恢复。

# 业务核心与包边界

先实现不依赖 HTTP 的 service 与内存 repository，写清业务不变量和错误类型。package 按业务能力组织，`cmd` 只负责装配；interface 由使用方定义，只抽象真正需要替换的边界。

# 协议与并发

handler 限制 body、严格解析 JSON、校验字段并映射稳定错误。共享状态先定义所有权，再选择 mutex 或 channel；所有 goroutine 都有退出条件、取消来源和有界队列。使用 race detector 证明测试路径中没有已发现竞态。

# 数据与一致性

数据库操作携带 context deadline，事务边界由业务用例决定。缓存先定义真相来源和失效窗口；消息消费假设重复与延迟存在，用业务键、唯一约束或状态机实现幂等。

# 运行与关闭

服务记录结构化日志、请求指标和关联 ID。SIGTERM 到达后先停止接收新流量，再排空请求、停止 worker、关闭依赖，并受总 deadline 控制。数据库超时、缓存不可用、重复请求和下游变慢都应有明确降级或失败行为。

# 交付证据

完成的 Go 服务应能在干净环境执行 `go test ./...`、race test、vet、构建、启动、冒烟、压测和关闭验证。README 记录配置、迁移、运行和故障演练；具体学习安排只维护在 Study Cycle。

