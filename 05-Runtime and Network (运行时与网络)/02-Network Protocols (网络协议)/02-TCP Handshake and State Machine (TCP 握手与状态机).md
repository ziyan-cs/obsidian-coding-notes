---
status: stable
confidence: high
verified: 2026-09-17
---

> [!summary]- 复述检查：学完后再展开
>
> **回答**：TCP 用三次握手同步双方初始序列号并确认双向通信能力，用四次挥手独立关闭两个方向；状态机决定资源何时可释放。TIME_WAIT 由主动关闭方承担，用于吸收旧报文并保证最后 ACK 可重传。
>
> > [!note] 本节重点： 为什么是三次/四次、每步状态变化、异常场景
>
> > [!note] 本节重点： 三次握手同步双方初始序列号并处理历史报文；挥手由两条独立发送方向决定，TIME_WAIT 的具体时长依赖协议栈与配置。

# 三次握手（建立连接）

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

# 四次挥手（断开连接）

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

> [!summary]- 复述与自测：学完后再展开
>
> - **常见误区**：以为挥手必然是四次报文（被动方无数据可发时 FIN 紧跟 ACK 可合并；双方同时关闭出现 CLOSING）；忽略 TIME_WAIT 只出现在主动关闭方。
> - **自测**：1) 为什么是三次不是两次？ 2) TIME_WAIT 出现在哪一方，为什么等 2MSL？
>
> ---
>
> 四次挥手后的 TIME_WAIT 细节见 → [TIME_WAIT 深度分析](02c-TIME_WAIT：Why%20&%20How%20to%20Handle%20(TIME_WAIT原因与处理).md)
>
> ---

# TCP State Machine (TCP 状态机)

> [!note] 本节重点：每个状态的含义、转换条件、服务端/客户端各自的状态路径

## 完整状态列表

| 状态          | 含义                       |
| ----------- | ------------------------ |
| CLOSED      | 初始状态，无连接                 |
| LISTEN      | 服务端监听，等待连接               |
| SYN_SENT    | 客户端发 SYN，等 SYN+ACK       |
| SYN_RCVD    | 服务端收 SYN，回 SYN+ACK，等 ACK |
| ESTABLISHED | 连接已建立，可传输数据              |
| FIN_WAIT_1  | 主动关闭方发 FIN，等 ACK         |
| FIN_WAIT_2  | 主动关闭方收 ACK，等对端 FIN       |
| CLOSE_WAIT  | 被动关闭方收 FIN 回 ACK，等应用关闭   |
| LAST_ACK    | 被动关闭方发 FIN，等最后 ACK       |
| TIME_WAIT   | 主动关闭方等 2MSL，防旧报文干扰       |
| CLOSING     | 双方同时关闭，罕见中间状态            |

## 状态转换图

```text
CLOSED ──passive open──────────────→ LISTEN
CLOSED ──active open / connect─────→ SYN_SENT
LISTEN ──receive SYN───────────────→ SYN_RCVD
SYN_SENT ──receive SYN+ACK─────────→ ESTABLISHED
SYN_RCVD ──receive ACK─────────────→ ESTABLISHED
SYN_RCVD ──timeout / RST───────────→ CLOSED
ESTABLISHED ──active close─────────→ FIN_WAIT_1
ESTABLISHED ──passive close────────→ CLOSE_WAIT
FIN_WAIT_1 ──receive ACK───────────→ FIN_WAIT_2
FIN_WAIT_1 ──receive FIN───────────→ CLOSING
FIN_WAIT_1 ──receive FIN+ACK───────→ TIME_WAIT
FIN_WAIT_2 ──receive FIN───────────→ TIME_WAIT
CLOSING ──receive ACK──────────────→ TIME_WAIT
CLOSE_WAIT ──close()───────────────→ LAST_ACK
LAST_ACK ──receive ACK─────────────→ CLOSED
TIME_WAIT ──2MSL timeout───────────→ CLOSED

Notes on TIME_WAIT:
  - Wait 2MSL to ensure the last ACK arrives
  - Prevent old connection packets from interfering with new connections
```

## 重点状态深析

### CLOSE_WAIT 大量出现

- 服务端收到了客户端的 FIN，但**应用程序没有调用 close()** 关闭连接
- 常见原因：代码 bug（连接泄漏）、线程阻塞、连接池未释放
- **大量 CLOSE_WAIT = 服务端代码问题**，需要排查是否有连接未正确关闭

