---
study_stage: backlog
tags: [go/project, backend/delivery]
---

> [!abstract] 学习定位
> 第一个 Go 服务的目标不是堆功能，而是把一个小业务从契约、实现、数据、测试、观测到可重复交付完整走通，并留下别人能够复核的证据。

> [!summary] 核心摘要
>
> 以短链接 API 为例，先完成内存版业务规则，再接入 HTTP、MySQL 和 Redis；所有外部调用都服从请求 deadline，所有失败都有稳定语义，最终通过测试、压测、日志和关闭实验证明服务不仅“能启动”，而且能被验证和运维。

# 项目范围与完成定义

选择短链接服务作为第一个项目，因为它同时包含唯一性、缓存、重定向、热点读和过期语义。第一版的完成时间取决于你的 Go 基础和真实投入，不预设“一周做完”。

第一版只实现：

- `POST /v1/links`：校验 URL，创建唯一短码；
- `GET /{code}`：查询有效链接并返回 `302`；
- `GET /v1/links/{code}`：查询元数据；
- MySQL 保存事实，Redis 加速读取；
- JSON 结构化日志、请求指标、健康检查和优雅关闭。

暂不实现登录、多地域、复杂分析和自研发号器。完成的判断不是功能数量，而是：新机器能按 README 启动；测试可重复；失败路径可观察；停止进程不会无界丢请求；压测结果带环境与原始命令。

# 架构与请求路径

```text
client
  -> HTTP middleware
  -> handler       协议解析、校验、错误映射
  -> service       短码规则、过期判断、事务意图
  -> repository    MySQL / Redis / clock / ID source

read:  Redis hit -> redirect
       Redis miss -> MySQL -> populate cache -> redirect

write: validate -> generate code -> MySQL unique constraint
      -> return created resource
```

数据库是 source of truth。Redis 不可用时允许受限回源，但必须限制并发，防止缓存故障演变为数据库雪崩。短码碰撞由数据库唯一约束最终兜底，不能只相信随机算法“概率足够低”。

## 目录和依赖方向

```text
cmd/api/main.go              组装依赖、启动与关闭
internal/httpapi/            handler、middleware、HTTP error
internal/link/               领域模型、service、接口
internal/storage/mysql/      MySQL repository
internal/cache/redis/        cache adapter
internal/config/             配置解析与校验
migrations/                  可重复数据库迁移
tests/integration/           真实依赖集成测试
```

依赖从外向内：handler 依赖 service interface，service 依赖自己定义的 repository/cache/clock 接口，基础设施层实现这些接口。不要为了“分层”给每个函数创建一层；边界的价值在于隔离协议、业务和外部失败。

```go
type LinkRepository interface {
    Create(ctx context.Context, link Link) error
    FindByCode(ctx context.Context, code string) (Link, error)
}

type Service struct {
    repo  LinkRepository
    cache LinkCache
    clock Clock
}
```

接口优先定义在使用方，并保持小而明确。测试 service 时用 fake repository 保存真实状态，比为每个方法设置脆弱 mock 更容易验证业务行为。

# 契约、数据与不变量

请求与响应必须先写出来，再写 handler：

```http
POST /v1/links
Content-Type: application/json

{"url":"https://example.com/docs","expires_at":"2026-10-01T00:00:00Z"}
```

```json
{"code":"aZ3k9Q","short_url":"https://s.example/aZ3k9Q","expires_at":"2026-10-01T00:00:00Z"}
```

稳定错误格式：

```json
{"code":"invalid_url","message":"url must use http or https","request_id":"..."}
```

第一版 schema：

```sql
CREATE TABLE link (
    id          BIGINT PRIMARY KEY AUTO_INCREMENT,
    code        VARCHAR(16) NOT NULL,
    target_url  TEXT NOT NULL,
    expires_at  DATETIME(6) NULL,
    created_at  DATETIME(6) NOT NULL,
    UNIQUE KEY uk_link_code (code)
);
```

需要始终成立的不变量：`code` 唯一；只允许 `http/https`；过期链接不再重定向；创建成功的记录即使缓存写入失败也能从数据库读取；重复碰撞只在有限次数内重试。

`DATETIME(6)` 不携带时区：写入前把过期时间规范化为 UTC，读取后按同一约定解释；客户端仍用带时区的 RFC 3339 时间表示。否则服务与数据库会话时区不同就可能误判过期。

明确 HTTP 语义：格式错误或非法 URL 返回 `400`，不存在返回 `404`，已过期可以返回 `410`，内部依赖失败返回稳定的 `5xx` 错误码。不要把数据库错误字符串直接返回客户端。

# 生命周期与失败边界

