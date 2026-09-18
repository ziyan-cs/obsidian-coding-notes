---
status: stable
confidence: high
content_verified: 2026-09-18
verified: 2026-10-11
review_stage: learn
review_due: 2026-10-11
tags: [engineering/verification]
---

> [!abstract] 学习定位
> 测试不是“把函数跑一遍”，而是用可重复的实验约束系统行为。本篇统一测试分层、用例设计、替身边界、并发与故障测试以及证据保存；语言框架的具体语法留在各语言专题。

# 从风险反推测试

先问“什么失败最贵”，再决定测什么。典型风险包括：计算错误、状态损坏、权限绕过、重复扣款、消息丢失、资源泄漏、超时扩散和不兼容升级。每条高风险路径至少要有一个正常用例、一个边界用例和一个失败用例。

测试用例应写成可判定的契约：

```text
Given  已知状态与依赖
When   触发一个动作
Then   可观察结果与不变量
And    不允许发生的副作用
```

例如“创建订单失败”不能只断言 HTTP 500，还要验证库存未永久扣减、幂等键仍可重试、错误日志带 request_id，且指标只增加一次。

## 测试分层与职责

| 层级 | 验证对象 | 依赖范围 | 主要价值 | 常见误用 |
|---|---|---|---|---|
| Unit Test | 纯函数、领域规则、状态机 | 进程内 | 快速定位逻辑错误 | 把实现细节全部 mock 掉 |
| Component Test | 一个服务或组件 | 数据库/队列可用测试实例 | 验证序列化、SQL、事务和配置 | 仍用假对象代替关键基础设施 |
| Contract Test | 服务间请求与响应约定 | 协议边界 | 阻止不兼容变更 | 只检查字段存在，不检查语义 |
| Integration Test | 多个真实组件协作 | 容器或隔离环境 | 暴露网络、事务和时序问题 | 环境共享导致随机失败 |
| End-to-End Test | 关键用户旅程 | 近生产系统 | 验证整体可用性 | 数量过多、慢且难定位 |

健康的测试组合通常是底层多、顶层少，但不存在适用于所有项目的固定比例。支付、存储和并发中间件应把更多预算放在集成、恢复与一致性验证上。

# 用例设计：从示例到不变量

## 等价类、边界值与决策表

- 等价类（Equivalence Partitioning）：从行为相同的输入集合中选代表值。
- 边界值（Boundary Value）：检查 `min-1`、`min`、`max`、`max+1`，以及空、零、极大值。
- 决策表（Decision Table）：权限、状态和配置组合较多时，列出条件与动作，防止遗漏组合。
- 状态迁移（State Transition）：验证允许的边和禁止的边，例如订单不能从 `PAID` 回到 `CREATED`。

## Property-based 与 Fuzz Testing

示例测试证明“这些输入正确”；性质测试检查“一类输入始终满足不变量”。适合序列化、解析器、数据结构和状态机：

- `decode(encode(x)) == x`；
- 排序结果有序，且元素多重集合不变；
- 任意字节输入不得导致解析器崩溃或越界；
- 重复提交同一幂等键，持久化副作用至多一次。

失败输入必须保留为 regression corpus，避免修复后再次出现。

## 测试替身与边界

| 类型 | 含义 | 合适场景 |
|---|---|---|
| Stub | 返回预设值 | 控制罕见错误分支 |
| Fake | 简化但可工作的实现 | 内存仓库、假时钟 |
| Mock | 验证交互是否发生 | 外发通知、计费调用 |
| Spy | 记录真实调用 | 观察参数或次数 |

优先 mock 自己拥有的窄接口，不要 mock 数据库驱动、HTTP 客户端的深层内部或第三方 SDK 的每个方法。关键边界应至少有一组真实协议/真实实例测试，否则 mock 与现实会同时漂移。

时间、随机数、UUID 和外部 I/O 应通过依赖注入变得可控；不要用真实 `sleep` 等待异步完成，应该等待可观测条件并设置硬超时。

# 数据、并发与失败路径

测试必须可独立、可重复、可并行：每个用例拥有自己的数据库 schema、容器、临时目录或唯一命名空间；清理动作即使断言失败也要执行。

并发测试关注不变量而非线程调度细节：

- 无 data race；
- 没有重复消费、丢失更新和永久阻塞；
- 取消、超时和关闭后资源能回收；
- 重试不会放大非幂等副作用。

故障注入应覆盖超时、连接重置、部分响应、磁盘满、依赖限流、进程重启和消息重复。每次只改变一个主要变量，记录注入点、持续时间、预期降级和实际恢复证据。

# 证据与持续集成

一次可复核的测试运行至少记录：

```text
revision / build_id
toolchain and dependency versions
environment and configuration
seed / input corpus / fixture version
command and exit code
failed assertion with expected vs actual
logs, metrics, traces or artifacts
```

Flaky test 不是“多跑几次就好”。先判断根因属于共享状态、时间假设、无界异步、资源竞争还是环境不一致；隔离只能临时止血，并应有负责人和恢复条件。

## CI 中的执行顺序

```text
format/lint -> compile/type check -> unit -> component
            -> integration/contract -> security scan
            -> package -> smoke/canary verification
```

快速、确定的检查先运行，昂贵检查后运行。门禁应基于风险：核心不变量失败必须阻断；覆盖率变化用于发现盲区，但单一覆盖率数字不能证明质量。

# 实践与资料

- [ ] 为一个 CRUD 服务写出风险清单，并将每项风险映射到测试层级。
- [ ] 为解析器加入 property/fuzz 测试，保存最小失败样本。
- [ ] 用真实数据库验证事务回滚、唯一约束和并发更新。
- [ ] 构造依赖超时与连接中断，证明服务能取消请求并释放资源。
- [ ] 让 CI 产出机器可读报告、失败日志和可下载构建产物。

## 关联专题

- [Release Verification and Quality Gates (发布验证与质量门禁)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/07-Release%20Verification%20and%20Quality%20Gates%20(发布验证与质量门禁).md)：把测试结果转化为合并与发布决策。
- [Testing Benchmark and Evidence (测试压测与证据)](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/02-Testing%20Benchmark%20and%20Evidence%20(测试压测与证据).md)：项目交付时如何组织证据包。

## 参考资料

- [Google Testing Blog](https://testing.googleblog.com/)
- [OWASP Web Security Testing Guide](https://owasp.org/www-project-web-security-testing-guide/)
- [Go Fuzzing](https://go.dev/doc/security/fuzz/)
- [Python unittest documentation](https://docs.python.org/3/library/unittest.html)
