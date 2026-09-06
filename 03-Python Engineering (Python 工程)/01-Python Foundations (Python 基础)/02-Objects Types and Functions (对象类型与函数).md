---
tags: [language/python, python/basics]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Core Types, Data Model & Functions — 先理解对象和引用

> [!abstract] 一句话结论：Python 变量绑定对象而非“存放一个 C++ 值”；可变对象共享引用是初学阶段最重要的坑。

```python
def add_tag(tags: list[str], tag: str) -> list[str]:
    return [*tags, tag]  # 返回新 list，避免修改调用方

names = ["Ada"]
alias = names
alias.append("Lin")
assert names == ["Ada", "Lin"]  # 两个名字指向同一对象
```

## 常用对象

| 类型 | 适合 | 常见坑 |
| --- | --- | --- |
| `list` | 有序、可变序列 | 复制引用而非元素 |
| `dict` | 键到值映射 | 键必须可 hash |
| `set` | 去重、成员判断 | 无顺序语义 |
| `tuple` | 固定记录 | tuple 内元素仍可能可变 |
| `str` | 文本 | 不可变；频繁拼接用 `''.join` |

## 函数规则

- 类型标注帮助阅读、IDE 和检查工具，不会默认在运行时强制执行。
- 返回 `None` 表示“没有值”时，要明确是否为正常分支。
- 用 keyword-only 参数（`def f(*, timeout: float)`）让配置调用更清楚。

## 经典反例：可变默认参数

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

## C++ / Go 对照

- C++ 关注值、引用、移动及所有权；Python 先问“这个名字是否和别处指向同一个可变对象”。
- Go 的 slice/map 也有共享底层数据的语义，但 Python 没有编译期类型保证。

## 自测

1. `a = b` 是否复制 list？怎样明确复制？
2. 为什么 `None` 常用作可变默认值的哨兵？
