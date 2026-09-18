---
status: learning
confidence: high
content_verified: 2026-09-18
verified: 2026-09-27
review_stage: learn
review_due: 2026-09-27
tags: [language/python, engineering/workflow]
---

> [!abstract] 学习定位
> 本篇描述 Python 小工具从问题到可交付制品的长期工作流，不规定四周进度。当前任务由 Hub 的 Study Cycle 选择。

> [!summary] 核心摘要
>
> Python 实践应从真实重复劳动出发，先固定输入输出和失败语义，再补测试、日志、打包与干净环境验证；语法覆盖率不是完成标准。

# 定义问题与边界

先写清使用者、输入来源、输出格式、数据规模、允许修改的资源和失败后的状态。首个项目应足够小，例如日志汇总、文件转换或 API 健康检查，避免同时引入 Web 框架、数据库和异步系统。

# 建立最小实现

用纯函数表达转换规则，把文件、时间、网络和 subprocess 放在边界层。路径使用 `pathlib`，资源用 context manager，外部调用设置 timeout；输入经过 schema、长度和类型校验，错误通过异常链与稳定退出码表达。

# 验证与失败安全

测试覆盖正常、边界、非法输入和副作用。文件写入使用临时文件加原子替换，批处理提供 dry-run 或可恢复记录；日志不泄漏 token、完整请求或个人数据。并发只有在顺序、资源上限和取消语义明确后引入。

# 工程化交付

项目使用 `pyproject.toml` 声明元数据与依赖，锁定实际版本，提供 CLI 入口、配置样例、类型检查、lint 和测试命令。从空目录仅按 README 安装并运行，才能证明没有依赖本机隐式状态。

# 完成证据

一个 Python 工具完成时，应留下源码、测试、构建制品、匿名样例、一次真实失败记录和恢复方式。进一步选题和学习进度统一进入 Study Cycle，本篇不保存周次或月末清单。

