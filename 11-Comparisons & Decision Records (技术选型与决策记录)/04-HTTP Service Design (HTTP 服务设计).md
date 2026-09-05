---
tags: [comparison, backend/http]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# HTTP Service Design — 语言无关的服务边界

> **结论**：无论实现语言，handler 处理协议，service 处理业务，repository 隔离外部系统；请求必须有 timeout、错误语义和可观测性。

```text
client → routing/middleware → handler → service → repository → DB/cache
```

## 必须统一的约定

- request ID、结构化日志、timeout / cancellation。
- 输入大小和 schema 校验；认证/授权边界。
- 成功与失败 JSON 结构、HTTP status、幂等语义。
- health/readiness、优雅关闭和依赖超时。

语言的差异（C++ event loop、Go `net/http`、Python client/tool）应服务于该边界，不改变这些基本责任。
