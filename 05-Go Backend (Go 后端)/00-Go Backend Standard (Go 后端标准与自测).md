---
study_stage: backlog
tags: [learning/standard, language/go, backend]
---

> [!abstract] 完成标准
> Go 主线以可维护的并发服务为目标。学完后应能写出边界清楚、可取消、可测试、可观测并能优雅退出的 HTTP/RPC 服务。

版本基线：以 Go 1.27 官方文档核对语言与标准库；动手前用 `go version` 确认本机工具链，并在 `go.mod` 的 `go` 行声明项目最低版本。`ServeMux` 方法路由要求 Go 1.22+；Go 1.27 才支持具体类型的泛型方法。

# 语言与工程基础

- [ ] 能解释值语义、slice、map、interface、method set、组合与泛型的核心规则。
- [ ] 能用 module 管理依赖，按 package 职责组织代码，并避免循环依赖和全局可变状态。
- [ ] 能以 `error` 传递失败原因，使用 wrapping、`errors.Is/As`，区分业务错误与系统错误。
- [ ] 能写 table-driven tests、benchmark，并使用 race detector 验证并发代码。

# 并发与运行时

- [ ] 能说明 goroutine、channel、mutex、context 各自解决的问题，不用 channel 替代所有同步。
- [ ] 能设计取消、超时、背压和有界 worker pool，避免 goroutine 泄漏。
- [ ] 能解释 scheduler、stack growth、GC 的基本行为，以及它们对延迟和内存的影响。
- [ ] 能用 tracing、pprof 和指标定位阻塞、分配、CPU 与 goroutine 问题。

# Web 服务与数据访问

- [ ] 能用 `net/http` 建立路由、中间件、输入校验、错误映射和优雅关闭。
- [ ] 能设计 context 传播、请求 ID、结构化日志、超时与认证边界。
- [ ] 能正确使用连接池、事务、预编译/参数化查询，并处理重试和幂等。
- [ ] 能实现并解释 JSON、Protobuf/gRPC 的接口兼容与演进策略。

# 交付验收

- [ ] 完成一个含数据库的 HTTP 服务，具备测试、迁移、配置、日志和健康检查。
- [ ] 为关键路径增加超时、限流或队列上限，并验证过载时的行为。
- [ ] 使用 race detector、benchmark 与 pprof 留下可复查证据。
- [ ] 能容器化运行服务，并说明部署、回滚和配置注入方式。

# 综合自测

1. 一个 channel 由谁关闭，为什么接收方通常不应关闭它？
2. `context.Context` 为什么应沿调用链传递，而不应存进长期存活的结构体？
3. HTTP handler 已返回，但它启动的 goroutine 仍在运行，会产生什么风险？
4. 数据库操作超时后，如何判断是否可以安全重试？
