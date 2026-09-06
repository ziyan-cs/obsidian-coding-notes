---
tags:
  - network/http
status: 🌱
---

# HTTP⧸2 Key Features — HTTP2核心特性了解

> [!abstract] 核心考点：HTTP/2 多路复用、头部压缩 HPACK、二进制分帧、服务器推送等核心特性

## HTTP/2 要解决的问题

HTTP/1.1 的结构性问题：
1. **队头阻塞**：同一连接串行处理请求
2. **头部冗余**：每次请求重复发送相同头部（Cookie、User-Agent 等）
3. **文本协议低效**：头部文本传输浪费带宽
4. **连接过多**：为并行请求打开多个 TCP 连接

## 二进制分帧（Binary Framing）

HTTP/2 将文本协议改为二进制协议，所有通信在**一个 TCP 连接**上的**多个流**中进行：

```
HTTP/2 帧结构 (9 字节头部 + 载荷)：
  +-----------------------------------------------+
  | Length (24 bits)     | Type (8) | Flags (8)   |
  +-----------------------------------------------+
  | Stream Identifier (31 bits)                   |
  +-----------------------------------------------+
  | Frame Payload (0+)                            |
  +-----------------------------------------------+

帧类型：DATA, HEADERS, PRIORITY, RST_STREAM, SETTINGS, PUSH_PROMISE, PING, GOAWAY, WINDOW_UPDATE
```

**关键概念：**
- **流（Stream）**：一个完整的请求-响应交互，每个流有唯一 ID
- **消息（Message）**：映射到 HTTP 请求或响应的完整帧序列
- **帧（Frame）**：通信的最小单位，包含所属流 ID

## 多路复用（Multiplexing）

HTTP/2 在**一个 TCP 连接**上交错发送多个流的帧：

```
HTTP/1.1 串行（队头阻塞）：
  | 请求1 → → | 响应1 ← ← | 请求2 → → | 响应2 ← ← |

HTTP/2 多路复用（交错）：
  发送：| 流1头部   | 流2头部   | 流1数据   | 流3头部   | 流2数据 |
  接收：| ← 流1头部 | ← 流2头部 | ← 流1数据 | ← 流3头部 | ← 流2数据 |

  流1: GET /index.html
  流2: GET /style.css
  流3: GET /app.js
  全部并发在一个 TCP 连接上传输！
```

**好处：** 
- 消除 HTTP/1.1 的队头阻塞
- 只需要一个 TCP 连接（减少连接数）
- 更高效的网络利用率（少竞争）

## 头部压缩 HPACK

HTTP/2 使用 HPACK 算法压缩头部；收益取决于头部重复度、动态表状态和内容，不能预设固定压缩比例：

```
HPACK 核心机制：
  1. 静态表：预定义的常用头部（如 :method: GET、:status: 200）
  2. 动态表：连接建立过程中出现的头部缓存
  3. Huffman 编码：对文本值压缩

实际大小随站点与请求变化，应以抓包或压测观察
```

**静态表示例：**

| Index | Header Name | Header Value |
|-------|------------|-------------|
| 2 | :method | GET |
| 3 | :method | POST |
| 5 | :path | / |
| 7 | :scheme | http |
| 8 | :scheme | https |
| 24 | cache-control | |

编码时只需传输索引号而非完整字符串。

## 服务器推送（Server Push）

服务器可以主动向客户端推送资源，无需客户端显式请求：

```
客户端请求 index.html：
  C: 我需要 index.html

服务器推送（无需客户端请求）：
  S: 我猜你还需要 style.css（推送）
  S: 我猜你还需要 app.js（推送）
  S: 这是你要的 index.html（正常响应）

客户端收到 PUSH_PROMISE 帧后可选择拒绝
```

**适用场景：** HTML 页面依赖的 CSS、JS、字体文件等可预测资源。

**注意：** Server Push 已被 Chrome 团队在 HTTP/2 中弃用，推荐使用 103 Early Hints 替代。

## 优先级和流量控制

**流优先级：** 客户端可以告诉服务器哪些资源更重要，服务器优先发送高优先级流。

```
依赖树示例：
  :root
   ├── index.html（默认权重 16）
   ├── style.css（依赖于 index.html，权重 128）
   └── app.js（依赖于 index.html，权重 32）
```

**流量控制：** 流级别的流量控制（基于 WINDOW_UPDATE 帧），防止一个流占满整个连接带宽。

## HTTP/2 的队头阻塞

HTTP/2 解决了**请求级别的队头阻塞**，但仍有**TCP 级别的队头阻塞**：

```
TCP 丢包时：
  流1: [数据 1] [数据 2] [数据 3]
  流2: [数据 4] [数据 5] [数据 6]
  如果数据 3 丢失，TCP 会阻塞直到 3 被重传
  流4/流5 后续数据即使到了也无法交付 → TCP HOL blocking
```

这正是 HTTP/3 使用 QUIC（基于 UDP）的原因——在流级别处理丢包，不影响其他流。

> [!tip]- **工程要点**：HTTP/2 不在协议层强制 TLS，但浏览器部署通常通过 HTTPS 使用它。多路复用、HPACK 的收益取决于资源结构与网络条件，不能承诺固定首屏提升。HTTP/3 以 QUIC 减少 TCP 级 HOL；是否双栈部署取决于客户端、CDN 与运维条件。

## 30 秒回答

**HTTP/2 解决了什么、没解决什么？** 它把 HTTP 消息拆成可交错的二进制帧，消除 HTTP/1.1 管道化的请求级 HOL，并压缩头部；但同一 TCP 连接丢包仍会阻塞后续字节交付。HTTP/3 通过 QUIC 将不同流的丢包恢复隔离。

---

HTTP/2 相关协议见 → [HTTPS & TLS Overview (HTTPS原理概览)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/07-HTTPS%20&%20TLS%20Overview%20(HTTPS原理概览).md>) · [gRPC & Protobuf (RPC框架与序列化)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/09-gRPC%20&%20Protobuf%20(RPC框架与序列化)%20⭐.md>)
