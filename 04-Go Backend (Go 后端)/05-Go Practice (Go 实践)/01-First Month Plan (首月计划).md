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

**01-First Month Plan (首月计划)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


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

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 01-First Month Plan (首月计划) 的问题、核心机制与边界。

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
