---
status: learning
confidence: low
verified: 2026-09-06
tags: [language/go, go/errors, go/context]
---

> [!abstract] 一句话结论：Go 用显式 `error` 表达可预期失败，用 `defer` 绑定局部清理，用 `context` 传播请求取消、截止时间和请求范围数据。

# 30 秒回答

**回答展开**：函数返回后立刻处理 `error` 并补充上下文；资源一获取就 `defer` 释放；跨调用链只传递派生出的 `context`，并尊重取消与超时。

# 最小例子

```go
func readName(ctx context.Context, path string) (string, error) {
    f, err := os.Open(path)
    if err != nil { return "", fmt.Errorf("open %q: %w", path, err) }
    defer f.Close()

    select {
    case <-ctx.Done():
        return "", ctx.Err()
    default:
    }
    b, err := io.ReadAll(f)
    if err != nil { return "", fmt.Errorf("read name: %w", err) }
    return strings.TrimSpace(string(b)), nil
}
```

# 规则

- 错误发生处立刻处理、包装或返回；调用者用 `errors.Is` / `errors.As` 判断语义。
- `defer` 在**所在函数返回时**执行，采用后进先出；循环内 `defer` 可能延迟过久。
- `context.Context` 作为第一个参数传入；不要放进 struct，不要传 `nil`，不要用它传可选业务参数。
- `panic` 适合无法恢复的程序员错误或启动期不变量被破坏，不适合普通请求失败。

# C++ 对照

| C++ | Go | 关键差异 |
| --- | --- | --- |
| exception | `error` 返回值 | 失败路径在签名中显式可见 |
| RAII 析构 | `defer` | `defer` 是函数作用域，不是对象作用域 |
| stop token | `context` | 同时承载 deadline/cancellation 传播 |

# 自测

1. 为什么 `defer f.Close()` 应紧跟成功 `Open`？
2. 为什么不能用 `context` 传数据库连接或业务参数？
3. 什么时候应该包装 error，什么时候原样返回？

# 从零建立模型

本页主题是 **03-Errors Defer and Context (错误、defer 与 context)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 渐进练习

1. **第一步 · 理解**：读：标出本页代码中错误向上返回、资源释放和 goroutine 退出的位置。
2. **第二步 · 实现**：写：为一个纯业务函数补 table-driven test；若有并发，写一个取消或关闭案例。
3. **第三步 · 验证**：测：运行 `go test`，并在适用时运行 `go test -race` 或 benchmark，记录结论与环境。

# 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Errors Defer and Context (错误、defer 与 context)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Go Map (Go导航)](/04-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/00-Go%20Map%20(Go导航).md)
- 下一步：[02-Modules Packages and Tooling (模块包与工具链)](/04-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/02-Modules%20Packages%20and%20Tooling%20(模块包与工具链).md)
