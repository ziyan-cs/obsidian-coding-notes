---
status: stable
confidence: medium
verified: 2026-09-17
---

> [!abstract] 一句话结论：缓存是派生数据，不是新的真相来源；设计先明确回源、失效、击穿保护和一致性边界。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：先确定数据库是真相来源，再选择 cache-aside 等读写路径；为 miss、热点失效和回源失败设置明确策略，接受并说明短暂不一致。

# Cache-Aside 基线

1. 读：缓存命中直接返回；未命中从数据库读取并填充缓存。
2. 写：先更新数据库，再删除或更新缓存；选择必须与并发和失败处理匹配。
3. 失效：设置合理 TTL，并为热点、穿透、雪崩配置专门保护。

# 一条读路径

```text
client -> service -> Redis hit? -> return cached value
                       |
                       no
                       v
                    database -> cache with TTL -> return value
```

缓存 miss 不等于业务不存在。先回源确认，再决定是否缓存空结果；对不存在的高频 key 可以使用短 TTL 的空值或 Bloom filter，但 Bloom filter 允许 false positive，不能把它当作“数据一定存在”的证明。

# 设计参数不是常数

TTL、最大容量、超时、热点保护的阈值都应由业务新鲜度和测量决定。商品详情可容忍短暂旧值，余额与库存通常不能直接套用同一策略。写路径出现“数据库已提交、删除缓存失败”时，要有重试、消息补偿或可接受的过期窗口，而不是假设它永远不会发生。

> [!question]- 自测：先回答再展开
> 数据库提交成功但缓存删除失败时，读到旧值的窗口怎样产生？你的业务能接受多久？

> [!info]- 延伸阅读
> - 下一步：[03-Messaging and Idempotency (消息与幂等)](/04-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/03-Messaging%20and%20Idempotency%20(消息与幂等).md)
