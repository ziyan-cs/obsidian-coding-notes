---
tags: [language/go, learning/checklist]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-First Month Plan (首月计划)

> [!abstract] 阅读定位
>
> 完成标准：不是“看完 Go 教程”，而是交付一个有测试、能优雅关闭的 todo HTTP API。

## 30 秒回答

**核心结论**：阅读定位  完成标准：不是“看完 Go 教程”，而是交付一个有测试、能优雅关闭的 todo HTTP API。


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

写入 [[00-Start Here (学习入口)/05-Mistake Log (错误记录)|Mistake Log]]：本周最难概念、一次真实错误、下周唯一改进点。



## 从零建立模型

本页主题是 **01-First Month Plan (首月计划)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

## 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

## 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。



## 渐进练习

1. **第一步 · 理解**：读：标出本页代码中错误向上返回、资源释放和 goroutine 退出的位置。
2. **第二步 · 实现**：写：为一个纯业务函数补 table-driven test；若有并发，写一个取消或关闭案例。
3. **第三步 · 验证**：测：运行 `go test`，并在适用时运行 `go test -race` 或 benchmark，记录结论与环境。

## 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-First Month Plan (首月计划)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
