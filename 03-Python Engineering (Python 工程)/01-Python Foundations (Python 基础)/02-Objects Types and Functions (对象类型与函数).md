---
status: learning
confidence: low
verified: 2026-09-17
review_due: 2026-09-22
tags: [language/python, python/basics]
---

> [!abstract] 一句话结论：Python 变量绑定对象而非“存放一个 C++ 值”；可变对象共享引用是初学阶段最重要的坑。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：变量保存的是对象引用；赋值和传参默认传递引用。遇到 list、dict 等可变对象时，先判断是否共享，再决定复制或原地修改。

# 常用对象

| 类型 | 适合 | 常见坑 |
| --- | --- | --- |
| `list` | 有序、可变序列 | 复制引用而非元素 |
| `dict` | 键到值映射 | 键必须可 hash |
| `set` | 去重、成员判断 | 无顺序语义 |
| `tuple` | 固定记录 | tuple 内元素仍可能可变 |
| `str` | 文本 | 不可变；频繁拼接用 `''.join` |

# 函数规则

- 类型标注帮助阅读、IDE 和检查工具，不会默认在运行时强制执行。
- 返回 `None` 表示“没有值”时，要明确是否为正常分支。
- 用 keyword-only 参数（`def f(*, timeout: float)`）让配置调用更清楚。

# 经典反例：可变默认参数

```python
# 错：默认 list 只在定义函数时创建一次
def append_bad(x, items=[]):
    items.append(x)
    return items

def append_ok(x, items=None):
    if items is None:
        items = []
    items.append(x)
    return items
```

# C++ / Go 对照

- C++ 关注值、引用、移动及所有权；Python 先问“这个名字是否和别处指向同一个可变对象”。
- Go 的 slice/map 也有共享底层数据的语义，但 Python 没有编译期类型保证。

> [!question]- 自测：先回答再展开
> 1. `a = b` 是否复制 list？怎样明确复制？
> 2. 为什么 `None` 常用作可变默认值的哨兵？

> [!info]- 延伸阅读
> - 下一步：[03-Path Files and Serialization (路径文件与序列化)](/03-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/03-Path%20Files%20and%20Serialization%20(路径文件与序列化).md)
