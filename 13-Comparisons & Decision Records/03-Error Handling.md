---
tags: [comparison, errors]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Error Handling — 异常、error、exception

> **结论**：错误处理的目标是保留语义和上下文，并由合适边界映射为用户可见响应；不是把所有失败吞掉或打印。

| 语言 | 常见机制 | 服务层建议 |
| --- | --- | --- |
| C++ | exception、`std::expected` 风格结果 | 明确模块错误边界；避免跨层丢失上下文 |
| Go | 显式 `error`、wrapping、`errors.Is/As` | 立即处理/包装/返回；少用 panic |
| Python | exception | 边界处捕获特定异常、记录上下文、映射退出码/HTTP 错误 |

**反例**：`catch (...)` / `except Exception: pass` / 忽略 `err` 都会把可定位失败变成静默数据错误。
