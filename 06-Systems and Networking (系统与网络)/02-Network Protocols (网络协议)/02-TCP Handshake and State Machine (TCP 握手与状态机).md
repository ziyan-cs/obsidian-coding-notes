---
study_stage: backlog
tags: [network/tcp, systems/state-machine]
---

> [!summary] 核心摘要
>
> TCP 用三次握手交换并确认双方初始序列号，关闭时则分别终止两个发送方向；状态机决定资源何时可释放。TIME_WAIT 通常由主动关闭方承担，用于吸收旧报文并保留响应 FIN 重传的状态。
>

# 三次握手（建立连接）

TCP 连接建立需要双方交换并确认各自的初始序列号（Initial Sequence Number, ISN）。三次报文使客户端与服务端都完成“发送自己的 ISN、收到对方的 ISN、确认对方 ISN”这三个条件；“验证双方收发能力”只能作为现象理解，不能替代序列号同步这一协议原因。

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

若只交换两次，发起方还没有确认接收方的 ISN，接收方也无法确定最初的 SYN 是当前请求还是网络中的旧副本。RFC 9293 所描述的三次握手要求最初 SYN 的发送方再确认一次，使双方对这次连接使用的序列空间达成一致。此时服务端在收到最终 ACK 前处于 `SYN_RCVD`，而不是已经进入 `ESTABLISHED`。

## 为什么不需要四次？

服务端可以把 SYN 和 ACK 合并成一个报文同时发送，没有必要分两次。

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
## 为什么通常看到四个报文？

- 收到 FIN 只表示对方**不再发数据**，但对方仍可接收本地方向的数据
- 本地 TCP 会先确认收到 FIN，而本地应用可能稍后才调用 `close()` 或 `shutdown()` 关闭自己的发送方向
- 因此 ACK 与本地 FIN 通常分开发出；若应用立即关闭且实现允许，它们可以合并，抓包不保证永远恰好四个报文

## 常见异常场景

|场景|发生什么|
|---|---|
|第三次握手丢失|服务端超时重传 SYN+ACK，客户端再次 ACK|
|服务端没有 LISTEN|服务端回 RST，连接被重置|
|同时打开|双方同时发 SYN，各自进入 SYN_SENT → SYN_RCVD，最终 ESTABLISHED|
|同时关闭|双方同时发 FIN，各自进入 CLOSING 状态，最终都进入 TIME_WAIT|

# 状态机与故障定位

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

## 重点状态

### CLOSE_WAIT 大量出现

- 本地 TCP 收到了对端的 FIN，但**本地应用程序没有调用 close()** 关闭连接
- 常见原因：代码 bug（连接泄漏）、线程阻塞、连接池未释放
- 大量 `CLOSE_WAIT` 通常指向本地应用没有及时关闭已被对端半关闭的连接，应结合 fd 数、goroutine/线程栈和连接所有权定位，不能只调内核参数。

### TIME_WAIT 大量出现

- 说明这台机器是**主动关闭方**，且有大量短连接快速建立/断开
- 总量大不等于已经故障；真正风险取决于相同目的端点、临时端口范围、连接建立速率和端口复用条件
- 优先检查是否存在不必要的短连接，再评估长连接、连接池或协议多路复用

### FIN_WAIT_2 长时间存在意味着什么？

- 主动关闭方等待对端发 FIN，但对端迟迟不发
- 对端应用层还在处理，没有调用 close()
- Linux 的 `tcp_fin_timeout` 针对失去应用引用的孤儿 `FIN_WAIT_2` 连接，不是 `TIME_WAIT` 时长开关

# TIME_WAIT 的意义与处理

## 为什么存在 TIME_WAIT？

TIME_WAIT 是主动关闭方在发送最后一个 ACK 后进入的等待状态，持续 **2MSL**。实际时长、端口范围和相关 sysctl 取决于操作系统与内核版本；不要把某个 Linux 环境的数值当作通用常量。

