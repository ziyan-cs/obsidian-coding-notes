# 后端工程学习笔记

以 C++ 的对象与资源模型、Python 工程实践和 Go 服务开发为主线，逐步连接系统、网络、数据、可靠性与项目交付。笔记以中文解释概念，保留必要的英文术语；每篇知识点末尾有动手题、机制追问和折叠参考。

这是学习笔记库，不是可直接部署的服务项目。涉及版本行为、性能数字或安全结论时，请核对笔记注明的环境、验证方式和原始资料。

## 开始阅读

| 需求 | 入口 |
| --- | --- |
| 按任务继续学习 | [学习周期](<./00-Hub (学习入口)/09-Study Cycle (学习周期).md>) |
| 浏览知识结构 | [知识库地图](<./00-Hub (学习入口)/00-Vault Map (知识库地图).md>) |
| 跨主题查阅 | [主题知识图谱](<./00-Hub (学习入口)/08-Topic Network (主题知识图谱).md>) |
| 了解能力方向 | [后端学习路线](<./00-Hub (学习入口)/01-Backend Learning Roadmap (后端学习路线).md>) |
| 安排复习 | [间隔复习系统](<./00-Hub (学习入口)/02-Spaced Review System (间隔复习系统).md>) |
| 查看写作约定 | [笔记规范](<./00-Hub (学习入口)/03-Note Standard (笔记规范).md>) |

**在 GitHub 阅读**：从上表进入笔记；库内链接使用相对路径，可在仓库页面继续导航。笔记中的 Obsidian 提示框在 GitHub 上可能显示为普通引用块，正文和代码仍可阅读。

**在 Obsidian 使用**：克隆或下载仓库，将目录作为 Obsidian 库打开。

```bash
git clone https://github.com/ziyan-cs/obsidian-coding-notes.git
```

笔记、属性和题目无需第三方插件；仓库内的外观配置可按个人偏好调整。Obsidian 标签面板中的 `topic/*` 可跨模块查看同一主题，`language/*` 可按语言筛选。学习记录保存在笔记的 YAML 属性中，不会由脚本自动改写。

## 内容导航

| 编号 | 模块入口 |
| --- | --- |
| 01 | [基础能力](<./01-Foundations (基础能力)/00-Foundations Standard (基础能力标准与自测).md>) |
| 02 | [工程基础](<./02-Engineering Fundamentals (工程基础)/00-Engineering Fundamentals Standard (工程基础标准与自测).md>) |
| 03 | [C++ 后端](<./03-C++ Backend (C++ 后端)/00-C++ Backend Standard (C++ 后端标准与自测).md>) |
| 04 | [Python 工程](<./04-Python Engineering (Python 工程)/00-Python Engineering Standard (Python 工程标准与自测).md>) |
| 05 | [Go 后端](<./05-Go Backend (Go 后端)/00-Go Backend Standard (Go 后端标准与自测).md>) |
| 06 | [系统与网络](<./06-Systems and Networking (系统与网络)/00-Systems and Networking Standard (系统与网络标准与自测).md>) |
| 07 | [数据系统与分布式](<./07-Data Systems and Distributed Computing (数据系统与分布式)/00-Data Systems and Distributed Computing Standard (数据系统与分布式标准与自测).md>) |
| 08 | [云原生与可靠性](<./08-Cloud Native and Reliability (云原生与可靠性)/00-Cloud Native and Reliability Standard (云原生与可靠性标准与自测).md>) |
| 09 | [安全工程](<./09-Security Engineering (安全工程)/00-Security Engineering Standard (安全工程标准与自测).md>) |
| 10 | [AI 系统工程](<./10-AI Systems Engineering (AI 系统工程)/00-AI Systems Engineering Standard (AI 系统工程标准与自测).md>) |
| 11 | [项目实践](<./11-Projects (项目实践)/00-Project Standard (项目标准与自测).md>) |
| 12 | [求职与成长](<./12-Career (求职与成长)/00-Career Standard (求职能力标准与自测).md>) |

学习周期串联基础程序、C++ 资源与构建、HTTP 服务、Go 数据服务和项目实践；算法穿插练习，分布式、云原生、安全与 AI 等专题按实际项目或目标岗位深化。模块的 `00-... Standard` 页给出掌握范围和综合自测。

## 复习检索

在仓库根目录运行 [Shell 脚本](./List-ReviewDue.sh) 或 [PowerShell 脚本](./List-ReviewDue.ps1)。默认列出**逾期、今天和明天**的笔记；`-All` / `--all` 列出所有已填写 `review_due` 的笔记。输出包含日期、阶段和相对路径。脚本只读取文件，不修改学习进度。

**Linux / macOS（Bash）**

```bash
bash ./List-ReviewDue.sh
bash ./List-ReviewDue.sh --all
bash ./List-ReviewDue.sh --root /path/to/vault
```

**Windows（PowerShell）**

```powershell
.\List-ReviewDue.ps1
.\List-ReviewDue.ps1 -All
.\List-ReviewDue.ps1 -Root 'D:\Notes\Obsidian-Coding'
```

PowerShell 7 在 Linux / macOS 上也可运行：`pwsh -File ./List-ReviewDue.ps1 -All`。Shell 脚本使用系统自带的 GNU 或 BSD `date`；Bash 3.2 及以上适用。若所有 `review_due` 仍为空，检索结果为空是预期行为。

第一次学懂一篇笔记后，把 `study_stage` 改为 `D1`，并将 `review_due` 填为次日日期（`YYYY-MM-DD`）。复习阶段和日期的更新规则见[间隔复习系统](<./00-Hub (学习入口)/02-Spaced Review System (间隔复习系统).md>)。

## 维护与来源

笔记的事实依据、时效和改动原则见[来源与时效规则](<./00-Hub (学习入口)/06-Sources & Freshness Policy (来源与时效规则).md>)；发现内容或链接问题，可按[审计与迁移队列](<./00-Hub (学习入口)/07-Vault Audit & Migration Queue (审计与迁移队列).md>)记录。图表、代码与练习随对应笔记维护。
