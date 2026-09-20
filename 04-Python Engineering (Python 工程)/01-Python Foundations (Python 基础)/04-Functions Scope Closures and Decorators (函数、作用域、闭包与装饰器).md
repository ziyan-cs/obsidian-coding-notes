---
study_stage: learn
review_due: 2026-10-02
tags: [language/python, python/functions]
---

> [!abstract] 学习定位
> 函数是带局部作用域的对象；参数绑定、闭包捕获和装饰器包装都会影响接口语义，不能只把它们当语法技巧。

# 参数绑定与接口设计

调用函数时，实参对象绑定到形参名称；Python 不复制对象，也没有“统一按值/按引用传递”这一简单说法。函数能修改传入的可变对象，但给形参重新赋值不会改变调用者名称的绑定。

~~~python
def update(items: list[int]) -> None:
    items.append(1)  # 修改共享对象
    items = []       # 只重绑局部名称
~~~

参数从左到右可分为 positional-only、普通参数、`*args`、keyword-only 和 `**kwargs`。公共接口不要无约束接收 `**kwargs`，否则拼写错误和接口变化难以被发现。

~~~python
def request(
    method: str,
    url: str,
    /,
    *,
    timeout: float,
    retries: int = 0,
) -> bytes:
    ...
~~~

`/` 左侧只能按位置传递，`*` 右侧必须写参数名。timeout、重试、危险开关适合 keyword-only，因为调用点能表达含义。

# 默认参数与返回契约

默认表达式在 `def` 执行时求值一次，而不是每次调用求值。可变默认对象会跨调用共享；时间戳、随机数和环境变量也可能因此被意外固定。

~~~python
def collect(value: int, output: list[int] | None = None) -> list[int]:
    result = [] if output is None else output
    result.append(value)
    return result
~~~

返回值应有稳定语义。不要让相同函数在成功时返回对象、失败时偶尔返回 `None`、另一些失败又抛异常。若“没有结果”是正常状态，可返回 `T | None`；若输入或外部依赖失败，应使用有语义的异常或显式结果类型。

# LEGB 作用域与闭包

名称查找遵循 Local、Enclosing、Global、Builtins。赋值默认创建当前局部名称；`global` 和 `nonlocal` 会改变绑定位置，但大量使用通常说明状态边界不清晰。

闭包让内部函数保留外围作用域中的绑定，适合创建带配置的小函数：

~~~python
from collections.abc import Callable

def make_validator(limit: int) -> Callable[[str], bool]:
    def valid(text: str) -> bool:
        return 0 < len(text) <= limit
    return valid
~~~

循环中创建闭包容易遇到 late binding：闭包在调用时查找变量，而不是在创建时复制当前值。

~~~python
bad = [lambda: i for i in range(3)]
good = [lambda i=i: i for i in range(3)]
~~~

默认参数技巧在此处把当前对象绑定到每个函数自己的参数，但实际工程中也可以用辅助函数或 `functools.partial` 表达得更清楚。

# 装饰器不是魔法

装饰器接收 callable 并返回 callable，`@decorator` 等价于定义后执行 `function = decorator(function)`。包装函数必须保留元数据，并正确转发参数和返回值。

~~~python
from collections.abc import Callable
from functools import wraps
from time import perf_counter
from typing import ParamSpec, TypeVar

P = ParamSpec("P")
R = TypeVar("R")

def timed(func: Callable[P, R]) -> Callable[P, R]:
    @wraps(func)
    def wrapper(*args: P.args, **kwargs: P.kwargs) -> R:
        started = perf_counter()
        try:
            return func(*args, **kwargs)
        finally:
            elapsed = perf_counter() - started
            print(f"{func.__name__} took {elapsed:.3f}s")
    return wrapper
~~~

认证、事务、重试等装饰器会改变控制流和失败语义，必须说明顺序、异常、同步/异步兼容性与是否可重复执行。把所有逻辑藏进装饰器会让调用链难以调试。

# 函数设计与测试

- 优先让函数只做一件可描述的事，并把文件、网络、时钟等依赖从参数传入。
- 避免依赖可变全局状态；测试之间会互相污染，并发时更危险。
- 纯函数相同输入得到相同输出，适合承载业务规则；副作用集中在边界层。
- 文档字符串解释契约、单位、异常与副作用，不重复函数名。

练习：编写一个重试高阶函数，参数包含最大尝试次数、退避函数和“哪些异常可重试”的判断。用 fake 函数验证成功、永久失败和达到上限三条路径。

> [!question]- 理解检查
> 1. 为什么给形参重新赋值不会改变调用者变量，但 `append` 可能改变调用者看到的 list？
> 2. late binding 为什么会让多个 lambda 得到相同结果？
> 3. 一个重试装饰器为什么必须考虑幂等性？

> [!info]- 官方参考
> - [Python Tutorial: Defining Functions](https://docs.python.org/3/tutorial/controlflow.html#defining-functions)
> - [Python FAQ: Why do lambdas defined in a loop return the same result?](https://docs.python.org/3/faq/programming.html#why-do-lambdas-defined-in-a-loop-with-different-values-all-return-the-same-result)
