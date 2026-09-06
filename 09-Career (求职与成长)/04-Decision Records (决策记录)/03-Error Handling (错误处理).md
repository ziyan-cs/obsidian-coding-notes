---
tags: [comparison, errors]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 03-Error Handling (错误处理)

> [!abstract] 阅读定位
>
> 结论：错误处理的目标是保留语义和上下文，并由合适边界映射为用户可见响应；不是把所有失败吞掉或打印。

| 语言 | 常见机制 | 服务层建议 |
| --- | --- | --- |
| C++ | exception、`std::expected` 风格结果 | 明确模块错误边界；避免跨层丢失上下文 |
| Go | 显式 `error`、wrapping、`errors.Is/As` | 立即处理/包装/返回；少用 panic |
| Python | exception | 边界处捕获特定异常、记录上下文、映射退出码/HTTP 错误 |

**反例**：`catch (...)` / `except Exception: pass` / 忽略 `err` 都会把可定位失败变成静默数据错误。

## 服务端错误边界

```text
底层错误（I/O / DB / dependency）
        → 领域错误（not found / conflict / validation）
        → 协议错误（HTTP status + safe response）
```

- 日志记录完整上下文和 cause chain；响应只返回调用方需要的安全信息。
- 重试仅针对可判定为 transient 的失败，且必须设上限、退避和幂等条件。
- `panic` / 未捕获 exception 只适合不可恢复的进程级错误；请求错误应正常返回。

## 30 秒回答与自测

**回答**：好的错误处理保留原因、上下文和边界语义。底层不直接决定 HTTP 文案；在 service/handler 边界把可预期错误映射为稳定响应，并让未知错误可观测。

- 自测：`duplicate key` 应映射为哪类领域错误？是否该让客户端看到原始 SQL？
- 自测：一次超时为什么不能无条件重试？
