---
status: learning
confidence: low
content_verified: 2026-09-18
tags: [cloud/iac, cloud/gitops]
---

> [!abstract] 学习定位
> 基础设施和部署状态应以可审查、可重复、可回滚的声明描述；GitOps 用持续协调把声明与实际环境连接起来。

> [!summary] 核心摘要
>
> IaC 解决环境如何创建，GitOps 解决期望状态如何持续交付与纠偏。二者都需要版本、评审、状态管理、秘密边界和漂移检测。

# 声明与状态

声明式配置描述目标状态，控制器负责比较并收敛；命令式脚本描述操作步骤。声明式并不自动幂等，资源名称、外部副作用和 provider 行为仍需验证。

IaC state 记录资源身份与依赖，是敏感且关键的数据。它应使用远端存储、并发锁、加密与备份，不能随意复制或手工编辑。

# 变更流程

~~~text
change -> validate -> plan/diff -> review -> apply/reconcile
       -> observe -> verify -> rollback or forward fix
~~~

计划输出可能包含敏感信息；生产 apply 需要受控身份和审批。导入既有资源、重命名和删除尤其要确认实际影响，不能只看配置文本。

# GitOps 边界

Git 仓库保存期望状态，集群内控制器拉取并协调。凭据不应以明文提交；使用外部秘密系统、加密清单或运行时注入。紧急手工修改会产生 drift，事后必须回写声明或撤销。

多环境应复用基础配置并显式表达差异，避免复制整套 YAML。版本升级先在较小环境验证，再逐步推进。

# Terraform 式生命周期与状态边界

以 Terraform 为例，配置、state 和真实基础设施是三份不同信息。`plan` 根据配置与 state 推导变更，但 provider 读取失败、未知值和外部手工修改都会影响结果；plan 不是绝对安全证明。

远端 state 需要加密、访问控制、锁和版本备份。state 可能包含资源属性与敏感值，不能作为普通 artifact 公开。资源重命名、导入和迁移 state 时先在副本或非生产环境验证，并保留恢复点。

模块边界围绕稳定能力，例如网络、集群或服务数据库，而不是把每个资源包装一层。输入、输出和 provider 版本形成模块契约；升级先阅读变更说明、生成 plan、验证迁移，再逐环境推广。

# GitOps 的协调循环

```text
Git desired state -> reconciler -> cluster actual state
       ^                 |
       +--- status -------+
```

控制器反复协调意味着错误声明也会被反复执行。删除、缩容和权限变化需要保护策略；暂停协调只是临时止损，最终要让 Git 中的声明重新成为事实。

应用代码、部署清单和环境 promotion 可以分仓或同仓，但必须能追踪“哪个制品摘要由哪个声明部署到哪个环境”。不要只保存可变镜像 tag，应固定 digest 或不可变版本。

# 可执行实验

- 从空环境创建资源，再销毁并重建，验证文档和依赖是否完整。
- 手工修改一个标签或副本数，观察 drift 如何被发现和纠正。
- 制造并发 apply，验证远端锁与失败恢复。
- 模拟控制器不可用，确认现有 workload 不会因此停止，但新变更无法收敛。
- 对删除数据库、放开公网访问等高风险 plan 设置人工批准和策略阻断。

官方参考：[OpenGitOps Principles](https://opengitops.dev/)、[Terraform State](https://developer.hashicorp.com/terraform/language/state)。

# 验证

- 新环境能否从声明重建？
- 删除资源是否经过保护与审批？
- 控制器失败时是否可观察？
- 回滚应用版本时，数据库和基础设施是否兼容？
- 手工漂移能否被发现并处理？

> [!info]- 官方参考
> - [OpenGitOps Principles](https://opengitops.dev/)
