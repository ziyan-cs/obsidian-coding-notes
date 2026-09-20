---
study_stage: backlog
tags: [language/go, go/errors, go/context]
---

> [!abstract] 学习定位
> Go 用显式 `error` 表达可预期失败，用 `defer` 绑定局部清理，用 `context` 传播请求取消、截止时间和请求范围数据。

> [!summary] 核心摘要
>
> 函数返回后立刻处理 `error` 并补充上下文；资源一获取就 `defer` 释放；跨调用链只传递派生出的 `context`，并尊重取消与超时。

# 错误语义与包装

error 是普通接口值。错误信息为人提供上下文，错误类型或 sentinel 为程序提供稳定判断。使用 `%w` 包装保留链，调用方通过 `errors.Is/As` 判断，不比较完整错误字符串。

```go
var ErrNotFound = errors.New("not found")

func loadUser(id int64) (User, error) {
    user, err := repository.Find(id)
    if err != nil {
        return User{}, fmt.Errorf("find user %d: %w", id, err)
    }
    return user, nil
}
```

只在增加本层有用信息时包装，避免每层重复“failed to”。公开 sentinel/type 会成为 API 契约；先判断调用者是否真的需要分支处理。错误可按无效输入、冲突、未找到、瞬态依赖和内部故障分类，再由 HTTP/RPC 边界映射状态。

# Defer 的求值与作用域

`defer` 注册时立即求值函数值与参数，函数返回前按后进先出执行。闭包则在执行时读取捕获变量。理解这个差别才能解释循环和日志中的意外值。

资源获取成功后立即 defer 清理，并检查重要 Close/Flush/Commit 错误。循环内直接 defer 会等整个函数返回，可能同时占用大量文件；把每轮逻辑提取到小函数，使资源及时释放。

事务使用 `defer tx.Rollback()` 覆盖所有提前返回，再显式 `Commit`；但 commit 错误仍必须返回。defer 不是 RAII：它绑定函数作用域，不能随对象离开任意块自动析构。

# Panic 与 Recover 边界

普通请求失败返回 error。panic 表示程序不变量被破坏、无法继续的初始化错误，或运行时错误。库不应为普通输入 panic，让所有调用者被迫 recover。

`recover` 只在同一 goroutine 的 deferred 函数中生效。HTTP server 可在请求边界恢复，记录堆栈并返回 500，使单个请求不终止整个进程；恢复后仍要判断共享状态是否可信。后台 goroutine 的 panic 不会被另一个 goroutine 的 recover 捕获。

# Context 的职责

context 传播取消、deadline 和少量请求范围元数据。把它作为第一个参数，不保存进长期 struct，不传 nil，不用它隐藏业务配置。派生 context 后总是调用 cancel，以便尽早释放 timer 和子树引用。

只在调用支持 context 的阻塞 API 或 select 监听 `Done` 时，取消才真正生效。入口检查一次然后执行长 CPU 循环，并不会响应客户端断开。

# 失败设计练习

实现批量导入：非法记录返回带行号的错误；数据库 timeout 保留错误链；事务失败回滚；顶层把错误分类为可重试和永久失败。注入一次 panic，验证只有请求边界 recover，同时日志包含 stack 和 request ID 且不泄漏原始敏感记录。
# 两种不同的资源边界

本地文件读取和 HTTP 请求都要及时释放资源，但取消能力不同。`os.File` 的普通读取没有 `context.Context` 参数；读取前做一次 `ctx.Done()` 检查，**不能**让随后的 `io.ReadAll` 因取消自动中断。因此下例把文件大小单独设上限，而把请求取消交给真正支持 context 的 HTTP API。

```go
package lesson

import (
    "context"
    "fmt"
    "io"
    "net/http"
    "os"
    "strings"
)

func readName(path string) (string, error) {
    f, err := os.Open(path)
    if err != nil {
        return "", fmt.Errorf("open name file: %w", err)
    }
    defer f.Close()

    const maxBytes = 4096
    body, err := io.ReadAll(io.LimitReader(f, maxBytes+1))
    if err != nil {
        return "", fmt.Errorf("read name file: %w", err)
    }
    if len(body) > maxBytes {
        return "", fmt.Errorf("name file exceeds %d bytes", maxBytes)
    }
    return strings.TrimSpace(string(body)), nil
}

func getStatus(ctx context.Context, client *http.Client, url string) (int, error) {
    req, err := http.NewRequestWithContext(ctx, http.MethodGet, url, nil)
    if err != nil {
        return 0, fmt.Errorf("build request: %w", err)
    }
    resp, err := client.Do(req)
    if err != nil {
        return 0, fmt.Errorf("send request: %w", err)
    }
    defer resp.Body.Close()
    return resp.StatusCode, nil
}
```

`getStatus` 返回 HTTP 状态码，但不会自动把非 2xx 当成 Go `error`；调用方还需按协议判断。`client` 应配置 timeout 并复用；上下文取消是否能及时终止具体 I/O，要通过测试和客户端实现验证。关闭错误若影响数据正确性（例如写文件的 flush/close），要显式检查，不能仅靠 `defer Close()`。

# C++ 对照

| C++ | Go | 关键差异 |
| --- | --- | --- |
| exception | `error` 返回值 | 失败路径在签名中显式可见 |
| RAII 析构 | `defer` | `defer` 是函数作用域，不是对象作用域 |
| stop token | `context` | 同时承载 deadline/cancellation 传播 |

> [!question]- 自测：先回答再展开
> 1. 为什么 `defer f.Close()` 应紧跟成功 `Open`？
> 2. 为什么不能用 `context` 传数据库连接或业务参数？
> 3. 什么时候应该包装 error，什么时候原样返回？
