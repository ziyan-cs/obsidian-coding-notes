---
study_stage: backlog
review_due:
tags: [language/python, topic/data-processing]
---

> [!abstract] 本篇要点
> Python 的核心语法围绕“对象组成的容器”和“协议驱动的操作”展开；选对容器、写清边界，比追求一行代码更重要。

# 控制流与真值

缩进定义代码块。`if/elif/else` 表达互斥分支，`for` 遍历 iterable，`while` 表达条件仍成立就继续。`break` 结束最近一层循环，`continue` 进入下一轮；循环的 `else` 只在没有被 `break` 打断时执行，实际项目中应谨慎使用，避免读者误解。

`None`、数值零、空字符串和空容器是假值，其余对象通常为真。判断“参数未提供”使用 `is None`，不要写 `== None`；`is` 比较对象身份，`==` 比较值是否相等。

~~~python
def normalize_port(value: str | None) -> int:
    if value is None:
        return 8080
    port = int(value)
    if not 1 <= port <= 65535:
        raise ValueError(f"invalid port: {port}")
    return port
~~~

# 四种核心容器

| 类型 | 顺序 | 可变 | 典型操作 | 主要风险 |
| --- | --- | --- | --- | --- |
| `list[T]` | 有 | 是 | 追加、索引、切片 | 中间插入删除通常需要移动元素 |
| `tuple[...]` | 有 | 否 | 固定记录、拆包 | tuple 不可变不代表内部对象不可变 |
| `dict[K, V]` | 保留插入顺序 | 是 | 按键查询、更新 | key 必须可哈希；不要依赖缺失键自动存在 |
| `set[T]` | 无业务顺序 | 是 | 去重、集合运算 | 输出顺序不应作为接口契约 |

`x in list` 通常线性扫描；`x in set` 和 `key in dict` 平均接近常数时间，但仍有哈希计算、内存和最坏情况成本。数据规模很小时，可读性优先于容器微优化。

~~~python
records = [
    {"user_id": 1, "status": "ok"},
    {"user_id": 2, "status": "failed"},
    {"user_id": 1, "status": "ok"},
]

successful_users = {
    row["user_id"]
    for row in records
    if row["status"] == "ok"
}
~~~

# 切片、拆包与复制

切片 `[start:stop:step]` 的 `stop` 不包含在结果中。对 list 的普通切片会创建浅复制：外层 list 是新的，内部可变元素仍共享。

~~~python
matrix = [[1], [2]]
copy = matrix[:]
copy[0].append(9)
assert matrix == [[1, 9], [2]]  # 内层 list 仍共享
~~~

拆包要求元素数量匹配，`*rest` 可接收剩余部分。交换变量 `a, b = b, a` 先计算右侧 tuple-like 结果，再绑定左侧名称，不需要临时变量。

`dict.get(key)` 适合缺失时有正常默认值；若缺失是数据错误，应直接索引并让 `KeyError` 暴露问题，或显式校验。不要用 `get` 把损坏的数据静默变成 `None`。

# 推导式与生成器表达式

推导式由“结果表达式 + 遍历 + 可选过滤”组成：

~~~python
latencies_ms = [12, 31, 8, 105]
slow = [value for value in latencies_ms if value >= 30]
by_index = {index: value for index, value in enumerate(slow)}
~~~

若包含多层嵌套、异常处理、副作用或难懂条件，改写为普通循环。推导式的目标是让数据变换更清楚，不是压缩行数。

方括号推导式立即创建完整 list；圆括号生成器表达式按需产生元素，更适合一次消费的大输入：

~~~python
total = sum(int(line) for line in lines if line.strip())
~~~

# 工程边界与练习

- 不要在遍历 list/dict 时随意改变其大小；需要修改时遍历副本或先收集变更。
- 业务输出需要稳定顺序时显式排序，不依赖 set 或外部数据碰巧有序。
- `sorted()` 返回新 list；`list.sort()` 原地修改并返回 `None`。
- `[[0] * 3] * 4` 会让四行引用同一个内层 list，应写 `[[0] * 3 for _ in range(4)]`。

练习：读取若干 `{user_id, endpoint, status}` 记录，输出每个 endpoint 的失败用户集合和失败次数。分别用循环与推导式实现，并说明哪一版更容易扩展错误校验。

**参考资料：** [Python Tutorial: Data Structures](https://docs.python.org/3/tutorial/datastructures.html) · [Python Tutorial: Control Flow Tools](https://docs.python.org/3/tutorial/controlflow.html)

# 考察题目

## 1. 实现：失败用户统计

实现 `failed_users(records: list[dict[str, object]]) -> dict[str, set[int]]`。每条记录包含 `endpoint: str`、`user_id: int`、`status: str`；仅统计 `status == "failed"`，按 endpoint 收集去重的用户 ID。缺字段或类型错误抛 `ValueError`，先用普通循环实现。测试 `user_id=True`。

> [!note]- 参考实现
> ```python
> def failed_users(records: list[dict[str, object]]) -> dict[str, set[int]]:
>     result: dict[str, set[int]] = {}
>     for record in records:
>         if not isinstance(record, dict):
>             raise ValueError("record must be a dict")
>         try:
>             endpoint = record["endpoint"]
>             user_id = record["user_id"]
>             status = record["status"]
>         except KeyError as exc:
>             raise ValueError("missing field") from exc
>         if not isinstance(endpoint, str) or type(user_id) is not int or not isinstance(status, str):
>             raise ValueError("invalid field type")
>         if status == "failed":
>             result.setdefault(endpoint, set()).add(user_id)
>     return result
> ```
> `bool` 是 `int` 的子类，这里用 `type(user_id) is int` 明确拒绝布尔值。

## 2. 机制追问

列表推导式与生成器表达式何时选择不同？

> [!note]- 参考答案
> 列表推导式立即构造结果并占用相应内存；生成器惰性迭代、适合流式处理，但只能按迭代语义消费，还要注意副作用和异常出现时机。
