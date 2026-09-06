---
tags: [language/go, go/basics]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

> [!abstract] 一句话结论：Go 用组合（composition）和小 interface 表达行为；interface 是隐式满足的，不是 C++ 的继承层级。

# 30 秒回答

**核心结论**：一句话结论：Go 用组合（composition）和小 interface 表达行为；interface 是隐式满足的，不是 C++ 的继承层级。


# 核心语法

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

# 接口的正确使用

接口应在**使用方**定义，并尽量小：例如 `io.Reader` 只需 `Read`。先写具体类型；当测试替身或多实现真实出现时再抽 interface。

```go
type UserStore interface { Find(context.Context, int64) (User, error) }
```

# 常见坑

- 将 `interface{}` / `any` 当作“泛型”；优先具体类型或真正的 generics。
- 混用值 receiver 和指针 receiver，导致某个类型不满足预期接口。
- 用 interface 模拟多层 OOP 继承；Go 更常用 struct 嵌入与函数组合。

# C++ 对照

| 问题 | C++ | Go |
| --- | --- | --- |
| 多态 | virtual / inheritance | 隐式 interface 实现 |
| 修改对象 | reference / pointer | `*T` receiver |
| 泛型 | template | type parameter（仅需要时） |
| 可见性 | `public/private` | 首字母大小写 + package |

# 自测

1. 为什么 `*User` 可能实现 interface，而 `User` 不实现？
2. 哪种情况下该先写 interface，哪种情况下不该？



# 从零建立模型

本页主题是 **01-Types Functions and Interfaces (类型函数与接口)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Types Functions and Interfaces (类型函数与接口)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
