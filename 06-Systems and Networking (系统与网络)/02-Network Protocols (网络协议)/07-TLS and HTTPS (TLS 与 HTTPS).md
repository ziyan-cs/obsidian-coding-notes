---
study_stage: backlog
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

TLS 1.3 完整握手通常可在 1 RTT 建立应用数据密钥；0-RTT 指符合条件的**恢复连接可提前发送应用数据**，不是整个握手在零往返内完成，也不保证服务端一定接受早期数据：

```
TLS 1.3 首次握手（1-RTT）：
  客户端 → ClientHello（含支持的加密算法 + 密钥共享 material）
  服务端 ← ServerHello + Certificate + Finished（含密钥共享 material）
  双方立即计算出会话密钥

TLS 1.3 允许 0-RTT 早期数据的恢复：
  客户端 → ClientHello + 受前次会话 PSK 保护的 early data
  服务端 ← 接受或拒绝 early data，继续完成握手
```

**主要变化：**
- 移除不安全的加密算法（RC4、DES、静态 RSA）
- 首次握手通常减少一个往返；实际收益取决于网络 RTT、恢复与部署配置
- 0-RTT 早期数据可能被重放，只在应用协议明确允许且具备抗重放设计时使用
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

有前向安全性（使用 (EC)DHE，且临时密钥及时销毁）：
  每次握手协商独立会话密钥；证书私钥用于认证，不直接加密会话密钥
  事后仅泄露证书私钥 → 通常不能解密已记录的过去会话；但攻击者可能冒充服务器，直到密钥被撤销/轮换
```

> 前向安全性保护的是已完成会话在长期认证密钥日后泄露时的历史机密性；它不保护终端已泄露的会话密钥，也不自动为 TLS 1.3 0-RTT 提供前向安全性。

## 配置与排障

```nginx
# 教学示意，不是可直接上线的完整安全配置；套件、证书和 TLS 1.3 配置需按 Nginx/OpenSSL 版本核对。
server {
    listen 443 ssl;
    ssl_protocols TLSv1.2 TLSv1.3;           # 仅允许安全版本
    ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:...;
    ssl_prefer_server_ciphers on;
    ssl_session_cache shared:SSL:10m;         # 会话缓存加速
    ssl_session_timeout 10m;                  # 会话超时
}
```

> [!tip]- **工程要点**：TLS 1.2/1.3 的实际握手往返与恢复效果取决于版本和部署；会话恢复可减少建连成本。TLS 1.3 0-RTT 能降低恢复请求的等待，但可能被重放；只有应用协议明确允许重放、请求副作用可控且部署具备相应防护时才考虑启用，单有“幂等”标签未必足够。

