---
status: stable
confidence: high
content_verified: 2026-09-17
verified: 2026-10-16
review_stage: learn
review_due: 2026-10-16
previous_review_due: 2026-09-25
---

> [!abstract] 学习定位
> 异常表达失败语义，context manager 保证资源释放，类型标注把接口约束前置到阅读与检查阶段。

> [!summary] 核心摘要
>
> 只捕获能处理的异常并保留上下文；用 `with` 把获取与释放写在同一作用域；用类型标注配合检查器提前发现接口不匹配。

# 异常边界与异常链

异常表示当前操作无法按契约完成。优先捕获最具体的异常，只在以下情况捕获：能够恢复、需要转换为本层语义、或位于进程边界必须生成日志和退出码。

```python
try:
    payload = load_payload(path)
except FileNotFoundError as exc:
    raise ConfigError(f"config does not exist: {path}") from exc
else:
    validate(payload)
finally:
    metrics.flush()
```

`else` 只在 try 未抛异常时执行，避免把后续代码的错误误捕获；`finally` 无论如何执行，适合必须收尾的动作。`raise ... from exc` 保留因果链；`raise` 原样重新抛出当前异常。不要记录一次又重新抛出，让每一层重复打印相同堆栈。

自定义异常按调用者需要处理的语义分组，例如 `ConfigError`、`TransientRemoteError`、`InvalidInputError`。不要为每一句错误信息创建一个类型，也不要用异常承载正常的大量分支。

# Context manager 协议

`with manager as value` 调用 `__enter__`，离开作用域时调用 `__exit__`；即使正文抛异常也会退出。`__exit__` 返回真值会抑制异常，除非确实完成处理，否则应返回假值。

```python
from contextlib import contextmanager
from collections.abc import Iterator

@contextmanager
def transaction(connection) -> Iterator[object]:
    try:
        yield connection
    except Exception:
        connection.rollback()
        raise
    else:
        connection.commit()
```

context manager 把获取、使用和释放放在同一词法作用域。多个动态资源可使用 `ExitStack`；异步资源使用 `async with`。锁、临时目录、数据库事务和 trace span 都适合这种生命周期表达。

# 类型标注的真实边界

标注默认不会在运行时验证。它们用于接口阅读、IDE、静态检查和重构。公共边界优先标注参数、返回值、可空性、容器元素和 callable；局部类型显然时不必过度标注。

```python
from collections.abc import Iterable, Mapping

def error_rate(statuses: Iterable[int]) -> float:
    values = list(statuses)
    if not values:
        raise ValueError("statuses must not be empty")
    failures = sum(code >= 500 for code in values)
    return failures / len(values)
```

`Iterable[int]` 表达函数只需遍历，不要求调用方一定传 list。使用 union `T | None` 后，通过 `is None` 完成类型收窄。结构化 dict 逐渐复杂时，使用 dataclass、TypedDict 或明确领域对象，不要让 `dict[str, object]` 在系统中到处传播。

`Any` 会关闭一部分检查，应限制在未类型化第三方边界并尽快转换。`cast` 只告诉检查器，不做运行时验证；来自 JSON、环境变量和网络的数据仍需显式校验。

# 失败设计练习

为批量 API 工具设计异常层次：配置非法立即退出；单个输入非法记录并继续；远端 timeout 可在幂等前提下重试；认证失败停止整批；最终 CLI 将异常映射为稳定退出码。写测试验证异常链仍包含底层原因，日志不重复也不泄漏 token。
# 三条规则

1. 只捕获你能恢复或能补充上下文的异常；不要裸 `except` 吞掉错误。
2. 文件、连接、锁等资源优先写进 `with`，让退出路径天然释放资源。
3. 对公共函数标注输入、输出与可空性；类型不是运行时验证的替代品。

# 后端辅助中的用法

- 读取配置失败：抛出含文件路径和字段名的异常。
- 批量调用 API：区分网络瞬态失败、业务拒绝与数据格式错误。
- 数据转换函数：用类型标注暴露期望 schema，必要时配合运行时校验。

# 三件工具如何配合

```python
from pathlib import Path

def load_name(path: Path) -> str:
    try:
        with path.open(encoding="utf-8") as f:
            return f.read().strip()
    except OSError as exc:
        raise RuntimeError(f"cannot read {path}") from exc
```

`with` 保证文件关闭；异常链 `from exc` 保留底层原因；`Path -> str` 标注让调用契约更清楚。类型检查能发现接口不匹配，但它不会替你验证 JSON 字段、网络响应或用户输入；这些需要显式的运行时校验。

# 边界

库代码通常应抛出有语义的异常，让应用入口统一记录日志和决定 HTTP/CLI 返回码。若每一层都记录同一异常，会制造重复日志；若捕获后返回 `None`，又会把真正的失败伪装成正常分支。

> [!question]- 自测：先回答再展开
> 1. `finally` 与 context manager 分别解决什么问题？
> 2. 哪种情况应当重新抛出异常，而不是记录日志后继续？

