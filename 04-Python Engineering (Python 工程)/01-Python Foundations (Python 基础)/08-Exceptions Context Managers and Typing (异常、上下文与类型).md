---
study_stage: learn
review_due: 2026-10-16
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
# 组合起来看失败路径

写一个 `load_name(path: Path) -> str`：用 `with` 关闭文件，把底层 `OSError` 转为带路径的业务异常，并让 CLI 在最外层记录一次、返回非零退出码。然后故意制造文件不存在和解码错误，观察异常链与资源关闭。

注意 `finally` 中的操作若再次抛错，可能遮蔽原始异常；事务示例的 `commit()` 也可能失败，是否需要回滚取决于连接与数据库协议，不能把“提交失败”当成一定没有发生副作用。上面的 context manager 管理的是事务状态，并不自动关闭由调用方持有的连接。