它承担两个相互关联的职责：让主动关闭方仍能响应对端重传的 FIN，以及避免旧连接的延迟报文影响相同四元组的新连接。

## 最后 ACK 丢失

```txt
主动关闭方 ──── ACK ────> 被动关闭方
                          （如果 ACK 丢失）
主动关闭方 <─── FIN ───── 被动关闭方（重传）
主动关闭方 ──── ACK ────> 被动关闭方（再次确认）
```

若没有 TIME_WAIT，主动关闭方收到重传的 FIN 时连接已不存在，会回 RST，导致被动关闭方异常终止。

## 旧报文与四元组复用

若过早复用同一四元组（源/目的 IP 与端口），旧连接的延迟报文可能干扰新连接。TIME_WAIT 是 TCP 对旧报文和最终 ACK 重传建立的安全窗口；RFC 9293 将主动关闭后的等待规定为 2×MSL，但 MSL 取值和实现细节不能从某个 Linux sysctl 机械推算。

## 判断是否真的成为瓶颈

在频繁主动关闭出站连接时，TIME_WAIT 可能大量出现：

- 每个连接由四元组标识；大量指向同一目标的短连接可能使可用本地临时端口成为瓶颈，不能仅按 TIME_WAIT 总数直接判断端口耗尽
- 可用临时端口范围受系统配置限制（Linux 可查看 `/proc/sys/net/ipv4/ip_local_port_range`）
- 端口耗尽 → 新连接无法建立 → 服务不可用

使用 `ss -tan state time-wait` 观察端点分布，再结合临时端口范围、连接建立速率和失败日志判断。不要只看 `TIME_WAIT` 数量：服务端监听端口与客户端临时端口面对的约束并不相同。

处理顺序如下：

1. **确认主动关闭方和受限四元组。** 找到是谁在频繁创建短连接，以及失败是否真由端口或连接复用造成。
2. **先减少连接 churn。** 优先使用 HTTP keep-alive、HTTP/2 多路复用以及数据库或 Redis 连接池，同时修复超时、取消和连接归还路径。
3. **区分 socket 选项与协议状态。** `SO_REUSEADDR` 主要影响本地地址绑定规则，不会清除 `TIME_WAIT`，也不允许任意复用相同四元组。
4. **最后才评估内核参数。** 当前 Linux 文档明确要求不要在缺少专家建议时修改 `tcp_tw_reuse`；其取值和默认行为也可能随内核版本及网络命名空间变化。

### Linux 参数边界

不要把下面的配置当作 `TIME_WAIT` 时长设置：

```bash
net.ipv4.tcp_fin_timeout = 30   # 影响 FIN-WAIT-2 相关超时，不是 TIME_WAIT 时长开关
```

# 验证与自测

```bash
ss -tan
ss -tan state syn-recv
ss -tan state close-wait
ss -tan state time-wait
cat /proc/sys/net/ipv4/ip_local_port_range
```

观察时同时记录本地/远端地址、连接角色、应用日志和采样时间，避免从一张状态计数截图直接推断根因。

1. 三次握手中的第三个报文确认了谁的 ISN？服务端此时从哪个状态进入哪个状态？
2. 收到 FIN 为什么只代表一个发送方向关闭？应用还能做什么？
3. 大量 `CLOSE_WAIT`、`FIN_WAIT_2` 和 `TIME_WAIT` 分别优先排查哪一侧？
4. 为什么 `SO_REUSEADDR`、`tcp_fin_timeout` 和 `tcp_tw_reuse` 不能视为同一类“清理 TIME_WAIT”开关？

# 参考

- [RFC 9293 — Transmission Control Protocol](https://www.rfc-editor.org/rfc/rfc9293.html)：三次握手、状态机、关闭流程与 2MSL 要求。
- [Linux IP sysctl documentation](https://docs.kernel.org/networking/ip-sysctl.html)：`tcp_fin_timeout`、`tcp_tw_reuse` 等参数的当前语义；实践前仍需核对目标内核版本。
