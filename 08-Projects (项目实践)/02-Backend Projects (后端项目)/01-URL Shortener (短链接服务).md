---
tags: [project/url-shortener, backend/api]
status: seed
review_due: 2026-10-05
confidence: 1
verified: 2026-09-05
---

# 01-URL Shortener (短链接服务)

> [!abstract] 阅读定位
>
> 目标：用户创建短链接，访问短码后重定向；重点是读写路径、冲突、缓存与统计边界，不是生成一个随机字符串。

> 面试/系统设计参考：[[13-Career Prep (面试与成长)/02-System Design Cases (系统设计案例)/96-Short-URL (短URL系统设计)|96 短 URL 系统设计]]。

## 最小范围

- `POST /v1/links`：校验 URL，创建短码。
- `GET /{code}`：查询并 302 redirect。
- `GET /v1/links/{code}`：读取元数据。
- 不做登录、多地域、复杂分析；这些作为后续扩展。

## 关键问题

| 问题 | 第一版选择 | 需要验证 |
| --- | --- | --- |
| 短码冲突 | 数据库唯一约束 + 重试 | 冲突率与最大重试次数 |
| 热点读 | Redis cache-aside | 缓存未命中与失效行为 |
| 无效链接 | 显式 404 / 410 语义 | 不泄漏内部信息 |
| 统计 | 异步事件（后续） | 至少一次投递的去重 |

## 验收

- [ ] Go 标准库 HTTP API + `httptest`。
- [ ] MySQL 唯一约束、Redis 缓存、timeout 和结构化日志。
- [ ] Python checker 生成链接、验证 redirect、汇总延迟。
- [ ] 一张读路径图、一份压测报告、一条缓存失效复盘。

## 30 秒项目表达

我用短链接服务练习“写入正确性和读路径性能”的分离：短码由唯一约束兜底，读请求走 cache-aside，缓存失效时仍以数据库为 source of truth。第一版只证明重定向、冲突处理、超时和观测，不把多地域或分析系统伪装成已实现能力。

> [!warning] 常见误区
> hash 截断不等于永不冲突；302/301 的选择、负缓存和热点保护都需要以真实需求与测试结果决定。
