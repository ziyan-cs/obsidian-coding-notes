---
status: stable
confidence: high
content_verified: 2026-09-18
tags: [engineering/verification]
---

> [!abstract] 学习定位
> 安全检测的目标不是收集尽可能多的日志，而是把攻击假设转成可执行检测，并在事件发生后保留可信证据。本篇统一威胁驱动检测、日志语义、告警分级、证据保全和响应交接。

# 从威胁场景反推检测

每条检测规则先写清五件事：

```text
Threat      攻击者要完成什么
Telemetry   哪些可信数据源能观察到它
Logic       如何从事件识别行为
Triage      值班人员如何排除正常情况
Response    确认后如何止损与保全证据
```

例如“凭据填充”不能只写“登录失败次数高”。需要区分单账户多来源、单来源多账户、成功登录前后的失败序列，并结合设备、地理、速率和已知代理特征；阈值必须用历史基线验证。

## 安全日志的最小语义

重要事件包括认证、授权、密钥与权限变化、管理操作、数据导出、配置修改、进程/容器生命周期和安全控制失效。事件字段至少包含：

| 类别 | 关键字段 |
|---|---|
| 时间与来源 | UTC 时间、时区/时钟状态、主机、服务、环境 |
| 主体 | account、service identity、来源地址、会话标识 |
| 动作与对象 | operation、resource、权限范围、结果 |
| 因果关联 | request_id、trace_id、parent event、change/build id |
| 安全结果 | allow/deny、reason code、rule/version、risk level |

拒绝原因使用稳定枚举，面向用户的响应避免泄漏账户是否存在等信息。令牌、密码、私钥和完整会话凭据不得记录；必要敏感字段应最小化、脱敏并限制访问。

# 检测工程生命周期

1. 建立威胁模型和高价值资产清单。
2. 验证数据源完整性、时间同步和字段含义。
3. 用历史数据或攻击模拟开发规则。
4. 记录预期误报、盲区、严重度和负责人。
5. 通过 replay、purple-team 或受控测试验证。
6. 监控规则命中率、误报率、数据延迟和解析失败。
7. 架构、身份模型或日志 schema 变化时重新验证。

检测即代码：规则、测试样本、例外、版本和评审记录都应进入版本管理。不能只测“恶意样本会告警”，还要测正常高峰不会触发，以及数据缺失时会有独立健康告警。

## 告警分级与调查

严重度综合资产价值、攻击阶段、置信度、影响范围和是否仍在进行。一次调查按时间线组织事实：

```text
first observed -> initial access -> privilege/action
               -> containment -> eradication -> recovery
```

先保存原始事件，再进行聚合和解释。区分事实、推断和未知项；任何结论都记录查询、时间范围、过滤条件和分析者。跨系统时间线要考虑时钟偏差和采集延迟。

值班人员需要能回答：告警是否真实、影响哪些身份/资产、攻击者还能否继续、是否存在数据泄露或持久化、需要保全哪些证据、谁有权执行隔离。

# 证据保全与可追溯性

证据包应包含原始日志或镜像、哈希、获取时间、获取人、工具版本、时区和来源系统。访问、复制与转交形成 chain of custody；分析在副本上进行，原件只读保存。

日志集中存储并不自动等于可信：攻击者可能删除本地日志、污染字段或盗用合法身份。关键审计流应远程传输、限制删除权限、监控采集缺口，并设置符合组织和法规要求的保留期。

# 检测质量与边界

- coverage：关键威胁场景是否有可验证检测，而非规则总数；
- precision：命中中有多少值得调查；
- recall 无法直接精确得知，可用演练与已知事件估计盲区；
- MTTD/MTTA/MTTR：发现、确认和恢复耗时；
- telemetry health：数据新鲜度、缺失率、解析失败和时钟偏差；
- rule health：无命中、突增、异常静默和版本漂移。

指标应服务决策。通过压低告警数量获得“高 precision”，却遗漏真实攻击，不是成功。

## 与普通可观测性的边界

运维日志主要解释系统是否健康，安全日志还需证明“谁在何时对什么做了什么”，并具有更严格的完整性、权限与保留要求。两者可以共享采集管线和关联 ID，但安全事件不应被普通采样策略随机丢弃。

# 实践与资料

- [ ] 为登录、权限变更和批量导出各写一条 threat-to-detection 说明。
- [ ] 制作恶意、正常高峰、数据缺失三类测试样本并自动回放。
- [ ] 从一次模拟入侵生成带 UTC 时间、查询和证据哈希的调查时间线。
- [ ] 验证安全日志中不存在令牌、密码和完整个人敏感信息。
- [ ] 为高危告警写明隔离权限、升级联系人和恢复验收条件。

## 关联专题

- [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)：理解共用遥测管线与关联上下文。
- [Security Logging and Detection (安全日志与检测)](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/02-Security%20Logging%20and%20Detection%20(安全日志与检测).md)：安全领域的规则、平台与演练实现。
- [Incident Response and Recovery (安全事件响应与恢复)](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/03-Incident%20Response%20and%20Recovery%20(安全事件响应与恢复).md)：确认事件后的控制、根除和恢复。

## 参考资料

- [NIST SP 800-61 Rev. 3](https://csrc.nist.gov/pubs/sp/800/61/r3/final)
- [NIST Secure Software Development Framework](https://csrc.nist.gov/projects/ssdf)
- [OWASP Logging Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Logging_Cheat_Sheet.html)
