---
tags:
  - network/core
status: seed
review_due: 2026-09-26
confidence: 1
verified: stable
---

> [!abstract] 核心考点：> 为什么是三次/四次、每步状态变化、异常场景

# 02a-Three-Way Handshake & Four-Way Teardown (三次握手四次挥手)

> [!abstract] 核心考点：> 三次握手同步双方初始序列号并处理历史报文；挥手由两条独立发送方向决定，TIME_WAIT 的具体时长依赖协议栈与配置。

## 三次握手（建立连接）

TCP 连接建立需要三次报文交换，目的是**双方互相确认对方的发送和接收能力都正常**。

```text
  Client                      Server
    │                           │
    │  (CLOSED→SYN_SENT)        │
    ├── SYN(SEQ=x) ────────────→│
    │                           │  (LISTEN→SYN_RCVD)
    │←─ SYN+ACK(SEQ=y,ACK=x+1) ─┤
    |                           |
    │  (SYN_SENT→ESTABLISHED)   │
    ├── ACK(SEQ=x+1,ACK=y+1) ──→│
    │                           │  (SYN_RCVD→ESTABLISHED)
```

## 为什么必须三次，不能两次？

两次握手无法解决**历史连接的干扰**问题：

- 若网络中有一个旧的 SYN 延迟到达服务端，服务端回 SYN+ACK 就认为连接建立成功
- 但客户端知道这不是自己发的，会无视，服务端却一直等待，浪费资源
- 三次握手让客户端有机会拒绝历史连接（不发第三次 ACK）

> **本质：用三次握手同步双方的初始序列号（ISN），并排除旧连接干扰**

## 为什么不需要四次？

服务端可以把 SYN 和 ACK 合并成一个报文同时发送，没有必要分两次。

---

## 四次挥手（断开连接）

TCP 是**全双工**的，双方各自独立关闭自己的发送方向，所以需要四次。

```text
Active Closer             Passive Closer
    │                          │
    │  (ESTABLISHED→FIN_WAIT_1)│
    ├── FIN(SEQ=u) ───────────→│
    │                          │  (ESTABLISHED→CLOSE_WAIT)
    │←──── ACK(ACK=u+1) ───────┤
    │  (FIN_WAIT_1→FIN_WAIT_2) │
    │                          │  (flush buffered data)
    │                          │  (CLOSE_WAIT→LAST_ACK)
    │←──── FIN(SEQ=v) ─────────┤
    │  (FIN_WAIT_2→TIME_WAIT)  │
    ├── ACK(ACK=v+1) ─────────→│
    │                          │  (LAST_ACK→CLOSED)
    │  (TIME_WAIT→CLOSED)      │
    │  (wait 2MSL)             │
```
## 为什么需要四次，不能三次？

- 收到 FIN 只表示对方**不再发数据**，但对方还可以继续接收
- 被动关闭方可能还有数据没发完，ACK 和 FIN 不能合并
- 必须等被动方数据发完，才能发 FIN

## TIME_WAIT 为什么等 2MSL？

- **MSL**：报文在网络中的最大存活时间；具体定义与实际等待时长由协议栈和配置决定
- 等待 2MSL 的两个原因：
    1. **确保最后一个 ACK 能到达对端**：若对端没收到最后的 ACK，会重发 FIN，2MSL 足够接收重传并再次 ACK
    2. **让旧连接的所有报文在网络中消失**：防止新连接收到旧连接的延迟报文

---

## 常见异常场景

|场景|发生什么|
|---|---|
|第三次握手丢失|服务端超时重传 SYN+ACK，客户端再次 ACK|
|服务端没有 LISTEN|服务端回 RST，连接被重置|
|同时打开|双方同时发 SYN，各自进入 SYN_SENT → SYN_RCVD，最终 ESTABLISHED|
|同时关闭|双方同时发 FIN，各自进入 CLOSING 状态，最终都进入 TIME_WAIT|

## 30 秒回答 / 自测

- **30 秒回答**：三次握手为了双方同步 ISN 并排除历史旧连接干扰（两次无法拒绝旧 SYN，四次没必要——SYN+ACK 可合并）；挥手常见为四个报文，因为 TCP 全双工、双方独立关闭发送方向，但 ACK 与 FIN 在合适时机可以合并；主动关闭方进入 TIME_WAIT，以便重发最终 ACK 并隔离旧报文，实际时长取决于协议栈。
- **常见误区**：以为挥手必然是四次报文（被动方无数据可发时 FIN 紧跟 ACK 可合并；双方同时关闭出现 CLOSING）；忽略 TIME_WAIT 只出现在主动关闭方。
- **自测**：1) 为什么是三次不是两次？ 2) TIME_WAIT 出现在哪一方，为什么等 2MSL？

---

四次挥手后的 TIME_WAIT 细节见 → [TIME_WAIT 深度分析](02c-TIME_WAIT：Why%20&%20How%20to%20Handle%20(TIME_WAIT原因与处理).md)