配置来自环境变量或配置文件，但解析后进入强类型结构，并在启动时验证：监听地址、数据库 DSN、Redis 地址、请求 timeout、关闭期限和日志级别。日志不得打印密码或完整 DSN。

启动顺序：

```text
load and validate config
-> initialize logger
-> open DB and Redis clients
-> verify required dependency connectivity
-> build repositories and service
-> start HTTP server
-> mark readiness
```

数据库迁移由独立命令或明确的部署阶段执行，不能让多个实例无协调地修改 schema。启动失败要返回非零状态并留下可行动日志。

收到 `SIGTERM` 后先让 readiness 失败、停止接收新流量，再用有期限的 context 调用 `Server.Shutdown`，最后停止后台任务并关闭依赖。超过期限才强制退出。关闭测试应包含一个正在处理的慢请求，证明它能完成或被明确取消。

## 超时、错误与缓存边界

HTTP 请求的 context 贯穿 service、MySQL 和 Redis。每个依赖可以拥有更短子 deadline，为错误转换和响应预留时间。客户端断开后，未完成的数据库查询和回源任务应尽快停止。

缓存读取流程：

1. 查询 Redis；命中后解码并验证 schema/version；
2. miss 时查数据库；不存在与 Redis 故障必须区分；
3. 数据库返回有效值后，以有限 TTL 写缓存；
4. 缓存写失败不改变本次正确结果，但记录分类指标；
5. 热点 miss 使用进程内 singleflight，并限制全局回源并发。

不要缓存永久重定向 `301`，除非已经确定目标不可变；浏览器和中间代理可能长期保存结果。第一版使用 `302` 更容易修正错误链接。

# 验证与运行证据

| 层级 | 真实对象 | 必测行为 |
| --- | --- | --- |
| Service unit | service + fake repo/clock | URL 校验、碰撞重试、过期判断 |
| Handler | mux + service fake | JSON、status、header、body limit、取消 |
| Repository integration | 真实 MySQL | 唯一约束、NULL/时间、错误转换 |
| Cache integration | 真实 Redis | hit/miss、TTL、解码失败、不可用 |
| End-to-end | 完整服务 | 创建后重定向、重启后数据仍在 |
| Shutdown | 慢请求 + signal | 停止接流量、在期限内排空 |

运行基线：

```bash
go test ./...
go test -race ./...
go vet ./...
go test -run Integration -count=1 ./tests/integration/...
```

集成测试使用独立数据库/schema 和唯一 key 前缀，测试结束可靠清理。不要让用例依赖执行顺序，也不要通过长时间 `Sleep` 猜异步工作是否完成。

故障实验至少覆盖：Redis 断开时受限回源；数据库 timeout；创建提交成功但客户端断开；短码连续碰撞；关闭期间存在在途请求。

## 观测与性能证据

每个请求日志包含 service、version、request_id/trace_id、route、status、duration 和稳定错误类别，不记录 token、完整 URL 查询参数或请求体。

最小指标：

- HTTP 请求数、错误分类和 latency histogram；
- MySQL 查询延迟、连接池使用中/空闲/等待；
- Redis hit、miss、timeout、回源次数；
- goroutine、heap、GC 与进程资源；
- shutdown 开始、在途请求和超时退出。

压测前固定 revision、构建方式、机器、数据量、连接数、预热和持续时间。分别测缓存热读、冷读和创建写入，不把三个路径混成一个吞吐数字。报告 P50/P95/P99、错误率、CPU、内存和依赖状态，同时确认压测机不是瓶颈。

# 交付里程碑与资料

| 阶段 | 可见产物 | 通过条件 |
| --- | --- | --- |
| 1 | API 契约、schema、不变量 | 错误语义和范围写清 |
| 2 | 内存版 service | 无 HTTP 即可验证规则 |
| 3 | handler 与 middleware | `httptest` 覆盖协议边界 |
| 4 | MySQL repository/migration | 真实约束和事务测试通过 |
| 5 | Redis 与回源保护 | hit/miss/故障路径可观察 |
| 6 | 关闭与压测 | 有相同条件的原始证据 |
| 7 | README 与演示脚本 | 干净环境可复现 |

最终证据包包含：架构与读写路径图、API 示例、迁移文件、测试报告、一次故障注入记录、压测命令和结果、日志到 trace 的关联样本、已知限制与下一步。没有实际完成的多地域、高可用或千万 QPS 不写进“已实现能力”。

## 关联专题与官方资料

- [URL Shortener (短链接服务)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/01-URL%20Shortener%20(短链接服务).md)：后续项目迭代与验收记录。
- [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)：测试层次和证据要求。
- [Go database/sql tutorial](https://go.dev/doc/tutorial/database-access)
- [net/http package](https://pkg.go.dev/net/http)
- [Go Diagnostics](https://go.dev/doc/diagnostics)
