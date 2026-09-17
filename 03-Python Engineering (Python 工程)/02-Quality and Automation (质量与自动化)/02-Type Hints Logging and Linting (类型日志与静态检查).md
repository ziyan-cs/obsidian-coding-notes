---
status: learning
confidence: low
verified: 2026-09-17
review_due: 2026-09-29
tags: [language/python, python/quality]
---

> [!abstract] 一句话结论：类型标注、结构化日志和小函数并不让 Python 失去灵活性，反而让一次性脚本可以安全长大。

> [!tip] 把“边界”写清楚
> 优先给命令行参数、文件输入、HTTP 响应和公开函数写类型；内部很短、类型显然的局部变量不必为了标注而制造噪声。

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

# 规则

- 为 public function、复杂容器和边界输入写 annotation。
- log 使用参数化：`log.info("id=%s", user_id)`，不靠字符串拼接。
- 不记录密码、token、完整个人数据；错误日志保留上下文但要脱敏。
- `print` 适合 CLI 最终输出；诊断信息进入 logging。

> [!warning]- 易错点
> - 标注 `list` 而不说明元素类型，失去大部分价值。
> - 捕获裸 `Exception` 后吞掉错误，导致工具“成功”但结果错误。
> - 一开始引入太多 lint/type 工具；先让 pytest 和清晰函数跑通。
>
> > [!summary]- 复述检查：学完后再展开
> >
> > 类型标注是给人和静态检查工具的契约，不是运行时自动校验。日志记录诊断上下文，`print` 留给 CLI 的正常输出；两者都要避免敏感数据。小脚本一旦被重复使用，就应把输入、输出、错误与可观察性写成稳定边界。

> [!question]- 自测：先回答再展开
> 1. `Iterable[float]` 为什么比裸 `list` 更贴近 `mean` 的真实输入要求？
> 2. 为什么 `log.info("id=%s", user_id)` 优于 f-string 日志？
> 3. 遇到无法恢复的配置错误时，怎样既保留异常上下文又让 CLI 明确失败？

> [!info]- 延伸阅读
> - 下一步：[03-Project Quality Baseline (项目质量基线)](/03-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/03-Project%20Quality%20Baseline%20(项目质量基线).md)
