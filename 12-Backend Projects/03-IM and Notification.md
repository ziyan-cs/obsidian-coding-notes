---
tags: [project/im, backend/concurrency]
status: seed
review_due: 2026-10-05
confidence: 1
verified: 2026-09-05
---

# IM / Notification — 连接与消息可靠性

> **目标**：理解“连接活着、消息送达、用户看到”是三个不同问题。

## 最小范围

- 单机 WebSocket / TCP 连接管理。
- 心跳、登录/登出、单聊消息、离线消息占位。
- 不做端到端加密、群聊大规模 fan-out、多机全局顺序。

## 必答问题

- 连接断开怎么检测？心跳的误判成本是什么？
- producer 重试后如何避免重复消息？
- 消息 ID、会话 ID、顺序范围分别是什么？
- 服务关闭时怎样停止接入并处理在途消息？

## 验收

- [ ] 连接状态机图：connecting / active / closing / closed。
- [ ] 模拟慢客户端和断线重连。
- [ ] 写出幂等消费/去重方案与局限。
- [ ] Python 脚本模拟多客户端，不将测试当作真实性能数据。
