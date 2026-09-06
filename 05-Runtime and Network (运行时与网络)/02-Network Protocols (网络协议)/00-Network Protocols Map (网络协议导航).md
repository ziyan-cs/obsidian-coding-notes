---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 00-Network Protocols Map (网络协议导航)

> [!abstract] 阅读定位
>
> 网络协议学习不是记名词，而是沿着一次请求的连接、传输、解析、安全和 RPC 边界理解失败会在哪里发生。

## 学习顺序

1. `01–07`：TCP/IP、建连、状态机、TIME WAIT、拥塞、分帧与 UDP。
2. `08–11`：HTTP 报文、语义、连接管理和解析。
3. `12–14`：TLS、HTTP 2、gRPC 与 Protobuf。

## 连接到服务器实践

完成本目录后进入 `03-Server Networking`：将协议中的字节流、连接状态、超时和背压映射为 socket、事件循环、buffer 与业务处理。
