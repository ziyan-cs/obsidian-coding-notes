---
status: learning
confidence: medium
content_verified: 2026-09-18
tags: [cloud-native/kubernetes, kubernetes/scheduling, kubernetes/storage]
---

> [!abstract] 学习目标
> 从 API 对象进入调度、节点资源与存储控制循环，理解 Pod 为什么 Pending、为什么被驱逐、为什么挂载失败。重点不是背字段，而是根据事件和状态解释控制面决策。

> [!summary] 核心摘要
>
> scheduler 为未绑定 Pod 过滤并评分节点，kubelet 和运行时在目标节点落实规格；requests、节点条件、亲和性、污点与拓扑共同决定能否放置。持久化由 PV、PVC、StorageClass 与 CSI 协作，Pod 重建不等于数据自动正确恢复。

# 调度是一组约束求解

新 Pod 进入 API Server 后，scheduler 先过滤不满足硬约束的节点，再对候选节点评分并完成绑定。它根据声明和当前快照决策，不预测未来负载，也不了解业务层热点。

```text
Pending Pod
   -> filter: resources / node selector / affinity / taints / volume topology
   -> score: balance / affinity / topology preferences
   -> bind Pod to Node
   -> kubelet pulls image, mounts volume, starts containers
```

常见约束：

- `requests` 决定调度时预留的 CPU、内存等资源；实际使用量不能替代声明。
- node selector 与 required affinity 是硬条件；preferred affinity 只影响评分。
- taint 表示节点排斥条件，toleration 只表示 Pod 可以被考虑，不保证一定调度到该节点。
- topology spread 和 anti-affinity 可分散故障域，但约束过严会让 Pod 无处可放。
- Priority 与 preemption 可为高优先级 Pod 腾出位置，却不能创造资源，也可能放大低优先级服务抖动。

看到 `Pending` 时先读 Pod events 和 scheduler reason。`Insufficient cpu`、未满足 affinity、未绑定 PVC、不可容忍 taint 的处置完全不同，不能统一靠“加节点”解决。

# requests、limits 与驱逐

CPU 是可压缩资源：达到 CPU limit 时通常表现为 throttling；内存不可压缩，超过容器限制可能触发 OOM kill。节点整体资源紧张时，kubelet 还会根据 eviction signal 和 QoS 等因素驱逐 Pod。

| 配置 | 主要作用 | 常见风险 |
| --- | --- | --- |
| CPU request | 调度与份额基准 | 过低造成过度承诺，过高造成 Pending |
| CPU limit | 限制 CPU 使用 | 延迟型服务可能被不必要地 throttling |
| memory request | 调度与 QoS | 低估工作集会加剧节点压力 |
| memory limit | 容器内存上限 | 峰值、缓存或泄漏触发 OOM |
| ephemeral-storage | 临时存储预算 | 日志或临时文件填满节点磁盘 |

容量配置应来自负载测试和生产分位数，区分稳定工作集、启动峰值、突发余量与故障转移容量。HPA 根据指标调整副本数，但新副本仍受调度、镜像拉取、启动时间和下游容量限制；VPA、Cluster Autoscaler 或节点自动供给解决的是不同控制环。

# 持久化控制链

Pod 本地可写层随容器或 Pod 生命周期变化，不能承担数据库持久数据。Kubernetes 存储对象的职责如下：

```text
Pod -> PVC -> PV -> CSI driver -> storage backend
          ^
          +-- StorageClass controls dynamic provisioning
```

- PVC 表达工作负载对容量、访问模式和 StorageClass 的需求。
- PV 表示集群可用的存储资源及其回收策略。
- StorageClass 描述动态供给方式、参数、绑定模式等。
- CSI driver 将 Kubernetes 操作映射到底层云盘、块存储或文件系统。
- StatefulSet 提供稳定 Pod 身份和卷声明模板，不替数据库完成复制、选主、备份或一致性恢复。

访问模式是调度与挂载能力声明，不等于应用并发写入一定安全。卷快照也不必然具备应用一致性；数据库需要协调 flush、锁、日志或使用原生备份机制。

# 故障定位

按对象状态向下追踪，不要先删除 Pod 让证据消失：

1. `kubectl describe pod`：调度、拉镜像、挂载与探针事件。
2. `kubectl get pod -o wide`：节点、IP、重启次数和状态。
3. `kubectl describe node`：条件、可分配资源、taint 与压力。
4. `kubectl get pvc,pv,storageclass`：绑定、容量、回收策略与 provisioner。
5. CSI controller/node plugin 日志和云存储事件：供给、attach、mount 的具体失败。

`CrashLoopBackOff` 是重启退避结果，不是根因；`Pending` 也可能来自调度或卷绑定。记录首次失败时间、事件顺序与对象 generation，才能区分持续配置错误和瞬态基础设施故障。

# 实验与验收

在本地集群完成四个实验：设置无法满足的 request、添加未容忍 taint、创建错误 StorageClass 的 PVC、让容器超过内存 limit。每次先预测状态和事件，再观察 Pod、Node、PVC 与容器退出原因，最后修复并证明恢复。

# 参考资料

- [Kubernetes Scheduling, Preemption and Eviction](https://kubernetes.io/docs/concepts/scheduling-eviction/)
- [Kubernetes Resource Management for Pods and Containers](https://kubernetes.io/docs/concepts/configuration/manage-resources-containers/)
- [Kubernetes Storage](https://kubernetes.io/docs/concepts/storage/)

