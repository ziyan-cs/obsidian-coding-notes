---
tags: [language/go, go/basics]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-Types Functions and Interfaces (类型函数与接口)

> [!abstract] 一句话结论：Go 用组合（composition）和小 interface 表达行为；interface 是隐式满足的，不是 C++ 的继承层级。

## 30 秒回答

**核心结论**：一句话结论：Go 用组合（composition）和小 interface 表达行为；interface 是隐式满足的，不是 C++ 的继承层级。


## 核心语法

```go
type User struct { ID int64; Name string }

func (u User) Greeting() string { return "hi, " + u.Name }      // 值 receiver
func (u *User) Rename(name string) { u.Name = name }             // 指针 receiver

type Greeter interface { Greeting() string }

func printGreeting(g Greeter) { fmt.Println(g.Greeting()) }
```

- 大写开头的标识符（`User`）对包外可见（exported）；小写仅包内可见。
- `:=` 用于函数内“至少一个新变量”的短声明；公开 API 中别为了短而牺牲清晰。
- map、slice、channel 是引用到运行时结构的值；复制它们不等于复制全部元素。

## 接口的正确使用

接口应在**使用方**定义，并尽量小：例如 `io.Reader` 只需 `Read`。先写具体类型；当测试替身或多实现真实出现时再抽 interface。

```go
type UserStore interface { Find(context.Context, int64) (User, error) }
```

## 常见坑

- 将 `interface{}` / `any` 当作“泛型”；优先具体类型或真正的 generics。
- 混用值 receiver 和指针 receiver，导致某个类型不满足预期接口。
- 用 interface 模拟多层 OOP 继承；Go 更常用 struct 嵌入与函数组合。

## C++ 对照

| 问题 | C++ | Go |
| --- | --- | --- |
| 多态 | virtual / inheritance | 隐式 interface 实现 |
| 修改对象 | reference / pointer | `*T` receiver |
| 泛型 | template | type parameter（仅需要时） |
| 可见性 | `public/private` | 首字母大小写 + package |

## 自测

1. 为什么 `*User` 可能实现 interface，而 `User` 不实现？
2. 哪种情况下该先写 interface，哪种情况下不该？

## 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
01-Types Functions and Interfaces (类型函数与接口)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
