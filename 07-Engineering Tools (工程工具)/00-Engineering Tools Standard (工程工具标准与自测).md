---
status: stable
confidence: high
verified: 2026-09-17
tags: [learning/standard, engineering/tools, git]
---

> [!abstract] 完成标准
> 工具能力以“能安全完成工程任务并留下可审查证据”为准。命令只是入口，真正要掌握的是状态模型、失败恢复和团队协作边界。

# Git 协作

- [ ] 能解释工作区、暂存区、本地提交、分支和远端引用之间的关系。
- [ ] 能独立完成 clone、branch、add、commit、fetch、pull、push 和 tag，并在执行前确认目标分支与远端。
- [ ] 能选择 merge、rebase、cherry-pick 或 revert，解决冲突且不破坏公共历史。
- [ ] 能使用 status、diff、log、show、blame 和 bisect 查明“改了什么、谁改的、从哪次开始坏”。
- [ ] 能配置 `.gitignore`，避免提交密钥、构建产物、本地配置和编辑器垃圾；知道已跟踪文件不会因 ignore 自动消失。
- [ ] 能从 reflog、误提交或错误合并中恢复；涉及 reset、force push 前能说明影响范围与回退方案。

# 构建与依赖

- [ ] 能区分编译、链接、运行时加载和包管理问题，并读懂对应错误。
- [ ] 能用 CMake 或语言原生工具组织开发、测试、发布构建，固定必要的工具链与依赖版本。
- [ ] 能处理 include/library 搜索路径、静态/动态链接、ABI 与传递依赖。
- [ ] 能从干净环境复现构建，不依赖只存在于个人机器的隐式状态。

# 调试与性能

- [ ] 能使用断点、调用栈、变量观察、core dump 和 sanitizer 定位正确性问题。
- [ ] 能先建立可重复基线，再用 profiler、系统指标和火焰图定位性能瓶颈。
- [ ] 能区分 wall time、CPU time、吞吐、延迟分位数、内存与 I/O 指标。
- [ ] 能记录环境、输入、命令和结果，避免不可复现的性能结论。

# 自动化与交付

- [ ] 能写安全的 Shell/PowerShell/Python 自动化：参数校验、dry-run、日志、退出码和失败停止齐全。
- [ ] 能配置最小 CI 流程执行构建、测试和静态检查，并让失败原因可读。
- [ ] 能使用容器封装运行环境，理解镜像层、挂载、网络、健康检查和最小权限。
- [ ] 能整理一份别人可直接执行的 README，而不是只记录自己的操作过程。

# 云原生交付

- [ ] 能构建最小、不可变、非 root 的容器镜像，理解镜像层、运行时隔离与资源限制。
- [ ] 能解释 Pod、Deployment、Service、Controller 和调度器的职责，并沿请求路径排查故障。
- [ ] 能正确设计 ConfigMap、Secret、startup/readiness/liveness probes 和优雅终止。
- [ ] 能执行滚动发布、观测失败指标和回滚，并说明代码回滚无法自动撤销的外部状态。
# 综合自测

1. `git pull` 实际包含哪些动作，为什么有时应先 fetch 再决定 merge 或 rebase？
2. 一个已提交的密钥加入 `.gitignore` 后为什么仍在历史中，应如何处置？
3. “本机能编译、CI 不能编译”时，你会怎样比较环境和依赖图？
4. 一个程序变慢时，为什么先测量再优化，基线至少应记录哪些条件？
