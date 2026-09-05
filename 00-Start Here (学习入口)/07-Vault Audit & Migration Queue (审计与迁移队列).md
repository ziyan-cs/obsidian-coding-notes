---
tags: [vault/audit, vault/migration]
status: learning
review_due: 2026-10-05
confidence: 1
verified: 2026-09-05
---

# Vault Audit & Migration Queue — 全库改造队列

> **结论**：先让所有知识有入口、优先级和复习机制；再按项目需要深改高价值正文，不用一次机械重写所有旧内容。

## 已完成的架构改造

- [x] `00-Start Here (学习入口)`：路线、复习、笔记标准、术语、错误和来源规则。
- [x] `09-Go Programming (Go 编程)`：从零到 HTTP API 的首月路线。
- [x] `10-Python Toolkit (Python 工具链)`：自动化、测试、数据工具的首月路线。
- [x] `12-Backend Projects (后端项目)`：四个项目的设计与证据模板。
- [x] `11-Comparisons & Decision Records (技术选型与决策记录)`：跨语言与技术取舍。
- [x] C++ 后端主线地图。

## 本轮实际进度（2026-09-05）

- **入口导航**：新增 4 个章节 MOC——`00-CS Core MOC (计算机核心导航)`、`00-Algorithms MOC (算法导航)`、`00-Tools MOC (工程工具导航)`、`00-Career Prep MOC (面试导航)`；新增 wikilink 全部校验通过（0 dangling）。C++ 已有 `00-C++ Backend Core Map (后端能力地图)`，无需新增。
- **frontmatter 迁移**：60 篇主线笔记由 emoji 状态改为文本 `status` + `review_due` + `confidence` + `verified`；稳定基础标 `stable`，易变内容（编译链接、CMake、gRPC、hiredis、EXPLAIN）标 `NEEDS_VERIFY`。
- **内容闭环**：7 篇高频笔记补「30 秒回答 / 自测」——B+ 树、MVCC、Next-Key Lock、RDB、epoll API、TCP 握手、虚表。
- **未做（待后续）**：算法模板"错误模式 / 表达卡"改造；低频理论来源与待验证项补齐。

## 旧内容迁移优先级

| 优先级 | 范围 | 改造方式 | 触发条件 |
| --- | --- | --- | --- |
| P0 | C++ 所有权/并发；Linux I/O；网络服务器；MySQL/Redis | 补一句话结论、反例、自测、关联 | 当前阶段学习或项目遇到 |
| P1 | CMake、Git、调试、性能；HTTP/gRPC/MQ | 核对版本/命令，补最小实验 | 开始 Go 项目时 |
| P2 | 算法模板与面试材料 | 从题解转为错误模式和表达卡 | 每周算法复盘时 |
| P3 | 低频理论/未知项目设计 | 保留，记录来源和待验证项 | 有真实需求时 |

## 全库质量检查清单

- [ ] 每个重点笔记有“它解决什么问题”。
- [ ] 每个并发、资源、性能结论有边界或反例。
- [ ] 每个工具/API/版本主题有来源和验证日期。
- [ ] 每个项目有测试、测量、失败方案和限制。
- [ ] 每周到期复习不超过可执行范围；积压先减少新主题。

## 后续维护节奏

1. 每周：完成学习计划、清理到期复习、记录错误。
2. 每月：从 P0/P1 选择 3–5 篇旧笔记深改，而不是增加大量新标题。
3. 每个项目节点：把真实问题反哺到语言、系统和决策笔记。
