---
study_stage: backlog
---

> [!abstract] 学习定位
> 缓存是可丢弃的派生数据；设计必须先确定真相来源，再说明读写路径、失效窗口和回源保护。

> [!summary] 核心摘要
>
> cache-aside 以数据库为真相来源；miss、热点失效、删除失败和 Redis 不可用都要有明确策略，缓存不能悄悄改变业务正确性。

# Cache-Aside 读写路径

```text
read:  service -> Redis hit -> return
                  |
                  miss
                  v
              database -> encode + TTL -> Redis -> return

write: validate -> database commit -> invalidate cache -> response
```

读路径中，cache miss 不等于业务不存在。回源后再判断是否缓存空结果。写路径“先提交数据库再删除缓存”仍存在短暂旧值窗口，但不会让缓存成为提交前的真相；删除失败要通过重试、事件补偿或有限 TTL 收敛。

缓存对象应带 schema/version 语义。发布改变字段含义时，可以变更 key 版本或让解码器兼容旧值，避免新代码读取无法解释的旧缓存。

# 三类集中风险

| 风险 | 形成方式 | 常见保护 |
| --- | --- | --- |
| 穿透 | 大量不存在 key 每次回源 | 参数校验、短 TTL 空值、Bloom filter |
| 击穿 | 单个热点 key 同时过期 | singleflight/互斥回源、逻辑过期 |
| 雪崩 | 大量 key 同时过期或 Redis 故障 | TTL 抖动、限流、降级、容量与故障演练 |

标准 Bloom filter 在**所有当前有效 key 都已插入、没有错误删除且过滤器状态未丢失**的前提下，不产生 false negative，但允许 false positive。若数据库新增 key 没同步写入过滤器，仍可能误判不存在；它只能作回源前的辅助判断，不能成为唯一真相。缓存空值会占空间并改变“未找到”的表示，TTL 应较短且要防止恶意高基数 key。

singleflight 只合并同一进程内同 key 的并发回源；多实例仍可能同时访问数据库。分布式锁也不是免费解法，要设计租约、持有者崩溃与超时后的行为。

# 超时、连接与降级

Redis 调用要设置比请求总预算更短的 timeout，并限制连接池。池等待升高可能意味着 Redis 变慢、并发过高或连接泄漏。Redis 故障时的选择取决于数据语义：

- 非关键展示数据可绕过缓存回源，但必须限制回源并发，防止拖垮数据库。
- 可接受旧值的场景可服务 stale 数据，并明确最长陈旧时间。
- 库存、余额、权限等正确性敏感数据不能把缓存值当最终依据。
- 熔断与降级要有恢复探测，不能永久停留在故障模式。

# 一致性窗口与观测

TTL、容量、timeout 和热点阈值都由新鲜度目标与测量决定，不是固定常数。至少监控 hit ratio、miss 回源量、命令延迟、timeout、池等待、key 数量、内存淘汰和热点 key。

> [!question]- 理解检查
> 数据库提交成功但缓存删除失败时，旧值窗口怎样产生？如果所有实例同时绕过 Redis 回源，数据库会发生什么？

> [!info]- 关联
> - 可靠补偿：[03-Messaging and Idempotency (消息与幂等)](/05-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/03-Messaging%20and%20Idempotency%20(消息与幂等).md)
