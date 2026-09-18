---
status: learning
confidence: high
content_verified: 2026-09-18
verified: 2026-10-07
review_stage: learn
review_due: 2026-10-07
tags: [language/python, python/iteration]
---

> [!abstract] 学习定位
> Python 的 `for`、推导式和许多标准库函数都建立在 iterable/iterator 协议上；生成器把“怎样逐步产生数据”封装成可暂停状态机。

# Iterable 与 Iterator

iterable 能通过 `iter(obj)` 取得 iterator；iterator 通过 `next(it)` 逐个返回值，结束时抛出 `StopIteration`。`for` 循环负责处理这套协议。

~~~python
values = [10, 20, 30]
iterator = iter(values)
assert next(iterator) == 10
assert next(iterator) == 20
~~~

list 是可重复迭代的 iterable，每次 `iter(list)` 可得到新 iterator；文件对象和许多生成器通常是一次性 iterator，消费后不能自动回到开头。API 接收 `Iterable[T]` 时，不应默认它有长度、可索引或可重复读取。

# 生成器的暂停语义

含 `yield` 的函数调用后返回 generator，不立即执行函数体。每次 `next` 或循环推进时，从上次暂停位置恢复，局部变量仍保留。

~~~python
from collections.abc import Iterator
from pathlib import Path

def non_empty_lines(path: Path) -> Iterator[str]:
    with path.open(encoding="utf-8") as stream:
        for raw in stream:
            line = raw.strip()
            if line:
                yield line
~~~

文件在生成器真正开始迭代后才打开，并在生成器正常结束、异常退出或被关闭时离开 `with`。调用方若提前停止且长期保留生成器，资源释放时机可能延后；资源生命周期要求严格时，应让调用方显式持有 context manager。

`yield from child` 把子 iterable 的值向外转发，也会转发异常和生成器协议细节。普通数据流水线中优先把它理解为“委托迭代”，不要一开始依赖复杂的 `send/throw` 协程用法。

# 惰性带来的收益与代价

惰性处理避免把全部输入放进内存，并允许流水线组合：

~~~python
numbers = (int(line) for line in lines)
positive = (value for value in numbers if value > 0)
result = sum(positive)
~~~

代价是错误可能推迟到消费阶段发生，数据只能经过一次，调试时也不能直接看到全部结果。若数据很小、需要多次遍历或随机访问，list 往往更清晰。

`itertools` 提供组合工具，但它们继承一次性消费和无限序列风险。对 `count()` 等无限 iterator 必须用 `islice`、条件或外部取消限制。

# 自定义协议与鸭子类型

Python 倾向依赖行为而非具体继承树：实现 `__iter__` 就可参与迭代，实现 `__enter__/__exit__` 就可用于 `with`。类型标注中的 `Protocol` 能描述这种结构化接口。

~~~python
from typing import Protocol

class Writer(Protocol):
    def write(self, data: str) -> int: ...

def emit(writer: Writer, message: str) -> None:
    writer.write(message + "\n")
~~~

真实文件、内存 buffer 和测试 fake 都能满足接口。协议应保持小而稳定；把几十个方法塞进一个 Protocol 会重新制造紧耦合。

# 失败路径与实践

- 不要用 `list(generator)` 调试超大或无限输入。
- generator 中的异常在消费时出现，调用栈和日志要保留数据位置。
- 对外部流进行重试时，明确已消费位置和重复处理语义。
- `tee` 让 iterator 看似可复制，但可能在内部缓存大量未同步消费的数据。
- 异步 iterator 使用 `async for`，解决的是等待型数据源，不让 CPU 工作自动并行。

练习：实现逐行读取访问日志的生成器，返回解析成功记录；非法行写入单独错误报告。用十行样例和一个大文件验证内存不会随输入线性增长。

> [!question]- 理解检查
> 1. iterable 与 iterator 有什么区别？
> 2. 为什么 generator 的异常可能不在调用函数时发生？
> 3. 流式处理降低了哪部分内存，哪些聚合状态仍可能无限增长？

> [!info]- 官方参考
> - [Python Tutorial: Iterators and Generators](https://docs.python.org/3/tutorial/classes.html#iterators)
> - [itertools](https://docs.python.org/3/library/itertools.html)
