---
status: learning
confidence: low
verified: 2026-09-17
tags: [project/im, backend/concurrency]
---

> [!abstract] 一句话结论：即时通信要分开处理连接生命周期、消息投递与用户已读；每层都有独立的失败与重试语义。

> [!summary]- 复述检查：学完后再展开
>
> **核心结论**：心跳只能推断连接可能存活，broker ACK 只能说明投递或消费阶段，已读回执才是用户看到的业务证据；三者不能互相替代。

# 最小范围

- 单机 WebSocket / TCP 连接管理。
- 心跳、登录/登出、单聊消息、离线消息占位。
- 不做端到端加密、群聊大规模 fan-out、多机全局顺序。

# 必答问题

- 连接断开怎么检测？心跳的误判成本是什么？
- producer 重试后如何避免重复消息？
- 消息 ID、会话 ID、顺序范围分别是什么？
- 服务关闭时怎样停止接入并处理在途消息？

# 单聊的最小数据契约

```json
{"message_id":"uuid","conversation_id":"c42","sender_id":"u1","sequence":18,"body":"hello"}
```

`message_id` 用于去重；`conversation_id + sequence` 用于定义一个会话内的顺序范围；`sender_id` 用于权限和审计。服务端写入消息后再通知接收方，客户端重连时按最后确认的 sequence 拉取缺口，不能依赖一次 WebSocket 推送必达。

# 验收

- [ ] 连接状态机图：connecting / active / closing / closed。
- [ ] 模拟慢客户端和断线重连。
- [ ] 写出幂等消费/去重方案与局限。
- [ ] Python 脚本模拟多客户端，不将测试当作真实性能数据。

> [!summary]- 项目表达检查：学完后再展开
>
> 这个项目把“TCP/WebSocket 连接管理”和“消息可靠投递”拆开建模：连接状态机处理生命周期，消息 ID 与幂等键处理重复，离线与重试必须说明一致性边界。重点不是承诺全局顺序，而是明确顺序保证在哪个会话范围内成立。
>
> > [!warning] 常见误区
> > “客户端收到 ACK”不必然代表用户已看到消息；重连也不天然保证不会重复投递。
