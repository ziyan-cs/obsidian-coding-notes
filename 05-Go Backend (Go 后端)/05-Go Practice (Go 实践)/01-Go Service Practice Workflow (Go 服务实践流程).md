---
study_stage: backlog
tags: [language/go, backend/workflow]
---

> [!abstract] 学习定位
> 本篇描述 Go 服务从业务规则到生产运行的长期交付路径，不规定首月周次。当前任务由 Hub 的 Study Cycle 选择。

> [!summary] 核心摘要
>
> Go 实践先把业务规则与协议分离，再逐层加入取消、并发、数据、测试、观测和关闭语义；每一层都要用故障实验说明系统如何失败和恢复。

# 业务核心与包边界

先实现不依赖 HTTP 的 service 与内存 repository，写清业务不变量和错误类型。package 按业务能力组织，`cmd` 只负责装配；interface 由使用方定义，只抽象真正需要替换的边界。

从空目录开始时，先记录工具链版本，再初始化 module：

```bash
go version
go mod init example.com/your-service
go mod tidy
go test ./...
```

`example.com/your-service` 是占位 module path，真实项目应使用自己控制的导入路径。先写一个内存实现与 table-driven test，让业务规则在没有 HTTP、MySQL、Redis 的条件下能被验证。每加入一种外部依赖，都保留一条不依赖该服务的单元测试路径。

# 协议与并发

handler 限制 body、严格解析 JSON、校验字段并映射稳定错误。共享状态先定义所有权，再选择 mutex 或 channel；所有 goroutine 都有退出条件、取消来源和有界队列。使用 race detector 证明测试路径中没有已发现竞态。

每个新接口先列出成功、非法输入、不存在、冲突、依赖超时五类响应，标明 HTTP status、稳定的机器可读错误码与是否允许重试。用 `httptest` 发真实 HTTP 请求验证，不要只直接调用 service。若 handler 启动后台工作，明确何时脱离请求 context、由谁追踪与等待；默认不要在响应返回后留下无人管理的 goroutine。

# 数据与一致性

数据库操作携带 context deadline，事务边界由业务用例决定。缓存先定义真相来源和失效窗口；消息消费假设重复与延迟存在，用业务键、唯一约束或状态机实现幂等。

先验证无缓存读写，再加 Redis；这样 Redis 故障时才有明确的正确性基线。事务测试要接真实数据库，覆盖唯一约束冲突、回滚和提交后客户端断开。一次写入是否成功不能仅凭请求超时判断：超时可能发生在提交成功、响应丢失之后；重试前必须查询业务键或使用幂等键。

# 运行与关闭

服务记录结构化日志、请求指标和关联 ID。SIGTERM 到达后先停止接收新流量，再排空请求、停止 worker、关闭依赖，并受总 deadline 控制。数据库超时、缓存不可用、重复请求和下游变慢都应有明确降级或失败行为。

开发中保持一条短反馈链：

```bash
go fmt ./...
go test ./...
go vet ./...
go test -race ./...
go build ./...
```

上面是跨平台编译检查；真正生成可执行文件时再按目标系统确定输出名（Windows 通常为 `.exe`）和目标架构。`go test -race` 只检查执行到的路径，不能证明不存在竞态。提交前确认日志不会输出 token、密码、完整 DSN 或敏感请求体。发布包必须记录 Go 版本、依赖版本、构建命令与迁移版本，避免“本机能跑”无法复现。

# 交付证据

完成的 Go 服务应能在干净环境执行 `go test ./...`、race test、vet、构建、启动、冒烟、压测和关闭验证。README 记录配置、迁移、运行和故障演练；具体学习安排只维护在 Study Cycle。

| 阶段门槛 | 可复查证据 | 不通过时先处理什么 |
| --- | --- | --- |
| 业务规则 | table-driven test 与明确不变量 | 模型和错误语义 |
| 协议边界 | `httptest` 的状态、body、header、大小限制 | 输入校验与响应契约 |
| 数据边界 | 真库集成测试、迁移回滚方案 | 约束、事务和超时歧义 |
| 运行边界 | 日志/指标样本、慢请求关闭实验 | 泄漏、排空和依赖清理 |
| 性能边界 | 固定环境的 benchmark/pprof/压测原始命令 | 先定位瓶颈，再修改实现 |

这篇是所有 Go 服务通用的工作顺序；具体业务拆解、schema 和短链接故障案例放在“首个服务交付”专题。标准库与工具命令以 [Go command documentation](https://pkg.go.dev/cmd/go) 和 [Go testing package](https://pkg.go.dev/testing) 为准。
