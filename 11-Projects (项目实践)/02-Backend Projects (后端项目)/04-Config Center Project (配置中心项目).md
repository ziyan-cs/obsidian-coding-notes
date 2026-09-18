---
status: learning
confidence: low
content_verified: 2026-09-17
tags: [project/config-center, distributed/configuration]
---

> [!abstract] 学习定位
> 配置中心应将配置作为不可变、可验证、可回滚的发布物；watch 是通知加速器，版本化拉取才是恢复事实。

> [!summary] 核心摘要
>
> 客户端须能在断连后按版本重新拉取并原子切换有效配置；历史不修改，回滚通过发布已知安全的新版本完成。

# 最小范围

- namespace / key / version / value / author / timestamp。
- client 拉取、版本比较、watch 通知、校验与回滚。
- 不做跨地域强一致和复杂权限系统。

# 必答问题

- 配置格式错误谁拦截？发布前还是客户端加载时？
- 通知丢失后如何补偿？客户端如何重新同步？
- 灰度发布如何定义目标集合与回滚点？
- 配置生效是否需要原子切换？

# 配置发布的最小记录

```json
{"namespace":"checkout","key":"rate_limit","version":17,"value":"200","author":"ops","created_at":"..."}
```

发布端先校验 schema 和危险范围，再创建新版本；客户端校验版本和值后，将完整新配置构造成新对象再替换旧引用。不要逐字段原地修改共享配置，否则请求可能读到半新半旧的状态。

# API、版本与存储模型

把一次发布建模为不可变 revision，而不是直接覆盖 key：

```text
namespace -> revision 17 -> complete configuration snapshot
          -> revision 18 -> complete configuration snapshot
release: revision 18 -> target group canary-a
```

最小 API 包含创建草稿/校验、发布 revision、按 namespace 和版本拉取、获取当前 release、watch 变更以及回滚。写接口带幂等键；发布使用 compare-and-swap 或期望基线版本，防止两名操作者互相覆盖。

数据库保存 revision、内容摘要、schema 版本、作者、审批人和时间；内容较大时存对象存储并在数据库保存不可变引用。审计记录单独追加，不允许普通发布者修改历史。

# Watch 是提示，拉取才是事实

客户端启动时先拉当前版本，再建立 watch。收到通知后只获得 namespace/version，随后拉取完整配置、验证 checksum/schema，并构造新对象原子替换。watch 断线后携带 last seen version 重连；服务端无法补齐历史时要求客户端重新拉当前快照。

```text
start -> fetch current -> validate -> activate
     -> watch(version changed)
     -> fetch new -> validate -> atomic swap -> report status
```

通知可能重复、延迟或丢失，因此客户端按版本幂等处理，并周期性对账。若新配置无法解析，继续使用 last-known-good，报告失败并告警；不能把坏配置激活一半。

# 灰度、权限与故障边界

发布目标可以按实例标签、用户组或百分比分组，但同一实例必须稳定落在同一组。先观察 canary 的业务 SLI 和配置加载状态，再扩大范围。回滚创建指向旧内容的新 release，保留完整审计链。

RBAC 至少区分读取、编辑、发布和管理权限；敏感配置不应以明文混入普通配置，可保存 secret reference 并由运行时身份读取秘密系统。所有管理操作记录 actor、source、diff 摘要和结果。

服务端不可用时，客户端继续使用本地加密/受保护的 last-known-good，并限制最大陈旧时间。首次启动且无本地版本时应显式失败或进入受限模式，不能静默使用空配置。

# 实验和指标

- 两个发布者基于同一旧版本提交，验证冲突检测。
- watch 丢通知、重复通知、乱序通知和断线重连。
- 新版本 schema 错误，验证旧版本仍在服务。
- 灰度一半失败，验证停止推广并回滚。
- 服务重启和数据库短暂故障，验证客户端不读到半成品。

监控发布成功/失败、传播延迟、各版本活跃实例数、watch 连接、重连、客户端校验失败、last-known-good 年龄与回滚次数。一次发布完成的判据是目标实例报告已激活且业务信号正常，不只是服务端写库成功。

# 验收

- [ ] 配置版本不可变，回滚创建新版本而非改历史。
- [ ] Python 校验器验证 schema 和危险配置。
- [ ] 模拟 watch 断连、漏通知和服务重启。

> [!summary]- 项目表达检查：学完后再展开
>
> 配置中心把配置当成带版本的发布物：服务读取的是可验证版本，watch 只是加速通知而不是唯一事实来源；断连后必须能重新拉取并比较版本，回滚通过生成新版本保留审计链。
>

> [!warning] 常见误区
> “所有实例最终拿到同一配置”不等于安全发布。格式校验、灰度目标、回滚点和生效原子性都需要单独设计。
