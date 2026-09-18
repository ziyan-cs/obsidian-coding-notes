---
status: learning
confidence: low
content_verified: 2026-09-17
tags: [project/url-shortener, backend/api]
---

> [!abstract] 学习定位：目标：用户创建短链接，访问短码后重定向；重点是读写路径、冲突、缓存与统计边界，不是生成一个随机字符串。

> [!summary] 核心摘要
>
> 短链接服务把短码唯一映射到原 URL，写路径处理发号、冲突与持久化，读路径用缓存加速重定向。还要明确过期、恶意链接、热点 key 和访问统计的一致性边界。

# 最小范围

- `POST /v1/links`：校验 URL，创建短码。
- `GET /{code}`：查询并 302 redirect。
- `GET /v1/links/{code}`：读取元数据。
- 不做登录、多地域、复杂分析；这些作为后续扩展。

# 关键问题

| 问题 | 第一版选择 | 需要验证 |
| --- | --- | --- |
| 短码冲突 | 数据库唯一约束 + 重试 | 冲突率与最大重试次数 |
| 热点读 | Redis cache-aside | 缓存未命中与失效行为 |
| 无效链接 | 显式 404 / 410 语义 | 不泄漏内部信息 |
| 统计 | 异步事件（后续） | 至少一次投递的去重 |

# 数据模型与短码策略

```sql
CREATE TABLE link (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(16) NOT NULL,
    target_url TEXT NOT NULL,
    owner_id BIGINT NULL,
    expires_at DATETIME(6) NULL,
    created_at DATETIME(6) NOT NULL,
    UNIQUE KEY uk_code (code)
);
```

短码策略可以从两种方案起步：随机字符 + 唯一约束，简单且不暴露递增 ID，但要限制碰撞重试；ID 编码生成稳定且无碰撞，但会暴露规模和可枚举性，需要混淆或权限保护。第一版应实现一种并记录选择依据，不要把两者同时堆进代码。

URL 校验至少限制 scheme、长度和控制字符。若系统允许服务端抓取目标页面，还必须防 SSRF；若只是重定向，也要考虑钓鱼、恶意域名和滥用封禁。原始 URL 可能含 token 或个人信息，日志与分析事件不应完整记录。

# 读写路径与失败窗口

创建路径先校验，再生成短码并插入数据库；唯一冲突时重新生成，达到上限后返回可重试内部错误。缓存写入不是创建成功的必要条件，数据库提交成功后即可以返回结果。

```text
GET /{code}
  -> Redis GET
     -> hit: validate cached expiry -> 302
     -> miss: bounded DB lookup
              -> not found / expired -> 404 or 410
              -> found -> cache with TTL -> 302
```

缓存 TTL 不得超过业务过期时间。不存在结果可以短暂负缓存，但要防止攻击者用随机 code 制造高基数 key。热点失效时使用 singleflight、回源并发上限或逻辑过期，不能让所有实例同时压向数据库。

访问统计与重定向主路径解耦：先保证重定向成功，再把访问事件写入有界缓冲或消息系统。统计允许一定延迟和少量重复时，消费者按 event ID 去重；不能因为统计后端故障阻塞所有跳转。

# 分阶段实现与验证

1. 内存 repository：完成 API、校验、过期语义和 handler 测试。
2. MySQL：迁移、唯一约束、重启后数据保持、碰撞重试。
3. Redis：hit/miss、TTL、负缓存和不可用降级。
4. 运行能力：request ID、RED 指标、健康检查、优雅关闭。
5. 压测：分别测热读、冷读和创建，记录依赖负载与错误率。

故障实验包括 Redis 断开、数据库慢查询、缓存热点同时失效、客户端在提交后断开，以及短码连续碰撞。每次实验保留命令、日志、指标和恢复结果。

最小指标：创建成功/冲突/失败，重定向状态，cache hit/miss，回源并发，数据库池等待，P50/P95/P99。容量估算从链接新增量、平均 URL 长度、读写比、过期比例和热点分布开始，不凭空写“亿级”。

实现教程入口：[First Service Delivery (首个服务交付)](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/03-First%20Service%20Delivery%20(首个服务交付).md)。

# 验收

- [ ] Go 标准库 HTTP API + `httptest`。
- [ ] MySQL 唯一约束、Redis 缓存、timeout 和结构化日志。
- [ ] Python checker 生成链接、验证 redirect、汇总延迟。
- [ ] 一张读路径图、一份压测报告、一条缓存失效复盘。

> [!summary]- 项目表达检查：学完后再展开
>
> 我用短链接服务练习“写入正确性和读路径性能”的分离：短码由唯一约束兜底，读请求走 cache-aside，缓存失效时仍以数据库为 source of truth。第一版只证明重定向、冲突处理、超时和观测，不把多地域或分析系统伪装成已实现能力。
>

> [!warning] 常见误区
> hash 截断不等于永不冲突；302/301 的选择、负缓存和热点保护都需要以真实需求与测试结果决定。

> [!info]- 延伸阅读
> - 下一步：[02-IM and Notification (即时通讯与通知)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/02-IM%20and%20Notification%20(即时通讯与通知).md)

