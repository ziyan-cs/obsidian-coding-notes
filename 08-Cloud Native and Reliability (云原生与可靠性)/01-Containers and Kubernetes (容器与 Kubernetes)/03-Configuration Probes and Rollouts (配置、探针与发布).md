---
status: learning
confidence: high
content_verified: 2026-09-17
tags: [cloud-native/kubernetes, operations/reliability]
---

> [!abstract] 学习目标
> 可交付的 Kubernetes 服务必须把配置、秘密、健康、发布和观测设计成运行时契约。探针不是装饰，Secret 也不是天然加密；每个配置项都要能安全变更和回滚。

# ConfigMap 与 Secret

ConfigMap 保存非敏感配置，Secret 保存敏感数据对象，但 Kubernetes Secret 默认并不等于“已安全加密”：访问 API 或底层 etcd 的权限仍是关键风险，需要启用静态加密、严格 RBAC，并限制哪些 workload 能引用 Secret。

配置注入方式：

- 环境变量：简单，但进程通常无法自动看到更新，且可能进入诊断信息。
- 卷文件：适合证书和结构化配置，可以观察文件变化，但应用必须正确 reload。
- 配置服务/API：支持动态更新，但引入可用性、缓存、版本和权限问题。

配置要有 schema、默认值、启动校验和版本。无法识别的关键配置应 fail fast；动态配置更新要先校验、再原子切换，并保留最后一个可用版本。

# Startup、Readiness 与 Liveness

三类探针回答不同问题：

- startupProbe：慢启动应用是否已经完成初始化；成功前抑制 liveness/readiness 的干扰。
- readinessProbe：当前实例是否可以接收新流量；失败应从 Service endpoints 移除，但不一定重启。
- livenessProbe：进程是否陷入无法自行恢复的状态；失败会触发重启。

不要让 liveness 依赖数据库、缓存等共享下游，否则下游故障可能导致所有 Pod 同时重启。readiness 可以反映关键依赖，但要防止短抖动造成流量雪崩。探针超时和阈值必须覆盖最坏但仍健康的响应时间。

```yaml
startupProbe:
  httpGet: { path: /startup, port: 8080 }
  failureThreshold: 30
  periodSeconds: 2
readinessProbe:
  httpGet: { path: /ready, port: 8080 }
  periodSeconds: 5
livenessProbe:
  httpGet: { path: /live, port: 8080 }
  periodSeconds: 10
```

# 发布与回滚

滚动发布同时存在新旧版本，因此接口、数据库 schema、消息格式和缓存 key 必须在过渡期兼容。推荐顺序是 expand → deploy → migrate traffic → contract，而不是先删除旧字段再部署新代码。

发布前明确：

- `maxUnavailable` 与 `maxSurge` 如何影响容量。
- 新版本 readiness 成功的真实条件。
- preStop、SIGTERM 和 termination grace 是否足够排空请求。
- 失败指标、停止条件、回滚命令和数据库回退策略。
- 旧版本是否能读取新版本写入的数据。

Deployment 回滚只能恢复 Pod template，不能自动回滚外部数据库、消息或对象存储中的状态。高风险变更使用 canary 或分批发布，并让指标决定是否继续，而不是只等待固定时间。

# 观测与排障

最小证据链：

```bash
kubectl get deploy,rs,pod,svc,endpointslice
kubectl describe pod <pod>
kubectl logs <pod> --previous
kubectl get events --sort-by=.lastTimestamp
kubectl rollout status deployment/<name>
kubectl rollout history deployment/<name>
```

排障顺序：对象是否创建 → Pod 是否调度 → 容器是否启动 → 探针是否通过 → Service 是否有 endpoint → 网络是否可达 → 应用依赖是否健康。

应用日志写 stdout/stderr 并使用结构化字段；指标至少覆盖请求率、错误率、延迟、资源饱和度和队列积压；trace 需要贯穿入口与下游。Kubernetes 自身事件与日志有保留和格式边界，生产环境需要独立的集中存储、告警和关联标识。

# 交付验收

- [ ] 镜像不可变、非 root、无构建凭据，依赖与来源可追踪。
- [ ] requests、limits、探针和优雅关闭经过故障测试。
- [ ] 配置与 Secret 不进入镜像，权限遵循最小化原则并可轮换。
- [ ] 发布过程中至少验证一条成功请求、一个预期失败和一次回滚。
- [ ] 能解释 Pending、CrashLoopBackOff、OOMKilled、ImagePullBackOff 和 Ready=false 的证据入口。
- [ ] 数据库或缓存不可用时，不会因探针设计引发全量重启风暴。

> [!summary] 核心摘要
> 说明一次新版本从镜像进入 Deployment，到 Pod Ready、Service 接流量、指标确认成功的完整过程；指出代码回滚不能自动撤销哪些外部状态。

官方参考：[Kubernetes Secrets](https://kubernetes.io/docs/concepts/configuration/secret/) · [Configure Probes](https://kubernetes.io/docs/tasks/configure-pod-container/configure-liveness-readiness-startup-probes/) · [Deployments](https://kubernetes.io/docs/concepts/workloads/controllers/deployment/)
