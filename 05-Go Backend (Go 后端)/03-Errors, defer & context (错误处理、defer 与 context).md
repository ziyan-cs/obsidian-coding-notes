---
tags: [language/go, go/errors, go/context]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Errors, defer & context — 失败、清理与取消

> **一句话结论**：Go 用显式 `error` 表达可预期失败，用 `defer` 绑定局部清理，用 `context` 传播请求取消、截止时间和请求范围数据。

## 最小例子

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

## 规则

- 错误发生处立刻处理、包装或返回；调用者用 `errors.Is` / `errors.As` 判断语义。
- `defer` 在**所在函数返回时**执行，采用后进先出；循环内 `defer` 可能延迟过久。
- `context.Context` 作为第一个参数传入；不要放进 struct，不要传 `nil`，不要用它传可选业务参数。
- `panic` 适合无法恢复的程序员错误或启动期不变量被破坏，不适合普通请求失败。

## C++ 对照

| C++ | Go | 关键差异 |
| --- | --- | --- |
| exception | `error` 返回值 | 失败路径在签名中显式可见 |
| RAII 析构 | `defer` | `defer` 是函数作用域，不是对象作用域 |
| stop token | `context` | 同时承载 deadline/cancellation 传播 |

## 自测

1. 为什么 `defer f.Close()` 应紧跟成功 `Open`？
2. 为什么不能用 `context` 传数据库连接或业务参数？
3. 什么时候应该包装 error，什么时候原样返回？
