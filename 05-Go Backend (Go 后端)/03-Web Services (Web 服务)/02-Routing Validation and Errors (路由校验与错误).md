---
study_stage: backlog
---

> [!abstract] 学习定位
> HTTP handler 的职责是把协议输入转换为明确的应用命令，并把可预期失败稳定地映射为响应；业务规则不要散落在 handler 内。

> [!summary] 核心摘要
>
> handler 只做解析、校验、调用服务与映射响应；领域服务维护业务不变量；用统一错误码和响应体让客户端得到可预测结果。

# 输入限制与解码

所有 HTTP 输入都不可信。先限制 body 大小，再解码；JSON decoder 可拒绝未知字段，并确认文档结束后没有第二个 JSON 值。路径 ID、查询参数、header 与 body 分别解析成明确类型，再进入 service。

```go
r.Body = http.MaxBytesReader(w, r.Body, 1<<20)
dec := json.NewDecoder(r.Body)
dec.DisallowUnknownFields()
if err := dec.Decode(&input); err != nil {
    writeError(w, requestID, ErrInvalidJSON)
    return
}
if err := dec.Decode(&struct{}{}); err != io.EOF {
    writeError(w, requestID, ErrInvalidJSON)
    return
}
```

最大 body、字符串长度、分页上限和批量数量属于资源保护，不只是产品校验。校验顺序应确定，避免同一输入随机返回不同错误。

# 错误类型与映射

service 返回领域错误，不返回 HTTP status。handler 使用 `errors.Is/As` 将稳定错误分类映射到 400/404/409/429/5xx；未知错误统一记录完整链并向客户端返回不泄漏内部细节的 500。

错误响应先写 header 和 status，再编码 body；一旦 body 已写出就不能可靠改变 status。可使用集中 responder 确保 Content-Type、error code 和 request ID 一致，但不要把所有 handler 业务塞入万能中间件。

# 路由、认证与授权

路由负责 method 与路径匹配。认证确认调用者是谁，授权判断该主体能否操作目标资源；即使 URL 中 user ID 合法，也必须检查对象归属，防止对象级越权。

中间件顺序会改变行为：request ID 和恢复通常靠外，认证在业务 handler 前，日志应记录最终 status 和耗时。限流 key、代理 IP 和可信转发 header 必须基于部署拓扑设计，不能盲信客户端传来的 `X-Forwarded-For`。

# 协议测试

用 `httptest` 覆盖：错误 method、body 超限、未知字段、尾随 JSON、缺失字段、对象越权、领域冲突、依赖 timeout 与未知内部错误。断言 status、Content-Type、稳定 code、request ID 和敏感信息未泄漏。
# 一张可执行的错误映射表

| 来源 | 建议状态 | 客户端可见内容 |
| --- | --- | --- |
| JSON 语法、字段类型、未知字段 | 400 | 稳定 `invalid_argument`，指出可修正字段 |
| 请求体超过上限 | 413 | 不回显原始 body |
| 未认证 / 无权限 | 401 / 403 | 不泄漏对象是否存在等敏感事实 |
| 资源不存在 / 唯一键冲突 | 404 / 409 | 稳定业务 code |
| 内部错误 | 500 | 固定泛化信息，细节只进日志 |
| 依赖超时 | 由服务角色和 API 契约决定 | 作为网关时可用 504；不要机械把所有数据库 timeout 都标 504 |

在 handler 中按 `errors.Is/As` 判断已定义领域错误；对 `*http.MaxBytesError` 单独映射 413。错误 body 至少包含稳定 `code`、可安全展示的 `message` 和 `request_id`，例如 `{"code":"invalid_argument","message":"title is required","request_id":"..."}`。日志保留完整错误链，但过滤凭据和原始敏感输入。

> [!info]- 延伸阅读
> - 下一步：[03-API Lifecycle and Graceful Shutdown (服务生命周期)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/03-API%20Lifecycle%20and%20Graceful%20Shutdown%20(服务生命周期).md)

