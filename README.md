# Obsidian Coding Notes — Vault Conventions

> **定位**：以 C++ 后端为主线，Go 用于现代服务端工程，Python 用于自动化、测试与数据处理。这个库服务于系统学习、可执行复习、项目实践和两年后的校招，而不是收藏百科。

## Start Here

先从 [[00-Start Here (学习入口)/00-Vault Map (知识库地图)|知识地图]] 进入；学习节奏见 [[00-Start Here (学习入口)/01-Backend Learning Roadmap (后端学习路线)|两年学习路线]]；每次学习后按 [[00-Start Here (学习入口)/02-Spaced Review System (间隔复习系统)|间隔复习系统]] 回顾；新笔记使用 [[00-Start Here (学习入口)/03-Note Standard (笔记规范)|笔记规范]]；全库的渐进深改顺序见 [[00-Start Here (学习入口)/07-Vault Audit & Migration Queue (审计与迁移队列)|迁移队列]]。

## Note Frontmatter

```yaml
---
tags: [language/cpp, topic/ownership]
status: seed # seed | learning | review | solid
review_due: 2026-09-12 # optional; YYYY-MM-DD
confidence: 1 # 1-5; optional
verified: 2026-09-05 # optional; required for volatile tooling/API facts
---
```

- `seed`：刚创建或只读过；`learning`：正在理解和练习；`review`：需要按计划复习；`solid`：可独立解释并写出最小实现。
- 不要求把旧笔记一次性迁移。编辑一篇旧笔记时，再按新字段渐进补齐。

## Vault Layout

> **Auto-maintain:** When adding/renaming/removing top-level dirs below, update this section.

| Directory                         | Content                                                       |
| --------------------------------- | ------------------------------------------------------------- |
| `00-Start Here (学习入口)`         | Navigation, review system, templates, learning roadmap        |
| `01-CS Core (计算机核心基础)`      | Fundamentals, OS, computer organization                       |
| `02-Algorithms & Data Structures (算法与数据结构)` | Trees, graphs, DP, strings, bit ops          |
| `03-C++ Programming (编程语言)`    | Syntax, core mechanisms, modern C++, STL, concurrency, design |
| `04-Linux & System (Linux 系统)`   | Basics, processes/threads, I/O models, syscalls               |
| `05-Network Programming (网络编程)` | Foundations, socket, HTTP, server design patterns            |
| `06-Database (数据库)`             | MySQL: SQL, InnoDB, query optimization, high-availability     |
| `07-Distributed & Middleware (分布式与中间件)` | Redis, Nginx, MQ, distributed protocols      |
| `08-Development Tools (工程开发工具)` | Git, CMake, debug/profiling, misc                           |
| `09-Go Programming (Go 编程)`      | Go language and backend engineering                           |
| `10-Python Toolkit (Python 工具链)` | Python for automation, testing, and data work                |
| `11-Comparisons & Decision Records (技术选型与决策记录)` | Cross-language comparisons and technical decisions |
| `12-Backend Projects (后端项目)`   | Project evidence, designs, experiments                        |
| `13-Career Prep (面试与成长)`       | Interview knowledge, project walkthroughs, resume             |

> `assets/` (Obsidian attachments) and dot-dirs (`.obsidian/`, `.git/`, `.claude/`) are tool-managed, not content folders.
