# 后端工程学习笔记

以 C++ 的对象与资源模型、Python 工程实践和 Go 服务开发为主线，逐步连接系统、网络、数据、可靠性与项目交付。笔记以中文解释概念，保留必要的英文术语；每篇知识点末尾有动手题、机制追问和折叠参考。

这是学习笔记库，不是可直接部署的服务项目。涉及版本行为、性能数字或安全结论时，请核对笔记注明的环境、验证方式和原始资料。

## 开始阅读

| 需求 | 入口 |
| --- | --- |
| 按任务继续学习 | [学习周期](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/09-Study%20Cycle%20%28%E5%AD%A6%E4%B9%A0%E5%91%A8%E6%9C%9F%29.md) |
| 浏览知识结构 | [知识库地图](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/00-Vault%20Map%20%28%E7%9F%A5%E8%AF%86%E5%BA%93%E5%9C%B0%E5%9B%BE%29.md) |
| 了解能力方向 | [后端学习路线](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/01-Backend%20Learning%20Roadmap%20%28%E5%90%8E%E7%AB%AF%E5%AD%A6%E4%B9%A0%E8%B7%AF%E7%BA%BF%29.md) |
| 安排复习 | [间隔复习系统](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/02-Spaced%20Review%20System%20%28%E9%97%B4%E9%9A%94%E5%A4%8D%E4%B9%A0%E7%B3%BB%E7%BB%9F%29.md) |
| 查看写作约定 | [笔记规范](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/03-Note%20Standard%20%28%E7%AC%94%E8%AE%B0%E8%A7%84%E8%8C%83%29.md) |

**在 GitHub 阅读**：从上表进入笔记；库内链接使用相对路径，可在仓库页面继续导航。笔记中的 Obsidian 提示框在 GitHub 上可能显示为普通引用块，正文和代码仍可阅读。

**在 Obsidian 使用**：克隆或下载仓库，将目录作为 Obsidian 库打开。

```bash
git clone https://github.com/ziyan-cs/obsidian-coding-notes.git
```

笔记、属性和题目无需第三方插件；仓库内的外观配置可按个人偏好调整。学习记录保存在笔记的 YAML 属性中，不会由脚本自动改写。

## 内容导航

| 编号 | 模块入口 |
| --- | --- |
| 01 | [基础能力](./01-Foundations%20%28%E5%9F%BA%E7%A1%80%E8%83%BD%E5%8A%9B%29/00-Foundations%20Standard%20%28%E5%9F%BA%E7%A1%80%E8%83%BD%E5%8A%9B%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 02 | [工程基础](./02-Engineering%20Fundamentals%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%29/00-Engineering%20Fundamentals%20Standard%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 03 | [C++ 后端](./03-C%2B%2B%20Backend%20%28C%2B%2B%20%E5%90%8E%E7%AB%AF%29/00-C%2B%2B%20Backend%20Standard%20%28C%2B%2B%20%E5%90%8E%E7%AB%AF%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 04 | [Python 工程](./04-Python%20Engineering%20%28Python%20%E5%B7%A5%E7%A8%8B%29/00-Python%20Engineering%20Standard%20%28Python%20%E5%B7%A5%E7%A8%8B%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 05 | [Go 后端](./05-Go%20Backend%20%28Go%20%E5%90%8E%E7%AB%AF%29/00-Go%20Backend%20Standard%20%28Go%20%E5%90%8E%E7%AB%AF%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 06 | [系统与网络](./06-Systems%20and%20Networking%20%28%E7%B3%BB%E7%BB%9F%E4%B8%8E%E7%BD%91%E7%BB%9C%29/00-Systems%20and%20Networking%20Standard%20%28%E7%B3%BB%E7%BB%9F%E4%B8%8E%E7%BD%91%E7%BB%9C%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 07 | [数据系统与分布式](./07-Data%20Systems%20and%20Distributed%20Computing%20%28%E6%95%B0%E6%8D%AE%E7%B3%BB%E7%BB%9F%E4%B8%8E%E5%88%86%E5%B8%83%E5%BC%8F%29/00-Data%20Systems%20and%20Distributed%20Computing%20Standard%20%28%E6%95%B0%E6%8D%AE%E7%B3%BB%E7%BB%9F%E4%B8%8E%E5%88%86%E5%B8%83%E5%BC%8F%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 08 | [云原生与可靠性](./08-Cloud%20Native%20and%20Reliability%20%28%E4%BA%91%E5%8E%9F%E7%94%9F%E4%B8%8E%E5%8F%AF%E9%9D%A0%E6%80%A7%29/00-Cloud%20Native%20and%20Reliability%20Standard%20%28%E4%BA%91%E5%8E%9F%E7%94%9F%E4%B8%8E%E5%8F%AF%E9%9D%A0%E6%80%A7%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 09 | [安全工程](./09-Security%20Engineering%20%28%E5%AE%89%E5%85%A8%E5%B7%A5%E7%A8%8B%29/00-Security%20Engineering%20Standard%20%28%E5%AE%89%E5%85%A8%E5%B7%A5%E7%A8%8B%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 10 | [AI 系统工程](./10-AI%20Systems%20Engineering%20%28AI%20%E7%B3%BB%E7%BB%9F%E5%B7%A5%E7%A8%8B%29/00-AI%20Systems%20Engineering%20Standard%20%28AI%20%E7%B3%BB%E7%BB%9F%E5%B7%A5%E7%A8%8B%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 11 | [项目实践](./11-Projects%20%28%E9%A1%B9%E7%9B%AE%E5%AE%9E%E8%B7%B5%29/00-Project%20Standard%20%28%E9%A1%B9%E7%9B%AE%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |
| 12 | [求职与成长](./12-Career%20%28%E6%B1%82%E8%81%8C%E4%B8%8E%E6%88%90%E9%95%BF%29/00-Career%20Standard%20%28%E6%B1%82%E8%81%8C%E8%83%BD%E5%8A%9B%E6%A0%87%E5%87%86%E4%B8%8E%E8%87%AA%E6%B5%8B%29.md) |

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

第一次学懂一篇笔记后，把 `study_stage` 改为 `D1`，并将 `review_due` 填为次日日期（`YYYY-MM-DD`）。复习阶段和日期的更新规则见[间隔复习系统](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/02-Spaced%20Review%20System%20%28%E9%97%B4%E9%9A%94%E5%A4%8D%E4%B9%A0%E7%B3%BB%E7%BB%9F%29.md)。

## 维护与来源

笔记的事实依据、时效和改动原则见[来源与时效规则](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/06-Sources%20%26%20Freshness%20Policy%20%28%E6%9D%A5%E6%BA%90%E4%B8%8E%E6%97%B6%E6%95%88%E8%A7%84%E5%88%99%29.md)；发现内容或链接问题，可按[审计与迁移队列](./00-Hub%20%28%E5%AD%A6%E4%B9%A0%E5%85%A5%E5%8F%A3%29/07-Vault%20Audit%20%26%20Migration%20Queue%20%28%E5%AE%A1%E8%AE%A1%E4%B8%8E%E8%BF%81%E7%A7%BB%E9%98%9F%E5%88%97%29.md)记录。图表、代码与练习随对应笔记维护。
