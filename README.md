# Obsidian Coding Notes

> **定位**：面向零基础系统学习的中文工程笔记库。主线是 C++ 后端；Python 先用于自动化、测试与辅助开发；随后进入 Go 服务端工程。每篇笔记追求“能理解、能实践、能复习”，而不是碎片收藏。

## 从这里开始

- 总入口：[00-Vault Map (知识库地图)](/00-Hub%20(学习入口)/00-Vault%20Map%20(知识库地图).md)
- 学习周期：[09-Study Cycle (学习周期)](/00-Hub%20(学习入口)/09-Study%20Cycle%20(学习周期).md)
- 到期复习：在根目录运行 `./List-ReviewDue.ps1`；查看完整队列运行 `./List-ReviewDue.ps1 -All`。

## 阅读约定

1. 每个一级学习模块只保留一篇 `00-... Standard`：它是完成标准与综合自测，不是文件目录。先看标准明确目标，再按二级目录与文件序号学习。
2. 正文链接只在确有前置、对照或实践依赖时出现；不保留散落的旧式指向句，也不为每篇笔记重复挂导航链接。
3. 普通笔记的复述检查使用默认折叠的 callout，不进入大纲；自测必须指向本专题的判断、代码或故障场景。
4. `review_due` 只表示下一次建议复习日，学完或复习后按实际表现调整；它不是固定死线。

## 笔记属性

所有笔记使用统一的 YAML 属性顺序；后两项按需存在：

```yaml
---
status: learning       # learning | stable
confidence: medium     # low | medium | high
verified: 2026-09-17   # 最近一次编辑审阅日
review_due: 2026-09-18 # 可选：下一次建议复习日
tags: [topic/example]  # 可选：检索标签
---
```

`verified` 表示笔记经过编辑审阅的日期，不等同于所有外部事实在当天重新验证；涉及版本、API、性能数据的内容仍应优先查官方文档或本机测量。

## 模块完成标准

| 顺序 | 主题 | 学完后应会什么 |
| --- | --- | --- |
| 01 | 基础能力 | [00-Foundations Standard (基础能力标准与自测)](/01-Foundations%20(基础能力)/00-Foundations%20Standard%20(基础能力标准与自测).md) |
| 02 | C++ 后端 | [00-C++ Backend Standard (C++ 后端标准与自测)](/02-C%2B%2B%20Backend%20(C%2B%2B%20后端)/00-C%2B%2B%20Backend%20Standard%20(C%2B%2B%20后端标准与自测).md) |
| 03 | Python 工程 | [00-Python Engineering Standard (Python 工程标准与自测)](/03-Python%20Engineering%20(Python%20工程)/00-Python%20Engineering%20Standard%20(Python%20工程标准与自测).md) |
| 04 | Go 后端 | [00-Go Backend Standard (Go 后端标准与自测)](/04-Go%20Backend%20(Go%20后端)/00-Go%20Backend%20Standard%20(Go%20后端标准与自测).md) |
| 05 | 运行时与网络 | [00-Runtime and Network Standard (运行时与网络标准与自测)](/05-Runtime%20and%20Network%20(运行时与网络)/00-Runtime%20and%20Network%20Standard%20(运行时与网络标准与自测).md) |
| 06 | 数据与分布式 | [00-Data and Distributed Standard (数据与分布式标准与自测)](/06-Data%20and%20Distributed%20(数据与分布式)/00-Data%20and%20Distributed%20Standard%20(数据与分布式标准与自测).md) |
| 07 | 工程工具 | [00-Engineering Tools Standard (工程工具标准与自测)](/07-Engineering%20Tools%20(工程工具)/00-Engineering%20Tools%20Standard%20(工程工具标准与自测).md) |
| 08 | 项目实践 | [00-Project Standard (项目标准与自测)](/08-Projects%20(项目实践)/00-Project%20Standard%20(项目标准与自测).md) |
| 09 | 求职与成长 | [00-Career Standard (求职能力标准与自测)](/09-Career%20(求职与成长)/00-Career%20Standard%20(求职能力标准与自测).md) |

## 维护边界

- `.obsidian/`、`.git/`、`.trash/` 与 `assets/` 不属于笔记正文，不由内容整理流程改动。
- 新增文件沿用 `序号-English (中文).md`，目录最多三级；先决定归属，再开始写正文。
- 一个知识目录原则上至少包含 3 篇实质笔记；不足时优先合并到相邻主题，不用占位页凑数量。
- 超大笔记只在存在独立前置知识、实践闭环和复习价值时拆分，避免少量内容独占一篇。
