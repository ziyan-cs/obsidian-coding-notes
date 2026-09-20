---
study_stage: backlog
tags: [career/portfolio, project/evidence]
---

> [!note] 方法论坐标
> 项目证据包分别按 [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)、[Performance Benchmarking and Profiling (性能基准与剖析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/04-Performance%20Benchmarking%20and%20Profiling%20(性能基准与剖析).md) 与 [Release Verification and Quality Gates (发布验证与质量门禁)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/07-Release%20Verification%20and%20Quality%20Gates%20(发布验证与质量门禁).md) 的标准组织；本篇负责把它们收敛为可演示、可复核的交付物。

> [!abstract] 学习定位
> 简历写成果和证据，不写“使用了若干技术”；面试中所有数字都应能还原条件。

> [!summary] 核心摘要
>
> 每个结果都同时记录场景、数据规模、机器条件、命令和原始输出；区分测量值、推断与结论，不能复现的数字不进入简历。

# 每个项目至少保留

- [ ] README：问题、启动步骤、架构图、限制。
- [ ] 1 个关键设计决策及被放弃方案。
- [ ] 1 组测试证据：正常、边界、故障。
- [ ] 1 份压测或运行数据：环境、负载、指标、结论。
- [ ] 1 个真实 bug / 故障复盘。

# 项目表达模板

`为 <场景> 实现 <系统>；通过 <关键机制> 保证 <性质>；在 <环境/负载> 下测得 <指标>；已知限制是 <限制>。`

不要使用没有证据的“高并发”“百万 QPS”“分布式架构”等措辞。

压测报告至少分开记录**施加的负载**与**服务实际完成的工作**：客户端请求速率、成功率、超时/拒绝、P50/P95/P99、资源使用和下游瓶颈。固定并发客户端若服务变慢，其实际到达率也可能下降；仅看平均延迟或峰值 QPS 容易误判。测试前说明预热、持续时间、数据集、网络位置和是否包含 TLS、日志与依赖服务。

一次可复现记录可写为：`目标路径 + 环境/版本 + 数据规模 + 负载脚本与参数 + 原始输出 + 指标截图或导出 + 结论及局限`。先保存原始证据，再写“因为缓存命中提高而变快”等解释；若没有命中率和剖析数据，最多只能说观察到延迟变化，不能断言原因。

# 审核前的证据包

把每一个简历项目整理为可复查的最小证据包：

- 一张架构图：请求路径、状态存储、异步边界和故障点。
- 一个可运行命令：最小配置、启动与测试入口。
- 一份实验记录：环境、负载、baseline、指标、结论与限制。
- 一条故障复盘：现象、定位证据、修复、如何防止回归。

> [!tip] Obsidian 使用方式
> 在每个项目笔记底部链接证据文件或实验记录；面试前只复习“设计决策、关键数据、故障复盘”三项，而不是背完整 README。

> [!question]- 自测：先回答再展开
> - 你能在两分钟内从仓库启动项目，并说明一个已知限制吗？
> - 你能展示某个性能数字的环境、负载和原始证据吗？

> [!info]- 延伸阅读
> - 下一步：[01-Project Design Template (项目设计模板)](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/01-Project%20Design%20Template%20(项目设计模板).md)