### TIME_WAIT 大量出现

- 说明这台机器是**主动关闭方**，且有大量短连接快速建立/断开
- 短时间内大量 TIME_WAIT 会耗尽端口，新连接无法建立
- 解决方案：开启 `SO_REUSEADDR`、调整 `tcp_tw_reuse`，或改用长连接/连接池

### FIN_WAIT_2 长时间存在意味着什么？

- 主动关闭方等待对端发 FIN，但对端迟迟不发
- 对端应用层还在处理，没有调用 close()
- Linux 可设置 `tcp_fin_timeout` 控制超时时间

---

# TIME WAIT (TIME WAIT 原因与处理)

> [!note] 本节重点：为什么存在、带来什么问题、如何正确处理

## 为什么存在 TIME_WAIT？

TIME_WAIT 是主动关闭方在发送最后一个 ACK 后进入的等待状态，持续 **2MSL**。实际时长、端口范围和相关 sysctl 取决于操作系统与内核版本；不要把某个 Linux 环境的数值当作通用常量。

**两个核心原因：**

### 原因一：保证最后一个 ACK 被对端收到

```txt
主动关闭方 ──── ACK ────> 被动关闭方
                          （如果 ACK 丢失）
主动关闭方 <─── FIN ───── 被动关闭方（重传）
主动关闭方 ──── ACK ────> 被动关闭方（再次确认）
```

若没有 TIME_WAIT，主动关闭方收到重传的 FIN 时连接已不存在，会回 RST，导致被动关闭方异常终止。

### 原因二：让旧连接的所有报文消亡

若立即用相同的四元组（src IP, src port, dst IP, dst port）建立新连接，网络中残留的旧报文可能被新连接误收，造成数据混乱。2MSL 足以让所有旧报文过期消失。

---

## TIME_WAIT 带来的问题

在**高并发短连接**场景下（如 HTTP/1.0、频繁建连的微服务），TIME_WAIT 会大量堆积：

- 每个 TIME_WAIT 状态的连接占用一个本地端口
- 可用临时端口范围受系统配置限制（Linux 可查看 `/proc/sys/net/ipv4/ip_local_port_range`）
- 端口耗尽 → 新连接无法建立 → 服务不可用

---

## 处理方案

### 方案一：评估 `tcp_tw_reuse`（仅在明确的 Linux 出站连接场景）

bash

```bash
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_timestamps = 1
```

- 面向主动发起的出站连接；不应作为服务端监听端口重启的首选手段
- 具体语义存在内核版本差异，标记为 **VERSION_CHECK**

### 不要混淆：`tcp_fin_timeout`

bash

```bash
net.ipv4.tcp_fin_timeout = 30   # 影响 FIN-WAIT-2 相关超时，不是 TIME_WAIT 时长开关
```

### 方案三：使用长连接 / 连接池（根本方案）

- HTTP/1.1 Keep-Alive、HTTP/2 多路复用
- 数据库连接池、Redis 连接池
- 减少连接建立/断开频率，从根本上避免大量 TIME_WAIT

### 方案四：SO_REUSEADDR（服务端重启用）

c

```c
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

- 允许服务端在 TIME_WAIT 期间重新绑定同一端口
- 主要用于服务重启后快速恢复监听，不用等 2MSL

### ❌ 不推荐：tcp_tw_recycle

- 在 NAT 环境下会导致连接被错误丢弃（同一 NAT 后的多个客户端时间戳不一致）
- Linux 4.12 已彻底移除该参数

---

## 小结

|问题|原因|解法|
|---|---|---|
|端口耗尽|大量短连接主动关闭|tcp_tw_reuse + 长连接|
|服务重启失败|端口被 TIME_WAIT 占用|SO_REUSEADDR|
|残留报文干扰|TIME_WAIT 本身的作用|不应跳过，依赖时间戳保护|

> [!summary]- 复述检查：学完后再展开
>
> **TIME_WAIT 为什么存在？** 它由主动关闭方保持，用于重发最后 ACK 的机会，并降低旧连接报文干扰同四元组新连接的风险。大量 TIME_WAIT 首先说明连接创建/关闭过于频繁；优先复用连接、控制请求生命周期，再谨慎评估内核参数。
>
> ---
>
>
> ---
