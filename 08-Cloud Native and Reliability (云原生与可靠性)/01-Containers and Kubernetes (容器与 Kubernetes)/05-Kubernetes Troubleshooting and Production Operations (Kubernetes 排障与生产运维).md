---
status: learning
confidence: medium
content_verified: 2026-09-18
tags: [cloud-native/kubernetes, sre/troubleshooting, operations]
---

> [!abstract] 学习目标
> 建立从用户症状到 Kubernetes 对象、节点和依赖的排障证据链，并把发布、容量、备份和访问控制纳入生产运行。`kubectl` 命令只是取证工具，目标是形成可证伪的故障假设。

> [!summary] 核心摘要
>
> 生产排障先确定影响面和时间线，再沿入口、Service/EndpointSlice、Pod、节点与下游逐层验证。事件解释状态变化，日志解释组件行为，指标描述规模与趋势，trace 连接一次请求；重启只能改变现场，不能替代根因分析。

# 从症状构造排障路径

```text
user / synthetic check
   -> DNS and external load balancer
   -> Gateway / Ingress
   -> Service and EndpointSlice
   -> Pod readiness and application port
   -> node / CNI / kube-proxy or dataplane
   -> database, cache, queue and external API
```

先问：所有用户还是部分租户？单一区域还是全局？只影响新版本还是所有副本？错误率、延迟、吞吐从何时变化？然后用最少查询验证一个假设，不要在多个层同时改配置。

核心证据各有边界：

- object status 与 events：控制器观察到什么、何时尝试过什么；events 有保留期限，不能作为长期审计。
- application/container logs：解释进程行为；容器重建后要确认是否仍能取得 previous logs。
- metrics：回答影响规模、趋势和资源饱和；平均值可能掩盖尾延迟和单节点热点。
- traces：串联入口与下游阶段；采样策略可能漏掉低频错误。
- audit logs：谁对 API 做了什么；需要提前启用并保护敏感内容。

# 高频状态不是根因

| 表现 | 首要检查 | 不能直接推出 |
| --- | --- | --- |
| Pending | scheduler events、request、taint、affinity、PVC | 集群 CPU 一定不足 |
| CrashLoopBackOff | 当前/上次退出码、日志、配置、依赖 | Kubernetes 自身故障 |
| ImagePullBackOff | 镜像名、digest、registry、凭据、节点网络 | 镜像不存在 |
| Running but not Ready | readiness 输出、端口、依赖、初始化状态 | 进程已可服务 |
| OOMKilled | limit、工作集、峰值、泄漏、节点压力 | 只需调大内存 |
| 5xx / timeout | Endpoint、应用日志、连接池、下游与重试 | 入口控制器一定有问题 |

使用临时调试容器或专用 debug workload 时仍要遵守权限、镜像来源和审计要求。生产容器保持最小化，不应为了方便长期携带 shell、编译器和网络攻击工具。

# 发布与变更控制

安全发布要求版本可定位、状态可观察、影响可限制、失败可回滚：

1. 镜像绑定 digest，并保存源码、构建与 SBOM 证据。
2. 在发布前检查配额、策略、配置和数据库兼容性。
3. 用 maxUnavailable/maxSurge、PodDisruptionBudget 与拓扑分布控制容量窗口。
4. 逐步放量，比较新旧版本的错误率、尾延迟和业务指标。
5. 回滚前判断是否存在不可逆 Schema、消息或外部副作用。

readiness 防止未就绪实例接流量，liveness 用于恢复无法自愈的进程，startup probe 保护慢启动。错误的 liveness 会把依赖故障放大成重启风暴。

# 集群与工作负载安全

Kubernetes API 是关键控制面，应限制网络暴露并使用最小 RBAC。工作负载默认使用非 root、只读根文件系统、最小 Linux capabilities、RuntimeDefault seccomp，并避免自动挂载不需要的 ServiceAccount token。

NetworkPolicy 要同时考虑 ingress 与 egress；是否生效取决于 CNI 实现。Secret 对象不是默认端到端秘密管理方案：还需控制 API/RBAC、etcd 静态加密、外部密钥系统、注入路径、轮换与日志泄漏。

# 生产演练

为一个包含 API、数据库和缓存的服务建立故障手册，至少演练：错误探针、DNS 失败、无 Endpoint、节点压力、镜像拉取失败、下游超时和证书过期。每次演练保留时间线、命令、事件/指标截图、止损动作、恢复验证和长期修复，不以“Pod 重新 Running”作为结束标准。

# 参考资料

- [Kubernetes Troubleshooting](https://kubernetes.io/docs/tasks/debug/)
- [Kubernetes Observability](https://kubernetes.io/docs/concepts/cluster-administration/observability/)
- [Kubernetes Security Checklist](https://kubernetes.io/docs/concepts/security/security-checklist/)

