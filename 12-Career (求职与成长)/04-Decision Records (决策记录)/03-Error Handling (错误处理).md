---
study_stage: backlog
tags: [comparison, errors]
---

> [!abstract] 学习定位：结论：错误处理的目标是保留语义和上下文，并由合适边界映射为用户可见响应；不是把所有失败吞掉或打印。

> [!summary] 核心摘要
> 错误要保留原因、上下文和可处理语义，并在协议边界映射成稳定响应。异常、返回值和 error 类型只是载体；吞错、重复记录和无界重试都会破坏诊断。

# 服务端错误边界

```text
底层错误（I/O / DB / dependency）
        → 领域错误（not found / conflict / validation）
        → 协议错误（HTTP status + safe response）
```

- 日志记录完整上下文和 cause chain；响应只返回调用方需要的安全信息。
- 重试仅针对可判定为 transient 的失败，且必须设上限、退避和幂等条件。
- `panic` / 未捕获 exception 只适合不可恢复的进程级错误；请求错误应正常返回。

# 一条错误应完成的旅程

```text
cause -> classify -> add context -> log once -> safe response
```

例如数据库唯一键冲突可被分类为 `conflict`，由 handler 映射为 HTTP 409；日志保留 operation、business key、request ID 和原始 cause，响应只给稳定错误码与可行动信息。超时只有在操作幂等、调用方仍有时间预算且重试次数受限时才适合重试。

# 面试表达模板

“我把底层错误与对外协议分开：底层保留 cause，领域层分类，入口层映射稳定响应。日志只在能补充上下文的边界记录一次，避免泄漏内部细节和重复日志。”

> [!summary] 核心摘要
> 好的错误处理保留原因、上下文和边界语义。底层不直接决定 HTTP 文案；在 service/handler 边界把可预期错误映射为稳定响应，并让未知错误可观测。
>
> - 自测：`duplicate key` 应映射为哪类领域错误？是否该让客户端看到原始 SQL？
> - 自测：一次超时为什么不能无条件重试？
