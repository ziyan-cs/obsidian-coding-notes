---
status: learning
confidence: high
content_verified: 2026-09-18
verified: 2026-09-30
review_stage: learn
review_due: 2026-09-30
previous_review_due: 2026-09-22
tags: [language/python, python/object-model]
---

> [!abstract] 学习定位
> Python 名称绑定对象，赋值通常不复制对象；理解身份、相等、可变性和共享引用，是解释参数传递、容器修改与资源生命周期的基础。

> [!summary] 核心摘要
>
> 名称不是装值的盒子，而是当前作用域到对象的绑定。`a = b` 让两个名称指向同一对象；不可变对象的“修改”产生新对象，可变对象可以原地改变。复制前必须明确需要隔离哪一层状态。

# 名称、对象与类型

对象具有身份、类型和值；名称通过赋值绑定对象，类型属于对象而不是名称。

~~~python
x = 42
x = "ready"  # 名称 x 重新绑定到另一种对象
~~~

`id(obj)` 可观察一次运行中的身份，但业务代码不应依赖其具体数值。`type(obj)` 给出直接类型；判断替换关系通常用 `isinstance` 或 Protocol，而不是写死精确类型。

`==` 比较值语义，`is` 比较是否为同一对象。`None` 这类单例哨兵使用 `is`；字符串、整数和容器使用 `==`。解释器可能缓存小整数或字符串，不能据此把 `is` 当值比较。

# 可变与不可变

常见不可变对象包括 `int`、`float`、`bool`、`str`、`bytes`、`tuple`；常见可变对象包括 `list`、`dict`、`set` 和大多数自定义实例。不可变表示对象值不能原地改变，不代表名称不能重新绑定。

~~~python
text = "go"
before = id(text)
text += "od"          # 创建新字符串并重新绑定
assert id(text) != before

items = [1, 2]
before = id(items)
items.append(3)        # 原地修改同一个 list
assert id(items) == before
~~~

tuple 本身不可变，但可以引用可变元素；所以 tuple 不自动等于深度不可变，也不保证一定可哈希。

# 赋值与参数传递

~~~python
def add_record(records: list[str], name: str) -> None:
    records.append(name)

names: list[str] = []
alias = names
add_record(alias, "Ada")
assert names == ["Ada"]
~~~

函数调用时，形参绑定到实参对象。`append` 修改共享 list，所以调用者可以观察到；若函数执行 `records = []`，只是重新绑定局部名称。

接口应明确是否修改输入。命名可用 `update_*` 或 `*_in_place` 表示副作用；另一种选择是返回新结果。复制大对象也有时间和内存成本，应通过契约决定，而不是靠猜测。

# 浅复制、深复制与别名

~~~python
from copy import deepcopy

original = {"labels": ["backend"]}
shallow = original.copy()
deep = deepcopy(original)

shallow["labels"].append("python")
assert original["labels"] == ["backend", "python"]
assert deep["labels"] == ["backend"]
~~~

浅复制只复制最外层容器，内部对象继续共享。`deepcopy` 递归复制对象图，但可能昂贵，也不一定符合连接、锁、文件句柄或带身份实体的语义。更可靠的设计通常是构造明确的新领域对象，或让配置使用不可变值。

二维 list 的经典错误：

~~~python
bad = [[0] * 3] * 2
bad[0][0] = 1
assert bad == [[1, 0, 0], [1, 0, 0]]

good = [[0] * 3 for _ in range(2)]
~~~

第一种写法把同一个内层 list 引用了两次；推导式每轮创建新的内层对象。

# 哈希、相等与容器约束

dict key 和 set 元素必须可哈希，并且存放期间哈希值稳定。不可变内置值通常可哈希；包含 list 的 tuple 不可哈希。自定义类若按可变字段实现 `__hash__`，修改字段后对象可能再也无法从 set/dict 正确找到。

相等不等于身份。值对象可按字段比较；表示数据库行、连接或任务的实体可能需要稳定 ID。实现 `__eq__` 时同时考虑是否还应支持哈希，不要让容器语义自相矛盾。

# 生命周期与资源边界

对象至少在仍可达时存活。CPython 通常使用引用计数并辅以循环垃圾回收，但这是实现细节；代码不能依赖“最后一个引用消失后立刻清理”。文件、锁、事务和连接使用 `with` 管理，不能等待 `__del__`。

循环引用、全局容器、缓存、闭包和长期 task 都可能意外保留对象。排查内存增长时应寻找“谁仍然引用它”，并用可重复输入和内存工具验证，而不是只看分配位置。

# 实验

编写脚本依次演示普通赋值、list 浅复制、嵌套容器、函数原地修改与函数局部重绑。每一步先写预期的值、身份与共享关系，再运行验证。

随后设计一个配置对象并回答：调用方能否修改内部 list？getter 返回内部引用还是副本？多个 task 共享是否安全？没有这些契约，同一个对象会成为隐蔽耦合点。

> [!question]- 理解检查
> 1. `a = b` 对 list 做了什么，没有做什么？
> 2. 为什么 tuple 可能包含可变状态？
> 3. `deepcopy` 为什么不是共享问题的默认答案？
> 4. 为什么资源清理不能依赖垃圾回收时机？

> [!info]- 官方参考
> - [Python Data Model](https://docs.python.org/3/reference/datamodel.html)
> - [copy — Shallow and deep copy operations](https://docs.python.org/3/library/copy.html)
