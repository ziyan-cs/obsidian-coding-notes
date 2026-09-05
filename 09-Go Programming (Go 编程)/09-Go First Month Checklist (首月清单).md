---
tags: [language/go, learning/checklist]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# First Month Checklist — Go 第一个月

> **完成标准**：不是“看完 Go 教程”，而是交付一个有测试、能优雅关闭的 todo HTTP API。

## Week 1：命令、类型和 module

- [ ] 完成 [[00-Go Map & Setup (学习地图与环境)]]、[[01-Types, Functions & Interfaces (类型、函数与接口)]]、[[02-Modules, Packages & Tooling (模块、包与工具链)]]。
- [ ] 建立一个独立 Go module，能执行 format/test/vet。
- [ ] 写一个 struct、一个方法、一个小 interface，并解释它们的可见性。

## Week 2：错误、清理和并发

- [ ] 完成 [[03-Errors, defer & context (错误处理、defer 与 context)]]、[[04-Goroutines, Channels & sync (并发原语)]]。
- [ ] 写一个可取消的 worker；故意制造 race，再用 `-race` 观察。
- [ ] 给错误至少包装一次，并用 `errors.Is` 判断。

## Week 3：测试和 HTTP

- [ ] 完成 [[05-Testing & Quality (测试与质量)]]、[[06-HTTP, JSON & Middleware (HTTP 服务基础)]]。
- [ ] 实现 `GET /healthz`、`POST /todos`、`GET /todos`。
- [ ] 用 `httptest` 覆盖正常、错误方法、非法 JSON。

## Week 4：服务收尾

- [ ] 完成 [[07-API Lifecycle & Graceful Shutdown (服务生命周期与优雅关闭)]]、[[08-C++ to Go Decision Notes (C++ 到 Go 迁移决策)]]。
- [ ] 增加 request log 和 request ID。
- [ ] 支持 Ctrl+C 优雅关闭，并写一篇 3 分钟项目讲解。

## 每周复盘

写入 [[00-Start Here (学习入口)/05-Mistake Log|Mistake Log]]：本周最难概念、一次真实错误、下周唯一改进点。
