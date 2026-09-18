---
status: learning
confidence: high
content_verified: 2026-09-17
tags: [security/authorization, security/api]
---

> [!abstract] 学习目标
> 授权必须在服务端针对“主体、动作、资源、上下文”逐次判断。API 安全的核心不是隐藏接口，而是让每个对象、字段和业务动作都有明确权限与资源上限。

# 授权模型

一次授权决策可以写成：`allow(subject, action, resource, context)`。只检查角色而不检查目标资源，会产生最常见的越权问题。

- RBAC：角色映射权限，适合职责稳定的后台系统。
- ABAC：结合主体、资源和环境属性，适合租户、组织、地域或风险条件复杂的系统。
- ACL：权限直接附着在资源上，表达精细，但大规模维护成本高。
- Relationship-based：根据资源关系图判断，例如群成员、文档所有者和组织上下级。

默认拒绝（default deny），权限检查靠近业务资源边界，并由可信服务端数据确定 tenant ID、owner ID 和 role。网关可以做粗粒度入口控制，但不能替代业务服务的对象级授权。

# 对象、字段与功能权限

典型错误请求：

```http
GET /api/orders/10002
Authorization: Bearer <user-A-token>
```

即使用户 A 已登录，服务端仍要确认订单 `10002` 是否属于 A 或 A 被授权的租户。不要先按 ID 查询完整对象后再“希望调用方不会看到”。

需要分别检查：

- Object-level：能否访问这条订单、消息、文件或配置。
- Property-level：能否读取或修改 `role`、`price`、`owner_id` 等敏感字段。
- Function-level：普通用户能否调用管理、批量导出、退款或发布接口。
- Business flow：抢购、注册、验证码、评论等流程是否能被自动化滥用。

更新接口使用 allowlist 映射允许字段，不把请求 JSON 无条件绑定到数据库模型。响应 DTO 也应显式选择字段，避免过度暴露。

# 输入与资源边界

输入校验用于确认数据满足业务与结构约束，不是靠黑名单寻找“危险字符串”。数据库使用参数化查询；输出到 HTML、Shell、日志或 URL 时按目标上下文编码或限制。

每个接口至少定义：

- body、header、URL 和上传文件的最大尺寸。
- 数组长度、分页上限、字符串长度、数值范围和嵌套深度。
- 超时、并发上限、速率限制与下游调用预算。
- 允许的 content type、HTTP method 和 schema 版本。

SSRF 防护不能只校验字符串前缀：应限制允许的 scheme/host/port，解析并检查最终地址，控制重定向，阻断环回、链路本地和内部网段，并在网络层限制服务可访问范围。

# Web 与 API 常见边界

- CORS 是浏览器跨源读取策略，不是认证或服务端访问控制。
- CSRF 利用浏览器自动携带凭据；使用 cookie 会话时需要 SameSite、CSRF token 或同源校验等组合措施。
- XSS 的主要防线是按输出上下文编码与严格 CSP；输入清洗不能替代正确输出处理。
- SQL injection 通过参数化查询和安全 ORM 用法防御，不依赖手写转义。
- 错误响应对客户端稳定，但内部日志应保留 request ID、根因和安全事件上下文；不要把堆栈、SQL 或密钥返回给客户端。
- 第三方 API 的响应同样不可信，需要 schema、大小、超时和内容校验。

# 安全测试矩阵

| 维度 | 至少覆盖的反例 |
| --- | --- |
| 身份 | 无 token、过期 token、错误 issuer/audience、被撤销会话 |
| 对象 | 访问别人的 ID、跨租户 ID、已删除或归档资源 |
| 字段 | 提交只允许管理员修改的字段、响应泄漏内部字段 |
| 功能 | 普通用户调用管理接口、绕过前端直接请求隐藏 API |
| 资源 | 超大 body、深层 JSON、慢上传、高并发和昂贵查询 |
| 外联 | SSRF 到 localhost、metadata endpoint、重定向后的内网地址 |

> [!summary] 核心摘要
> 选择一个“修改订单”接口，说明身份从哪里来、对象归属如何检查、允许修改哪些字段、怎样限制资源消耗，以及需要哪些负向测试。

官方参考：[OWASP API Security Top 10](https://api-security.owasp.org/editions/2023/en/0x11-t10/) · [OWASP REST Security Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/REST_Security_Cheat_Sheet.html)
