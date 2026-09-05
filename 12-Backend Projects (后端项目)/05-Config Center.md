---
tags: [project/config-center, distributed/configuration]
status: seed
review_due: 2026-10-05
confidence: 1
verified: 2026-09-05
---

# Config Center — 配置不是一张表

> **目标**：让服务以可追溯、可验证、可回滚的方式获取配置，而不是“修改一个值后祈祷所有实例一致”。

> 面试/系统设计参考：[[13-Career Prep (面试与成长)/02 · 项目讲解/99-Config-Center (配置中心设计)|99 配置中心设计]]。

## 最小范围

- namespace / key / version / value / author / timestamp。
- client 拉取、版本比较、watch 通知、校验与回滚。
- 不做跨地域强一致和复杂权限系统。

## 必答问题

- 配置格式错误谁拦截？发布前还是客户端加载时？
- 通知丢失后如何补偿？客户端如何重新同步？
- 灰度发布如何定义目标集合与回滚点？
- 配置生效是否需要原子切换？

## 验收

- [ ] 配置版本不可变，回滚创建新版本而非改历史。
- [ ] Python 校验器验证 schema 和危险配置。
- [ ] 模拟 watch 断连、漏通知和服务重启。
