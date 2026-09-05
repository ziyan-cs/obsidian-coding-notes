---
tags: [language/python, python/quality]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Type Hints, Logging & Quality — 让脚本能被未来的你读懂

> **一句话结论**：类型标注、结构化日志和小函数并不让 Python 失去灵活性，反而让一次性脚本可以安全长大。

```python
import logging
from collections.abc import Iterable

log = logging.getLogger(__name__)

def mean(values: Iterable[float]) -> float:
    items = list(values)
    if not items:
        raise ValueError("values must not be empty")
    result = sum(items) / len(items)
    log.info("computed mean for %d values", len(items))
    return result
```

## 规则

- 为 public function、复杂容器和边界输入写 annotation。
- log 使用参数化：`log.info("id=%s", user_id)`，不靠字符串拼接。
- 不记录密码、token、完整个人数据；错误日志保留上下文但要脱敏。
- `print` 适合 CLI 最终输出；诊断信息进入 logging。

## 常见坑

- 标注 `list` 而不说明元素类型，失去大部分价值。
- 捕获裸 `Exception` 后吞掉错误，导致工具“成功”但结果错误。
- 一开始引入太多 lint/type 工具；先让 pytest 和清晰函数跑通。
