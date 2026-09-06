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

**核心结论**：一句话结论：Python 变量绑定对象而非“存放一个 C++ 值”；可变对象共享引用是初学阶段最重要的坑。


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

- 把脚本一次跑通当成工程正确，忽略环境隔离、输入校验、错误分类和可重复运行。
- 只读 API 名称而不为文件、网络、时间等外部边界写一个可控测试。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
02-Objects Types and Functions (对象类型与函数)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
