---
status: learning
confidence: high
content_verified: 2026-09-18
tags: [language/python, python/object-model]
---

> [!abstract] 学习定位
> Python 类把状态与行为组织为运行时对象；先用组合和小接口表达业务概念，再在确有替换关系时使用继承。

# 类定义与属性查找

执行 `class` 语句会创建类对象；调用类通常通过 `__new__` 创建实例，再由 `__init__` 初始化。实例属性通常保存在实例命名空间，类属性由所有实例通过类共享。

~~~python
class RetryPolicy:
    default_attempts = 3  # 类属性

    def __init__(self, attempts: int | None = None) -> None:
        self.attempts = attempts or self.default_attempts
~~~

读取 `obj.name` 时先考虑描述符，再查实例和类的 MRO；对初学阶段，至少要记住：在类上定义的可变 list/dict 会被所有未覆盖该属性的实例共享。

实例方法本质是函数通过 descriptor 绑定实例后得到的 bound method，`self` 只是惯例名称，不是关键字。`@classmethod` 接收类，适合替代构造器；`@staticmethod` 不接收实例或类，若与类型状态无关，也可能更适合放在模块函数中。

# 不变量与封装

Python 的单下划线表示“内部使用”的约定，不是访问控制。封装的价值是让对象始终满足不变量，而不是隐藏所有字段。

~~~python
class Account:
    def __init__(self, balance: int = 0) -> None:
        if balance < 0:
            raise ValueError("negative initial balance")
        self._balance = balance

    @property
    def balance(self) -> int:
        return self._balance

    def withdraw(self, amount: int) -> None:
        if amount <= 0 or amount > self._balance:
            raise ValueError("invalid withdrawal")
        self._balance -= amount
~~~

若一个字段无需计算、校验或兼容层，直接公开属性通常比机械 getter/setter 更符合 Python 风格。`property` 用于确有行为的属性访问，但昂贵 I/O 不应伪装成普通字段读取。

# Dataclass 与值对象

`@dataclass` 可生成初始化、比较和表示方法，适合主要承载数据的值对象。

~~~python
from dataclasses import dataclass, field

@dataclass(frozen=True, slots=True)
class Endpoint:
    url: str
    timeout: float = 3.0
    tags: tuple[str, ...] = field(default_factory=tuple)

    def __post_init__(self) -> None:
        if self.timeout <= 0:
            raise ValueError("timeout must be positive")
~~~

可变默认字段使用 `default_factory`。`frozen=True` 阻止普通属性赋值，但不是深度不可变；字段若指向可变对象，内部仍可能变化。`slots=True` 可减少实例字典等开销并限制随意新增属性，但是否值得使用应以对象数量、继承需求和 profile 为依据。

# 组合、继承与多态

继承适合真正的 is-a 替换关系。多继承遵循 MRO，协作式初始化需要一致使用 `super()`；没有充分理由时不要构造复杂菱形层次。

组合让对象显式持有依赖，更容易替换和测试：

~~~python
class ReportService:
    def __init__(self, repository, writer) -> None:
        self._repository = repository
        self._writer = writer

    def run(self) -> None:
        rows = self._repository.load()
        self._writer.write(rows)
~~~

调用方只需要依赖 `load` 和 `write` 行为，可用 Protocol 描述接口并在测试中传入 fake。Python 的多态常来自鸭子类型，不需要所有实现继承同一基类。

# 特殊方法与资源语义

`__repr__` 面向调试，避免泄漏密码和 token；`__eq__` 定义值相等后要同时考虑 `__hash__`，可变对象通常不应作为 dict key。`__enter__/__exit__` 定义上下文管理，`__iter__` 定义迭代，`__len__` 还会影响真值。

不要为了“Pythonic”实现大量特殊方法。每个协议都会产生用户预期：可迭代对象是否可重复消费、可哈希对象是否稳定、context manager 是否总能释放资源。

# 练习与边界

实现 `BatchJob`：构造时校验输入目录和并发上限；组合 `Reader`、`Processor`、`Writer` 三个小协议；用 fake 测试正常、空输入和写入失败。解释为什么不创建 `FileBatchJob`、`HttpBatchJob` 等深继承树。

> [!question]- 理解检查
> 1. 类属性中的可变对象为什么可能污染所有实例？
> 2. dataclass 的 `frozen=True` 为什么不是深度不可变？
> 3. 组合相比继承怎样改善测试与替换？

> [!info]- 官方参考
> - [Python Tutorial: Classes](https://docs.python.org/3/tutorial/classes.html)
> - [dataclasses](https://docs.python.org/3/library/dataclasses.html)
> - [Data Model](https://docs.python.org/3/reference/datamodel.html)
