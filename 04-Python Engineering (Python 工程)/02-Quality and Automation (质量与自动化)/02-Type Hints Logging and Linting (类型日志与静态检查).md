---
study_stage: learn
review_due: 2026-10-11
tags: [language/python, python/quality]
---

> [!note] 方法论坐标
> 日志与信号语义见 [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)，静态检查进入发布门禁的原则见 [Release Verification and Quality Gates (发布验证与质量门禁)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/07-Release%20Verification%20and%20Quality%20Gates%20(发布验证与质量门禁).md)；本篇聚焦 Python typing、logging 与 lint 工具。

> [!abstract] 学习定位
> 类型标注、结构化日志和小函数并不让 Python 失去灵活性，反而让一次性脚本可以安全长大。

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

# 类型边界与收窄

类型标注从系统边界开始：配置解析后尽快转换成 dataclass/TypedDict，网络 JSON 校验后转换成领域对象，避免 `dict[str, Any]` 向内部蔓延。`Any` 会让检查器放弃追踪，只应短暂存在于未类型化边界。

union 通过真实判断收窄：`if value is None`、`isinstance`、枚举分支。`cast()` 只改变检查器认识，不验证运行数据；若外部输入不符合类型，仍会在更远处失败。

对 callable、iterator、mapping 使用 `collections.abc` 中的抽象接口，使函数只要求真正需要的能力。泛型用于保持输入输出类型关系，不为炫技构造复杂 TypeVar 层级。

# 可观测日志

一条日志应支持回答：哪次操作、处理哪个对象、处于什么阶段、结果和耗时是什么。稳定字段可包括 `request_id`、`job_id`、`item_count`、`duration_ms` 和 `error_kind`。

异常通常在能够决定处置的边界记录一次，并使用 `logger.exception` 保留堆栈。底层函数补充上下文后重新抛出，不在每层重复打印。密码、token、Cookie、完整请求体和个人数据不得进入日志；脱敏应在结构化字段进入 logger 前完成。

日志级别表达行动意义：DEBUG 用于临时诊断，INFO 表示正常里程碑，WARNING 表示可恢复异常或退化，ERROR 表示当前操作失败。高频循环逐条 INFO 会产生费用和噪声，应聚合计数或采样。

# 格式化、lint 与静态检查

格式化器解决布局争论；linter 发现未使用代码、危险模式和复杂度；类型检查器验证接口一致性；测试验证运行行为。它们职责不同，不能因为 mypy/pyright 通过就省略数据校验和测试。

本地与 CI 使用同一配置和命令。逐步启用严格规则：先消除真实错误，再提高覆盖范围；大量无理由 `# noqa`、`type: ignore` 会把门禁变成装饰。每个忽略都应尽量指定规则并说明边界。

练习：给 API checker 定义响应 TypedDict 或 dataclass，在解析边界验证 JSON；日志只记录 URL host、status、耗时和 request ID；构造带 token 的错误，验证日志中没有秘密。
# 动手验证

给 API checker 的响应建立 `TypedDict` 或 dataclass：解析网络 JSON 时先检查字段，再进入带类型的业务层。用测试构造含 token 的错误，验证日志只包含 host、status、耗时和 request ID，不包含凭据。最后分别运行格式检查、静态检查和 pytest，说明三者发现的问题为何不同。

官方资料：[typing](https://docs.python.org/3/library/typing.html) · [logging](https://docs.python.org/3/library/logging.html)。
