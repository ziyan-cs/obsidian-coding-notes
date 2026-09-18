---
status: learning
confidence: high
content_verified: 2026-09-18
tags: [python/testing, testing/property, testing/integration]
---

> [!note] 方法论坐标
> 性质、模糊、集成与故障测试的共同设计原则见 [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)；本篇保留 Hypothesis 与 Python 环境实践。

> [!abstract] 学习定位
> 示例测试验证已想到的案例，性质与 fuzz 测试探索输入空间，集成测试验证真实边界；三者解决不同风险，不能互相替代。

# 从风险选择测试层次

| 层次 | 主要回答 | 速度与稳定性 | 适合失败 |
| --- | --- | --- | --- |
| 单元测试 | 规则对给定输入是否正确 | 快、确定 | 分支、边界、算法错误 |
| 性质测试 | 大量输入是否保持不变量 | 较快，可复现 seed | 未想到的组合与极值 |
| 集成测试 | adapter 与真实协议是否兼容 | 较慢，需要环境 | SQL、编码、序列化、配置差异 |
| 端到端测试 | 用户路径是否整体可用 | 最慢、定位较粗 | 部署、路由、权限与组合故障 |

测试组合由风险决定。解析器和状态机适合 fuzz；复杂业务规则适合性质测试；数据库迁移和 HTTP 客户端必须有少量真实集成测试。

# 性质与不变量

性质不是把实现再写一遍，而是描述所有合法输入都应满足的关系：序列化再反序列化保持值、排序结果单调且元素多重集合不变、归一化操作幂等。

~~~python
def test_normalize_is_idempotent(samples: list[str]) -> None:
    once = [normalize(value) for value in samples]
    twice = [normalize(value) for value in once]
    assert twice == once
~~~

Property-based 工具会生成输入并在失败后 shrink 到较小反例。保留失败 seed 或最小样本作为回归测试；生成器必须遵守业务输入约束，否则大量无意义非法数据会掩盖真正问题。

# Fuzzing 不等于随机乱测

Fuzz target 应快速、确定、无外部副作用，并具有可判断的 oracle，例如“不崩溃、不越界、输出可再次解析、结果满足不变量”。适合文件格式、协议帧、路径处理和数据转换边界。

一次发现的失败输入进入固定 corpus，修复后长期回归。记录工具版本、持续时间和 seed；无法复现的随机失败几乎没有工程价值。

# 集成测试与环境

集成测试尽量使用真实协议和接近生产的版本，同时控制数据和生命周期。每个测试创建独立 schema、临时目录或唯一资源名，结束后清理；测试失败时保留必要日志，不共享依赖执行顺序的全局状态。

外部 SaaS 不适合作为每次单元测试的依赖。可用本地 fake 验证大部分行为，再用少量契约测试确认请求格式、认证和错误映射与真实服务兼容。

# 故障注入

主动测试 timeout、连接重置、半截响应、磁盘满、权限拒绝、重复消息和进程取消。故障注入点应位于 adapter，使领域逻辑无需真实破坏机器环境。

断言不仅检查“抛异常”，还检查：资源是否释放、临时文件是否残留、事务是否回滚、重试是否越过预算、日志是否包含 request ID 且不泄漏秘密。

# 实践任务

为 JSON 批处理工具建立测试矩阵：

1. 单元测试字段校验与聚合规则。
2. 性质测试“写出后读回等价”和“重复执行结果不变”。
3. fuzz 非法 UTF-8、深层嵌套、超长字段与截断输入。
4. 集成测试真实文件权限、原子替换和 CLI 退出码。
5. 保存一个历史失败输入，证明修复不会回归。

> [!question]- 理解检查
> 1. 为什么 100% 行覆盖率仍可能漏掉严重输入组合？
> 2. fuzz target 为什么要快速、确定并具有 oracle？
> 3. fake 通过后，为什么仍需要少量真实协议集成测试？

> [!info]- 官方参考
> - [Python unittest: Organizing test code](https://docs.python.org/3/library/unittest.html#organizing-test-code)
> - [pytest Good Integration Practices](https://docs.pytest.org/en/stable/explanation/goodpractices.html)
