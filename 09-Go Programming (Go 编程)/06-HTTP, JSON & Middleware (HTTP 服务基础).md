---
tags: [language/go, go/http]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# HTTP, JSON & Middleware — 标准库写 API

> **一句话结论**：HTTP handler 负责协议转换，service 负责业务；先用 `net/http` 和 `httptest` 看清请求生命周期，再引入框架。

## 最小 handler

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

## 分层

```text
HTTP request → middleware → handler → service → repository
HTTP response ← error mapping ← result  ← data access
```

- **handler**：解析、校验、HTTP status/JSON 转换；不写复杂业务。
- **service**：业务规则、事务边界、协调依赖。
- **repository**：数据库/缓存/外部服务访问。
- **middleware**：日志、request ID、认证、恢复、超时等横切逻辑。

## 常见坑

- 先写响应头再写 body；写 body 后 status 通常已默认成 200。
- JSON decode 后检查多余字段、大小限制和必填项；不要把用户输入直接信任为结构正确。
- `http.Error` 是纯文本；JSON API 应统一自己的错误响应格式。
- `log.Fatal` 会调用 `os.Exit`，不应出现在可测试的 handler/service 中。

## 练习

写 `GET /healthz` 与 `POST /todos`，并用 `httptest.NewRecorder` 覆盖 200、400、405。
