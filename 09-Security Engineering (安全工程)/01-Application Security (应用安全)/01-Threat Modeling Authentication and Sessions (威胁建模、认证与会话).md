---
study_stage: backlog
tags: [security/threat-model, security/authentication]
---

> [!abstract] 学习目标
> 先识别资产、攻击者、入口和信任边界，再设计身份验证与会话。安全不是给正常流程补一个登录接口，而是约束所有“不可信输入如何获得权限”。

# 威胁建模与信任边界

威胁建模先回答四件事：保护什么、谁可能攻击、攻击从哪里进入、失败后损失是什么。资产不仅是密码和数据库，还包括用户身份、业务额度、配置、日志、算力和系统可用性。

```text
Browser / App
      |
      | untrusted network
      v
[Edge / Gateway] ---- [Identity Provider]
      |
      | authenticated identity + request context
      v
[Backend Service] ---- [Database / Cache / Queue]
      |
      +---- [Third-party API]
```

每条跨边界的数据都要回答：身份从哪里来、是否被篡改、是否允许访问该对象、输入大小是否受限、失败是否会泄漏信息。

常用分析步骤：

1. 画数据流与信任边界，标出公网入口、管理入口和第三方依赖。
2. 枚举资产与高价值操作，例如改密码、付款、导出数据和发布配置。
3. 对每条数据流检查 spoofing、tampering、repudiation、information disclosure、denial of service、elevation of privilege。
4. 为风险指定预防、检测、响应措施，并用测试或演练验证。

# 身份验证

Authentication（AuthN）确认“你是谁”，Authorization（AuthZ）决定“你能做什么”。二者必须分开：登录成功不代表可以访问任意资源。

密码处理原则：

- 只保存专用 password hashing 算法的结果，使用成熟库提供的 Argon2id、scrypt、bcrypt 或 PBKDF2；不要保存明文，也不要直接使用快速通用哈希。
- 每个密码使用独立 salt；算法参数需要随硬件能力升级，并为旧记录设计登录后渐进迁移。
- 登录错误不要暴露“用户存在但密码错误”等可枚举差异；对失败进行限速、告警和异常检测。
- 修改密码、邮箱、支付信息等敏感操作应重新验证身份；高风险场景使用 MFA。

认证结果应形成稳定的 subject identity，例如内部用户 ID，而不是直接相信客户端提交的角色、租户或用户字段。

# Session 与 Token

以一个校内资料平台为例：访客登录后，服务端把登录凭据验证为内部 `user_id`，创建会话，再在每次请求中从会话恢复主体。浏览器提交的 `user_id`、`role` 和 `tenant_id` 只能当输入，不能反过来决定身份。若用户改邮箱，先检查当前会话，再要求近期重新认证；若账号被封禁，还应使现有会话失效。这个流程把“登录成功”和“持续拥有访问权”分开。

服务端 session 通常让客户端只持有随机、不可预测的 session ID，权限与状态保存在服务端；JWT 把声明放进签名 token，便于跨服务验证，但撤销、密钥轮换和权限即时变更更复杂。

选择依据：

| 需求 | 服务端 Session | 短期 Access Token + Refresh Token |
| --- | --- | --- |
| 立即注销和封禁 | 服务端删除状态即可 | 需要短有效期、撤销表或版本检查 |
| 横向扩展 | 共享 session store 或粘性会话 | 验签可去中心化 |
| 权限频繁变化 | 每次读取服务端状态较直接 | token 内声明可能过期 |
| 泄漏影响 | session ID 等同临时凭据 | access/refresh token 同样属于敏感凭据 |

浏览器 cookie 至少考虑 `Secure`、`HttpOnly`、合适的 `SameSite`、窄化的 `Path/Domain` 与过期时间。登录、提权和重新认证后轮换 session ID，退出时让服务端状态失效。`SameSite` 是纵深防御，不能无条件替代 CSRF 防护。

JWT 使用时必须固定允许的算法，验证签名、issuer、audience、有效期和必要声明；不要把敏感数据当作“被加密”写进普通签名 JWT。

对浏览器应用，还要先决定凭据的存放和传输方式：Cookie 自动随请求发送，因此必须考虑 CSRF；脚本可读取的存储在 XSS 后可能泄漏 token。不存在对所有应用通用的“JWT 比 Session 更安全”结论。画出登录、续期、登出、封禁、密钥轮换五条状态路径，再选择机制。

# 失败路径与验证

至少验证以下攻击路径：

- [ ] 用户名枚举、暴力破解、凭据填充和验证码绕过。
- [ ] 登录前后 session fixation、token 重放、退出后继续使用。
- [ ] access token 过期、refresh token 重用与密钥轮换。
- [ ] 管理账户是否能从普通公网登录入口访问。
- [ ] 身份服务超时或不可用时，系统是 fail closed 还是允许有限降级。

> [!summary] 核心摘要
> 从一次登录开始，说明凭据如何验证、会话如何建立、每个请求如何恢复身份、权限在哪里检查，以及注销、泄漏和密钥轮换时怎样收回访问能力。

官方参考：[OWASP Authentication Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Authentication_Cheat_Sheet.html) · [OWASP Session Management Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Session_Management_Cheat_Sheet.html)
