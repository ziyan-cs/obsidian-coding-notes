---
tags: [language/go, go/http]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-HTTP JSON and Middleware (HTTP JSON与中间件)

> [!abstract] 一句话结论：HTTP handler 负责协议转换，service 负责业务；先用 `net/http` 和 `httptest` 看清请求生命周期，再引入框架。

> [!tip] 先守住边界，再增加框架
> 一个 handler 最有价值的职责是把不可信 HTTP 输入变成可验证的业务参数，并把业务结果稳定地映射为 HTTP 响应。路由框架不能替你完成这层边界设计。

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

## 统一错误响应

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

## 30 秒回答

Go 的 HTTP 分层中，handler 只处理协议边界：路由、输入校验、鉴权上下文与 JSON/状态码转换；service 保存业务规则和事务边界；repository 隔离存储细节。中间件放日志、超时、恢复和认证等横切关注点。这样既便于 `httptest` 覆盖边界，也便于替换实现。

## 自测

1. `POST /todos` 中，哪些错误应该映射为 400、401、404、409、500？
2. 为什么不应让 repository 直接返回 HTTP 状态码？
3. 怎样用 `httptest` 验证响应的状态码、`Content-Type` 和 JSON body？

## 练习

写 `GET /healthz` 与 `POST /todos`，并用 `httptest.NewRecorder` 覆盖 200、400、405。



## 零基础阅读路径

先理解普通函数、显式错误和 package 边界；接着跟一条请求经过 handler、service、repository；最后阅读 goroutine、context 与运行时部分。并发章节必须配最小程序。

## 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-HTTP JSON and Middleware (HTTP JSON与中间件)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
