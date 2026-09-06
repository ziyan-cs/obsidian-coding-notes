---
tags: [language/python, python/basics]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 02-Objects Types and Functions (对象类型与函数)

> [!abstract] 一句话结论：Python 变量绑定对象而非“存放一个 C++ 值”；可变对象共享引用是初学阶段最重要的坑。

## 30 秒回答

**02-Objects Types and Functions (对象类型与函数)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


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

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 02-Objects Types and Functions (对象类型与函数) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
