---
status: learning
confidence: low
content_verified: 2026-09-18
tags: [security/cloud, security/container, security/iam]
---

> [!abstract] 学习定位
> 云环境的核心安全边界是身份、权限和工作负载隔离；容器镜像只是交付格式，不自动成为强安全边界。

> [!summary] 核心摘要
>
> 每个工作负载使用独立身份和最小权限，通过短期凭据访问资源；镜像、运行配置、网络策略和集群 RBAC 共同决定攻击面。

# 身份优先于网络位置

服务不应因为“位于内网”就被默认信任。工作负载身份需要回答：谁在调用、允许执行什么、凭据能用多久、怎样撤销和审计。优先使用平台签发的短期身份，避免把长期云密钥写入镜像、环境模板或代码仓库。

RBAC 权限按 namespace、资源和动词拆分，避免通配符。人类管理员、CI/CD 和在线服务使用不同身份；高风险操作需要更强认证、审批或临时授权。

# 镜像与运行配置

- 使用受维护的最小基础镜像并锁定可追溯版本。
- 多阶段构建隔离编译工具；生成 SBOM，扫描依赖和镜像。
- 默认非 root，移除不需要的 Linux capabilities，启用只读文件系统。
- 限制 CPU、内存、进程数和临时存储。
- 不把 Docker socket、宿主机目录或特权模式暴露给普通工作负载。

签名与扫描证明来源和已知风险，不证明镜像没有漏洞；运行时配置错误同样可能绕过构建阶段控制。

# Kubernetes 边界

NetworkPolicy 控制允许流量，但能否生效取决于网络插件。Secret 对象不是机密管理终点；还需关注 etcd 加密、访问权限、外部密钥系统和日志泄漏。

Admission policy 可阻止特权容器、未签名镜像和危险挂载。Pod Security、seccomp、AppArmor/SELinux 与沙箱运行时提供不同层次限制，应按威胁模型组合。

# Pod 到云资源的身份链

优先使用工作负载身份联合：Pod 的 ServiceAccount 经受信任发行方换取短期云凭据，策略同时约束 namespace、service account、audience 和目标角色。避免节点级权限被所有 Pod 继承，也不要把长期 access key 放进 Secret 后永不轮换。

容器基线包括非 root、只读根文件系统、删除 capabilities、seccomp、资源限制和受控卷。镜像固定 digest、扫描并验证来源；admission policy 阻止特权、hostPath、hostNetwork 和未批准 registry。NetworkPolicy 需要同时验证入口和出站，且不能替代应用身份授权。

实验：用普通 Pod 尝试读取其他 namespace Secret、访问 metadata、创建特权容器和连接未授权服务；预期由 RBAC、网络、admission 与云 IAM 分层阻断，并产生审计事件。

参考：[Kubernetes Security Checklist](https://kubernetes.io/docs/concepts/security/security-checklist/)。

# 验证清单

1. 被入侵的 Pod 能访问哪些云 API、Secret 和其他服务？
2. ServiceAccount 是否被无关工作负载共享？
3. 镜像来源、依赖和部署清单能否追溯？
4. 管理员权限如何申请、过期和审计？
5. 节点或容器逃逸时还有哪些隔离层？

> [!info]- 官方参考
> - [Kubernetes Security](https://kubernetes.io/docs/concepts/security/)
> - [Kubernetes RBAC](https://kubernetes.io/docs/reference/access-authn-authz/rbac/)
