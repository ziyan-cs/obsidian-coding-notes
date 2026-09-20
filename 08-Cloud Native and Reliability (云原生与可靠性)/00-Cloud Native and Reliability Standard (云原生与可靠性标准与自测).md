---
study_stage: backlog
tags: [learning/standard, engineering/cloud-native, sre]
---

> [!abstract] 完成标准
> 能把已通过工程验证的服务部署到受控平台，在变化、过载和故障中保持可观测、可恢复。重点是运行机制与可靠性决策，不以会背 Kubernetes 对象为完成。

# 容器与 Kubernetes

- [ ] 能构建非 root、可追溯、最小权限的镜像，理解容器共享内核的隔离边界。
- [ ] 能解释 API Server、etcd、controller、scheduler、kubelet 的控制链，并说明 apply 成功为何不等于服务就绪。
- [ ] 能依据 requests、affinity、taint、拓扑、PVC/PV/CSI 解释调度与挂载结果。
- [ ] 能设置 requests/limits、startup/readiness/liveness、优雅终止与滚动发布。
- [ ] 能从事件、日志、指标、trace、容器状态和网络路径排查 Pending、未就绪、重启、OOM 与超时。
- [ ] 能设计 RBAC、ServiceAccount、NetworkPolicy、Secret 和 Pod 安全边界，并验证策略实际生效。

# 平台工程

- [ ] 能使用 IaC 描述资源，以 plan、评审、状态锁和漂移检测控制变更。
- [ ] 能解释 GitOps 协调、配置与秘密边界、多环境差异和策略即代码。
- [ ] 能设计一条服务黄金路径，同时保留可观察证据与受控逃生口。
- [ ] 能用交付时间、失败率和开发者反馈衡量平台，而不是组件数量。

# 可靠性工程

- [ ] 能定义用户可感知的 SLI/SLO，使用错误预算和燃烧率告警。
- [ ] 能做容量估算、过载保护、自动扩缩和单位请求成本分析。
- [ ] 能组织事故时间线、止损、恢复、复盘和可验证行动项。
- [ ] 能说明备份、复制、故障转移和恢复演练分别解决什么问题。

# 综合自测

1. 一个 Pod 长期 Pending 时，如何区分资源、taint/affinity、拓扑和 PVC 绑定问题？
2. 新版本 readiness 失败但 liveness 正常时，请沿配置、依赖、网络和应用状态排查。
3. GitOps 仓库已更新而集群未变化，怎样区分控制器、权限、策略和目标环境问题？
4. P99 升高而 CPU 不高时，怎样检查队列、连接池、下游与锁等待？
5. 一次发布造成跨区故障时，如何止损、保存证据并证明服务已经恢复？

