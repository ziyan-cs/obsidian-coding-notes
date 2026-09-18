---
status: learning
confidence: high
content_verified: 2026-09-17
tags: [cloud-native/kubernetes, backend/deployment]
---

> [!abstract] 学习目标
> Kubernetes 用声明式 API 和控制器把“期望状态”持续协调为“实际状态”。先理解 Pod、Controller、Service 和调度边界，再学习 YAML；不要把它当成一组需要背诵的 kubectl 命令。

# 集群与控制循环

```text
kubectl / CI
     |
     v
API Server ---- etcd
     |
     +---- Scheduler --------> choose Node
     |
     +---- Controllers ------> reconcile desired vs actual
                                  |
                    +-------------+-------------+
                    |                           |
                  Node A                      Node B
             kubelet + runtime          kubelet + runtime
                    |                           |
                   Pods                        Pods
```

API Server 是资源操作入口；etcd 保存集群状态；scheduler 为未调度 Pod 选择节点；controller 不断观察差异并采取动作；kubelet 负责让本节点 Pod 规格落地。一次 `kubectl apply` 成功只表示 API 接受对象，不代表应用已经 Ready。

Pod 是最小部署单元，内部容器共享网络命名空间和声明的卷，适合紧密耦合的 sidecar/init container，不是把任意服务塞在一起的“小虚拟机”。Pod 可被替换，应用不能依赖它的名称、IP 或本地临时文件永久存在。

# Workload Controller（工作负载控制器）

| 资源 | 适用场景 | 关键语义 |
| --- | --- | --- |
| Deployment | 无状态、可互换副本 | 管理 ReplicaSet，支持滚动更新 |
| StatefulSet | 稳定身份或持久卷的有状态副本 | 有序身份与卷绑定，不自动保证数据库一致性 |
| DaemonSet | 每节点一个 agent | 日志、监控、网络或存储组件 |
| Job / CronJob | 有结束条件的批任务 | 重试、并行度、截止时间和幂等 |

Controller 只能根据探针和对象状态协调，不理解业务事务。副本数增加也不会自动解决共享状态、热点、下游容量或幂等问题。

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: api
spec:
  replicas: 3
  selector:
    matchLabels: { app: api }
  template:
    metadata:
      labels: { app: api }
    spec:
      containers:
        - name: api
          image: registry.example/api@sha256:...
          ports:
            - containerPort: 8080
          resources:
            requests: { cpu: 100m, memory: 128Mi }
            limits: { memory: 256Mi }
```

镜像使用不可变 digest 可以避免同一 tag 指向不同内容。`requests` 参与调度与资源保障；`limits` 的 CPU/内存行为不同，设置前要理解 throttling、OOM 与应用负载。

# Service 与网络路径

Pod IP 会变化。Service 用 selector 选择一组后端 Pod，并提供稳定虚拟地址与服务发现；Ingress 或 Gateway API 处理进入集群的 HTTP/TLS 流量。它们不自动提供业务认证、授权和幂等。

排查请求路径时逐层确认：

1. DNS 是否解析到预期 Service。
2. Service selector 是否匹配 Pod label，EndpointSlice 是否有 Ready endpoint。
3. Pod 是否监听正确地址与端口，而不是只监听 `127.0.0.1`。
4. NetworkPolicy、CNI、节点网络和入口控制器是否允许流量。
5. 应用是否因下游超时、连接池或资源限制而慢。

# 调度、容量与故障

- Pod 重启不等于节点故障迁移；Controller 会根据实际状态创建替代副本。
- anti-affinity 或 topology spread 用于避免副本集中在同一故障域，但会增加调度约束。
- requests 过低会导致节点过度承诺，过高则可能 Pending；容量规划应基于真实指标。
- 临时磁盘、PID、连接数和下游配额同样可能成为资源边界。
- 优雅终止需要应用处理 SIGTERM、停止接流量、排空请求，并与 `terminationGracePeriodSeconds` 配合。

> [!summary] 核心摘要
> 从一次 Deployment 更新开始，说明 API Server、controller、scheduler、kubelet、Pod 和 Service 分别做什么；再解释一个 Pod Running 但请求仍失败时应检查哪些层。

官方参考：[Kubernetes Cluster Architecture](https://kubernetes.io/docs/concepts/architecture/) · [Pods](https://kubernetes.io/docs/concepts/workloads/pods/) · [Deployments](https://kubernetes.io/docs/concepts/workloads/controllers/deployment/)
