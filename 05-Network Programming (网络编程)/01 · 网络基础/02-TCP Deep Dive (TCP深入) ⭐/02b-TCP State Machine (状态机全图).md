---
tags:
  - network/core
status: 🌱
---

> [!important] **核心考点**：每个状态的含义、转换条件、服务端/客户端各自的状态路径

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

状态转换详解见 → [Three-Way Handshake & Four-Way Teardown (三次握手四次挥手)](</05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02a-Three-Way%20Handshake%20&%20Four-Way%20Teardown%20(三次握手四次挥手).md>) · [TIME_WAIT：Why & How to Handle (TIME_WAIT原因与处理)](</05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02c-TIME_WAIT：Why%20&%20How%20to%20Handle%20(TIME_WAIT原因与处理).md>)
