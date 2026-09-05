---
tags: [project/backend, career/portfolio]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Project Map — 用项目证明后端能力

> **一句话结论**：项目不是功能堆砌；它要证明你能定义边界、实现关键路径、验证正确性、测量性能，并复盘失败。

## 项目梯度

| 项目 | 先解决什么 | 主实现 | Python 配套 | 核心证据 |
| --- | --- | --- | --- | --- |
| URL Shortener | API、存储、缓存、限流 | Go | API 回归/数据生成 | 读写路径与缓存击穿处理 |
| IM / Notification | 长连接、消息顺序、离线投递 | Go；C++ 可做连接实验 | 协议/压测工具 | 连接生命周期与幂等 |
| Flash Sale | 高峰、库存、一致性、削峰 | Go | 压测结果报告 | 超卖防护与故障降级 |
| Config Center | watch、版本、灰度、可靠通知 | Go | 配置校验工具 | 一致性与回滚设计 |

## 项目完成定义（Definition of Done）

- [ ] 有需求、非目标（non-goals）和容量假设。
- [ ] 有可运行服务、README、配置示例和最小测试。
- [ ] 有 API、数据模型、关键路径与错误语义。
- [ ] 有一次压测或故障演练，以及测量数据。
- [ ] 有一份“失败方案与取舍”复盘，不把所有选择写成唯一正确答案。

## 目录规则

项目源代码可放在独立仓库；此目录只保存设计、实验、接口约定、复盘和面试表达，避免把可执行项目与学习笔记混成一团。

## 关联

- [[01-Project Design Template]]
- [[06-Portfolio Evidence Checklist]]
- [[13-Comparisons & Decision Records/00-Decision Map|Decision Map]]
