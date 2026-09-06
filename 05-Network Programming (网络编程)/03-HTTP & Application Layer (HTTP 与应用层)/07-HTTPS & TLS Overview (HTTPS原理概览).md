---
tags:
  - network/http
status: 🌱
---

# HTTPS & TLS Overview — HTTPS原理概览

> [!important] **核心考点**：HTTPS 的 TLS/SSL 握手流程、证书验证与数字签名、对称+非对称加密混合方案

## 为什么需要 HTTPS

HTTP 是明文传输，面临三大风险：
- **窃听**：中间人可以读取通信内容
- **篡改**：中间人可以修改通信内容
- **冒充**：中间人可以伪装成服务器

HTTPS = HTTP + TLS（传输层安全性），解决以上所有问题。

## 混合加密方案

TLS 结合了**对称加密**和**非对称加密**：

```
对称加密（如 AES）：
  优点：加密速度快，适合大量数据
  缺点：密钥分发困难

非对称加密（如 RSA）：
  优点：公钥公开，私钥保密
  缺点：加密速度慢，不适合大量数据

TLS 方案：
  1. 用非对称加密安全地协商一个临时对称密钥（会话密钥）
  2. 用对称加密加密后续的通信数据
```

## TLS 1.2 完整握手流程

```
Client                        Server
  │                             │
  │───── ClientHello ──────────→│  客户端发送：支持的 TLS 版本、加密套件列表、
  │                             │             随机数 random_C
  │←─── ServerHello ────────────│  服务端选择：TLS 版本、加密套件、
  │                             │             随机数 random_S
  │←─── Certificate ────────────│  服务端发送数字证书（含公钥）
  │←─── ServerHelloDone ────────│  通知客户端：服务器发送完毕
  │                             │
  │───── ClientKeyExchange ────→│  客户端用服务端公钥加密 premaster_secret
  │                             │  （一个随机数，只有服务端能解密）
  │                             │
  │                             │  双方都计算：master_secret =
  │                             │    PRF(premaster_secret, random_C, random_S)
  │                             │  再衍生出：会话密钥（对称密钥）
  │                             │
  │───── ChangeCipherSpec ─────→│  通知：后续通信将加密
  │───── Finished ─────────────→│  加密的握手消息完整性校验
  │                             │
  │←─── ChangeCipherSpec ───────│  通知：后续通信将加密
  │←─── Finished ───────────────│  加密的握手消息完整性校验
  │                             │
  │══════ 加密通信开始 ═══════════│  使用 AES/GCM 等对称加密
```

## 数字证书与 CA

证书的作用：**证明公钥确实属于声称的服务器**。

```
证书链：
  根 CA（自签名，预置在浏览器/操作系统中）
    └── 中间 CA（由根 CA 签发）
          └── 服务器证书（由中间 CA 签发）

证书内容：
  - 域名（CN / SAN）
  - 公钥
  - 签发者（CA）
  - 有效期
  - 数字签名（由 CA 的私钥生成）
```

**证书验证过程：**
```
1. 浏览器用 CA 的公钥解密证书上的数字签名
2. 得到证书的哈希值（摘要）
3. 浏览器自行计算证书的哈希值
4. 两个哈希值匹配 → 证书未被篡改
5. 检查域名是否匹配
6. 检查有效期
7. 检查是否被吊销（CRL/OCSP）
```

## TLS 1.3 的改进

TLS 1.3 将握手从 2-RTT 减少到 1-RTT（首次）或 0-RTT（恢复）：

```
TLS 1.3 首次握手（1-RTT）：
  客户端 → ClientHello（含支持的加密算法 + 密钥共享 material）
  服务端 ← ServerHello + Certificate + Finished（含密钥共享 material）
  双方立即计算出会话密钥

TLS 1.3 恢复握手（0-RTT）：
  客户端 → 立即发送加密数据（含前次会话的 PSK）
  服务端 ← 响应加密数据
```

**主要变化：**
- 移除不安全的加密算法（RC4、DES、静态 RSA）
- 握手时间缩短 50%
- 0-RTT 模式允许恢复会话时立即发送数据
- 前向安全性（Forward Secrecy）成为标配

## 前向安全性（Forward Secrecy）

```
无前向安全性：
  服务端私钥泄露 → 所有历史通信可解密

有前向安全性（使用 ECDHE）：
  每次会话协商独立密钥 ← 客户端和服务端各自生成临时密钥对
  服务端私钥泄露 → 只能解密未来的连接，无法解密历史记录
```

## 配置建议

```nginx
server {
    listen 443 ssl;
    ssl_protocols TLSv1.2 TLSv1.3;           # 仅允许安全版本
    ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:...;
    ssl_prefer_server_ciphers on;
    ssl_session_cache shared:SSL:10m;         # 会话缓存加速
    ssl_session_timeout 10m;                  # 会话超时
}
```

> [!tip]- **工程要点**：TLS 握手的主要成本是首次的 2-RTT（TLS 1.2）。生产环境通过会话复用（Session ID/Session Ticket）减少握手次数。对于移动端和 API 服务，TLS 1.3 的 0-RTT 能显著降低首字节延迟。注意 0-RTT 有重放攻击风险，需要应用层幂等性保护。

---

应用层协议进阶见 → [HTTP/2 Key Features (HTTP2核心特性了解)](</05-Network%20Programming%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/08-HTTP⧸2%20Key%20Features%20(HTTP2核心特性了解).md>) · [gRPC & Protobuf (RPC框架与序列化)](</05-Network%20Programming%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/09-gRPC%20&%20Protobuf%20(RPC框架与序列化)%20⭐.md>)
