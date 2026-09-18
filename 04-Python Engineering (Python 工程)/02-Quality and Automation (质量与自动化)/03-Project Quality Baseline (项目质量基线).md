---
status: stable
confidence: high
content_verified: 2026-09-18
previous_review_due: 2026-09-30
---

> [!note] 方法论坐标
> 跨语言的合并与发布判定见 [Release Verification and Quality Gates (发布验证与质量门禁)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/07-Release%20Verification%20and%20Quality%20Gates%20(发布验证与质量门禁).md)；本篇聚焦 Python 项目的格式、类型、测试和依赖基线。

> [!abstract] 学习定位
> 小脚本一旦被重复运行或交给别人使用，就应具备可安装、可测试、可检查、可观测的最小工程边界。

> [!summary] 核心摘要
>
> 把入口、依赖、测试、静态检查和日志放进同一个可重复执行的流程；先让别人能安装和验证，再谈扩展功能。

# 项目边界

即使项目只有几百行，也应把“业务规则”和“外部世界”分开。规则层接收普通值并返回结果；adapter 负责文件、HTTP、数据库与环境变量。这样测试失败时能分辨是规则错误、依赖不可用，还是配置错误。

```text
CLI / scheduler -> configuration -> application service -> domain logic
                                              |
                                  file / HTTP / DB adapters
```

一种常见布局如下；目录名称不是标准答案，依赖方向才是重点。

```text
project/
├─ pyproject.toml
├─ README.md
├─ src/example_app/
│  ├─ __init__.py
│  ├─ cli.py
│  ├─ service.py
│  └─ adapters.py
└─ tests/
   ├─ test_service.py
   └─ test_cli.py
```

`pyproject.toml` 集中声明构建系统、项目元数据、依赖与工具配置。可执行入口应调用一个普通函数，使 CLI 解析与业务逻辑可以分别测试。

# 质量流水线

- 格式化与静态检查：统一风格，尽早发现明显错误。
- 单元测试：覆盖纯函数、失败分支和边界数据。
- 日志：记录输入规模、关键阶段与失败上下文，避免打印敏感信息。
- 配置：环境变量或配置文件注入，不把密钥和机器路径写死。
- CI：在干净环境运行检查和测试。

本地与 CI 应调用同一组命令，避免“我机器上能跑”的隐式状态。合理顺序是：安装锁定的依赖、静态检查、单元测试、集成测试、构建产物、运行最小冒烟示例。工具可以替换，但任一步失败都应阻止错误版本继续交付。

测试按成本分层：纯函数单元测试数量最多；adapter 用临时目录、假服务或测试数据库验证；只有少量端到端测试覆盖真实入口。不要把所有测试都依赖互联网，否则失败既慢又难定位。

# 配置、错误与日志

- 缺少必要配置时在启动阶段失败，并说明缺少哪个键。
- 库代码返回或抛出带语义的错误，由 CLI 决定信息和非零退出码。
- 日志使用稳定字段，如 `task_id`、`input_count`、`duration_ms`、`error_kind`。
- 网络调用必须设置 timeout；重试要有上限、退避和幂等前提。
- 密钥、token、完整个人数据既不能写进代码，也不能进入日志。

# 可交付的完成信号

在另一台机器上，按 README 的少量步骤即可安装依赖、执行测试并运行一次示例任务。发布前逐项验证：

1. 从空环境创建虚拟环境，安装项目及开发依赖。
2. 运行格式化检查、静态检查和测试，确认命令可重复。
3. 用最小输入运行 CLI，检查成功输出、失败退出码与日志。
4. 构建 wheel 或目标产物，并从产物安装一次，而不只是从源码目录运行。
5. 检查密钥、机器绝对路径、缓存和测试数据没有进入产物。

> [!question]- 理解检查
> 如果单元测试全绿，但陌生机器无法运行项目，应从依赖声明、构建产物、配置入口和 README 哪一层开始定位？

> [!info]- 官方参考
> - [Packaging Python Projects](https://packaging.python.org/en/latest/tutorials/packaging-projects/)
> - [Writing your pyproject.toml](https://packaging.python.org/en/latest/guides/writing-pyproject-toml/)
