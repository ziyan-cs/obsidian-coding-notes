---
status: stable
confidence: high
content_verified: 2026-09-17
---

> [!abstract] 学习目标：理解 HTTPS 的安全目标、TLS 密钥建立、证书信任链及部署边界。

# 安全目标与加密模型

## HTTP 为什么需要 TLS

HTTP 是明文传输，面临三大风险：
- **窃听**：中间人可以读取通信内容
- **篡改**：中间人可以修改通信内容
- **冒充**：中间人可以伪装成服务器

HTTPS = HTTP over TLS（Transport Layer Security），在正确验证证书、域名与信任链的前提下，为通信提供机密性、完整性和服务器身份认证；它不自动消除应用层授权、XSS 或业务逻辑漏洞。

## 混合加密

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

# 握手与会话密钥

## TLS 1.2

```
TLS 1.2 handshake, simplified
  1. client -> server: ClientHello (versions, cipher suites, random)
  2. server -> client: ServerHello and Certificate
  3. client -> server: key exchange material
  4. both sides derive symmetric traffic keys
  5. both sides send Finished; protected application data begins

TLS 1.3 has a different, shorter handshake; do not use this diagram as its wire format.
```

## TLS 1.3 与 0-RTT

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
- 首次握手通常减少一个往返；实际收益取决于网络 RTT、恢复与部署配置
- 0-RTT 模式允许恢复会话时立即发送数据
- 前向安全性（Forward Secrecy）成为标配

# 证书与身份认证

证书的作用：**证明公钥确实属于声称的服务器**。

```
certificate chain
  root CA -> intermediate CA -> server certificate

certificate fields
  - subject names (CN / SAN)
  - public key, issuer, validity period, CA signature
```

**证书验证过程：**
```
1. 浏览器按签名算法用 CA 公钥**验证**证书签名
2. 浏览器自行计算待签名证书内容的摘要并核验签名
3. 签名验证通过 → 证书内容未被篡改，且由对应 CA 私钥签发
5. 检查域名是否匹配
6. 检查有效期
7. 检查是否被吊销（CRL/OCSP）
```

# 安全边界与部署

## 前向安全性

```
无前向安全性：
  服务端私钥泄露 → 所有历史通信可解密

有前向安全性（使用 ECDHE）：
  每次会话协商独立密钥 ← 客户端和服务端各自生成临时密钥对
  服务端私钥泄露 → 只能解密未来的连接，无法解密历史记录
```

## 配置与排障

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

> [!tip]- **工程要点**：TLS 1.2/1.3 的实际握手往返与恢复效果取决于版本和部署；会话恢复可减少建连成本。TLS 1.3 0-RTT 能降低恢复请求的等待，但可能被重放，只应用于可安全重试的幂等请求，并按当前 TLS/服务器文档配置。

> [!summary] 核心摘要
>
> TLS 用证书链验证服务器身份，再协商对称会话密钥保护后续 HTTP 数据。TLS 1.2 与 1.3 的握手消息不同，现代部署优先使用具前向保密的密钥交换。关键不是背报文，而是能说明：证书防冒充、AEAD 保证机密性/完整性、0-RTT 有重放边界、HTTPS 不替代应用鉴权。
>
> ---
>
> ---
