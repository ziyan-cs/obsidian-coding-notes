---
study_stage: backlog
tags: [engineering/verification]
---

> [!abstract] 学习定位
> 质量门禁把静态检查、测试、安全、制品完整性和上线观测转换为可执行的发布决策。目标不是检查越多越好，而是在反馈速度、风险和可追溯性之间建立明确规则。

# 从变更到可验证制品

```text
source -> review -> build -> test -> scan -> immutable artifact
       -> staging -> canary -> progressive rollout -> verification
                                      | failure -> rollback/roll forward
```

同一 revision 应生成一次制品，并在环境间晋升；不要在生产环境重新编译。制品关联 source revision、依赖锁文件、toolchain、构建日志、SBOM、签名/摘要和测试报告。

# 分层门禁

| 阶段 | 必要信号 | 失败处理 |
|---|---|---|
| Pre-commit | 格式化、快速 lint、局部单测 | 本地立即修复 |
| Pull request | 编译/类型、静态分析、单元/组件测试 | 禁止合并 |
| Main branch | 集成/契约、sanitizer、依赖与镜像扫描 | 禁止生成候选版本 |
| Release candidate | 迁移、回滚、性能、安全与恢复验证 | 不得推广 |
| Production rollout | canary SLI、错误预算、业务校验 | 自动暂停或回滚 |

门禁规则需要负责人、原因、例外审批、到期时间和恢复条件。永久 `allow_failure` 会把风险静默转移到生产。

## 静态检查与构建质量

格式化解决一致性；lint 与 static analysis 发现可疑模式；编译器警告、类型检查和依赖检查提供更强约束。工具输出要固定版本和配置，避免开发机与 CI 漂移。

缓存只加速计算，不能改变结果。缓存键需包含源码、依赖、编译器和关键选项；出现无法解释的差异时，应能禁用缓存重建。可复现构建要求相同输入尽可能得到相同输出，并控制时间戳、路径和非确定输入。

## 测试与安全门禁

门禁不是只看“测试通过”：还要确认执行了预期测试、没有被过滤、报告完整、失败 artifact 可访问。覆盖率只作为变化信号和盲区提示，不设置脱离风险的数字崇拜。

安全检查包括 secret scanning、依赖/镜像漏洞、许可证策略、SAST 和必要的 DAST。漏洞是否阻断应结合可利用性、暴露面和修复时限，但已知严重风险不能通过修改阈值消失。

数据库 schema、消息格式和公开 API 需要兼容性检查。expand/migrate/contract 分阶段发布，先让新旧版本共存，再清理旧字段。

# 上线前验证

发布候选应证明：

- 配置和密钥来自预期环境，启动失败是显式的；
- readiness 只在能够服务真实流量时成功；
- 数据迁移可重入、可观测，且有回滚或前滚方案；
- 容量、依赖限额和降级路径满足目标；
- runbook、值班人、变更窗口和停止条件已明确。

Smoke test 验证关键路径，不应对生产数据造成不可逆副作用。可用专用租户、幂等操作或可清理数据。

## 渐进式发布与自动判定

Canary 先接收小部分代表性流量，对照稳定版本比较成功率、延迟、资源、业务不变量和安全信号。每阶段要有最短观测窗口和最大风险窗口，避免刚启动即全量。

回滚条件必须在发布前定义。例如：

```text
5 min 内错误预算燃烧率超过阈值
或关键业务成功率显著低于 control
或出现数据一致性/安全事件
=> 暂停推广并执行回滚/流量切换
```

回滚应用版本不一定能回滚数据。涉及 schema、消息和外部副作用时，优先设计向前修复与兼容窗口。

# 发布证据与复盘

一次发布记录 revision、artifact digest、审批、配置差异、各阶段时间、监控快照、异常和最终结论。验证不仅是“Pod 都 Running”，还包括用户旅程与业务不变量。

失败门禁与事故需要反哺测试、检测和发布策略；不要只添加更多人工清单。能自动判定的规则进入流水线，需要判断的风险保留评审理由。

# 实践与资料

- [ ] 为一个服务画出从源码到生产的门禁图，标出每步证据和负责人。
- [ ] 构建一次 immutable artifact，并用 digest 证明测试与上线是同一制品。
- [ ] 为不兼容 schema 设计 expand/migrate/contract 发布过程。
- [ ] 编写 canary 自动停止条件并模拟失败回滚。
- [ ] 审核所有豁免项，补负责人、到期时间和恢复条件。

## 关联专题

- [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)：设计能支撑门禁决策的测试证据。
- [Security Detection and Incident Evidence (安全检测与事件证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/06-Security%20Detection%20and%20Incident%20Evidence%20(安全检测与事件证据).md)：把安全信号纳入发布和响应。

## 参考资料

- [NIST Secure Software Development Framework](https://csrc.nist.gov/projects/ssdf)
- [Kubernetes Deployments](https://kubernetes.io/docs/concepts/workloads/controllers/deployment/)
- [Google SRE Books](https://sre.google/books/)
