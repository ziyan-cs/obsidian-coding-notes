# Obsidian Coding Notes — Vault Conventions

> **定位**：以 C++ 后端为主线，Go 用于现代服务端工程，Python 用于自动化、测试与数据处理。这个库服务于系统学习、可执行复习、项目实践和两年后的校招，而不是收藏百科。

## Start Here

先从 [[00-Start Here/00-Vault Map|知识地图]] 进入；学习节奏见 [[00-Start Here/01-Backend Learning Roadmap|两年学习路线]]；每次学习后按 [[00-Start Here/02-Spaced Review System|间隔复习系统]] 回顾；新笔记使用 [[00-Start Here/03-Note Standard|笔记规范]]；全库的渐进深改顺序见 [[00-Start Here/07-Vault Audit & Migration Queue|迁移队列]]。

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
| `00-Start Here`                   | Navigation, review system, templates, learning roadmap        |
| `01-CS Core`                      | Fundamentals, OS, computer organization                       |
| `02-Algorithms & Data Structures` | Trees, graphs, DP, strings, bit ops                           |
| `03-C++ Programming`              | Syntax, core mechanisms, modern C++, STL, concurrency, design |
| `04-Linux & System`               | Basics, processes/threads, I/O models, syscalls               |
| `05-Network Programming`          | Foundations, socket, HTTP, server design patterns             |
| `06-Database`                     | MySQL: SQL, InnoDB, query optimization, high-availability     |
| `07-Development Tools`            | Git, CMake, debug/profiling, misc                             |
| `08-Distributed & Middleware`     | Redis, Nginx, MQ, distributed protocols                       |
| `09-Career Prep`                  | Interview knowledge, project walkthroughs, resume             |
| `10-Go Programming`               | Go language and backend engineering                            |
| `11-Python Toolkit`               | Python for automation, testing, and data work                 |
| `12-Backend Projects`             | Project evidence, designs, experiments                        |
| `13-Comparisons & Decision Records` | Cross-language comparisons and technical decisions           |

> `assets/` (Obsidian attachments) and dot-dirs (`.obsidian/`, `.git/`, `.claude/`) are tool-managed, not content folders.
