---
status: learning
confidence: low
verified: 2026-09-17
tags: [language/go, learning/checklist]
---

> [!abstract] 学习定位：完成标准：不是“看完 Go 教程”，而是交付一个有测试、能优雅关闭的 todo HTTP API。

> [!summary]- 复述检查：学完后再展开
>
> **核心结论**：完成标准：不是“看完 Go 教程”，而是交付一个有测试、能优雅关闭的 todo HTTP API。

# Week 1：命令、类型和 module

- [ ] 完成类型函数与接口、模块包与工具链两篇基础笔记，并建立可测试的 Go module。
- [ ] 建立一个独立 Go module，能执行 format/test/vet。
- [ ] 写一个 struct、一个方法、一个小 interface，并解释它们的可见性。

# Week 2：错误、清理和并发

- [ ] 完成 03-Errors, defer & context (错误处理、defer 与 context)、04-Goroutines, Channels & sync (并发原语)。
- [ ] 写一个可取消的 worker；故意制造 race，再用 `-race` 观察。
- [ ] 给错误至少包装一次，并用 `errors.Is` 判断。

# Week 3：测试和 HTTP

- [ ] 完成 05-Testing & Quality (测试与质量)、06-HTTP, JSON & Middleware (HTTP 服务基础)。
- [ ] 实现 `GET /healthz`、`POST /todos`、`GET /todos`。
- [ ] 用 `httptest` 覆盖正常、错误方法、非法 JSON。

# Week 4：服务收尾

- [ ] 完成 07-API Lifecycle & Graceful Shutdown (服务生命周期与优雅关闭)、08-C++ to Go Decision Notes (C++ 到 Go 迁移决策)。
- [ ] 增加 request log 和 request ID。
- [ ] 支持 Ctrl+C 优雅关闭，并写一篇 3 分钟项目讲解。

# 每周复盘

写入 [05-Mistake Log (错误记录)](/00-Hub%20(学习入口)/05-Mistake%20Log%20(错误记录).md)：本周最难概念、一次真实错误、下周唯一改进点。
