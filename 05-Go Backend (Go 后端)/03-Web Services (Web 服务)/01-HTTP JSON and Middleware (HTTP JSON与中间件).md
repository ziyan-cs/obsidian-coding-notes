---
status: learning
confidence: low
content_verified: 2026-09-17
tags: [language/go, go/http]
---

> [!abstract] 学习定位
> HTTP handler 负责协议转换，service 负责业务；先用 `net/http` 和 `httptest` 看清请求生命周期，再引入框架。

> [!tip] 先守住边界，再增加框架
> 一个 handler 最有价值的职责是把不可信 HTTP 输入变成可验证的业务参数，并把业务结果稳定地映射为 HTTP 响应。路由框架不能替你完成这层边界设计。

# 最小 handler

```go
type greeting struct { Message string `json:"message"` }

func hello(w http.ResponseWriter, r *http.Request) {
    if r.Method != http.MethodGet {
        http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
        return
    }
    w.Header().Set("Content-Type", "application/json")
    json.NewEncoder(w).Encode(greeting{Message: "hello"})
}

func main() {
    mux := http.NewServeMux()
    mux.HandleFunc("GET /hello", hello) // Go 1.22+ pattern
    log.Fatal(http.ListenAndServe(":8080", mux))
}
```

# 分层

```text
HTTP request → middleware → handler → service → repository
HTTP response ← error mapping ← result  ← data access
```

- **handler**：解析、校验、HTTP status/JSON 转换；不写复杂业务。
- **service**：业务规则、事务边界、协调依赖。
- **repository**：数据库/缓存/外部服务访问。
- **middleware**：日志、request ID、认证、恢复、超时等横切逻辑。

> [!warning]- 易错点
> - 先写响应头再写 body；写 body 后 status 通常已默认成 200。
> - JSON decode 后检查多余字段、大小限制和必填项；不要把用户输入直接信任为结构正确。
> - `http.Error` 是纯文本；JSON API 应统一自己的错误响应格式。
> - `log.Fatal` 会调用 `os.Exit`，不应出现在可测试的 handler/service 中。

# 统一错误响应

```go
type apiError struct {
    Code    string `json:"code"`
    Message string `json:"message"`
}

func writeJSONError(w http.ResponseWriter, status int, code, message string) {
    w.Header().Set("Content-Type", "application/json")
    w.WriteHeader(status)
    _ = json.NewEncoder(w).Encode(apiError{Code: code, Message: message})
}
```

> [!note] 先写 `WriteHeader`
> HTTP 状态码必须在写入响应 body 前确定；`Encode` 可能立刻写入 body，晚设置状态会得到默认的 `200 OK`。

# 把输入边界写完整

JSON API 需要限制 body、拒绝未知字段，并确认输入只有一个 JSON 值。否则超大请求可能消耗内存，拼写错误字段会被静默忽略，尾随垃圾数据也可能漏过。

```go
func decodeJSON(w http.ResponseWriter, r *http.Request, dst any) error {
    r.Body = http.MaxBytesReader(w, r.Body, 1<<20) // 示例值按接口决定
    dec := json.NewDecoder(r.Body)
    dec.DisallowUnknownFields()
    if err := dec.Decode(dst); err != nil {
        return fmt.Errorf("decode request: %w", err)
    }
    if err := dec.Decode(&struct{}{}); err != io.EOF {
        return errors.New("request body must contain one JSON value")
    }
    return nil
}
```

语法校验后仍需业务校验，例如名称长度、枚举取值和跨字段约束。唯一键冲突通常映射为 `409 Conflict`；依赖超时更接近 `504 Gateway Timeout`；未知内部错误返回稳定错误码，不把 SQL 或堆栈暴露给客户端。

# Middleware 的控制流

标准 middleware 是 `func(http.Handler) http.Handler`。越靠外层越早收到请求、越晚看到响应结束，因此 panic recovery、request ID、访问日志、认证、超时的顺序会影响可见上下文。

```go
func requestID(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        id := r.Header.Get("X-Request-ID")
        if id == "" { id = newRequestID() }
        ctx := context.WithValue(r.Context(), requestIDKey{}, id)
        next.ServeHTTP(w, r.WithContext(ctx))
    })
}
```

只把跨层真正需要且属于请求范围的元数据放入 context；业务参数仍用显式参数。认证确认主体是谁，授权判断该主体能否操作当前资源，两者不能混为一谈。

# Server 与 Client 都要有超时

不要直接使用没有边界配置的 `http.ListenAndServe` 作为生产入口：

```go
srv := &http.Server{
    Addr:              ":8080",
    Handler:           mux,
    ReadHeaderTimeout: 5 * time.Second,
    IdleTimeout:       60 * time.Second,
    MaxHeaderBytes:    1 << 20,
}
```

具体值必须根据负载和代理链测量。写超时对 streaming/长轮询有额外约束，需要按接口设计。出站请求使用复用的 `http.Client` 和配置合理的 `Transport`；不要每次创建新 client，也不要只依赖默认的无总超时行为。

官方参考：[net/http package](https://pkg.go.dev/net/http)、[encoding/json package](https://pkg.go.dev/encoding/json)。

> [!summary] 核心摘要
>
> Go 的 HTTP 分层中，handler 只处理协议边界：路由、输入校验、鉴权上下文与 JSON/状态码转换；service 保存业务规则和事务边界；repository 隔离存储细节。中间件放日志、超时、恢复和认证等横切关注点。这样既便于 `httptest` 覆盖边界，也便于替换实现。

> [!question]- 自测：先回答再展开
> 1. `POST /todos` 中，哪些错误应该映射为 400、401、404、409、500？
> 2. 为什么不应让 repository 直接返回 HTTP 状态码？
> 3. 怎样用 `httptest` 验证响应的状态码、`Content-Type` 和 JSON body？

# 练习

写 `GET /healthz` 与 `POST /todos`，限制 body，拒绝未知字段，并用 `httptest.NewRecorder` 覆盖 200、400、405、413。再用一个会超时的假 repository 验证 request context 能终止下游工作。

> [!info]- 延伸阅读
> - 下一步：[02-Routing Validation and Errors (路由校验与错误)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/02-Routing%20Validation%20and%20Errors%20(路由校验与错误).md)

