# Obsidian Coding Notes

> **定位**：面向零基础系统学习的中文工程笔记库。主线是 C++ 后端；Python 先用于自动化、测试与辅助开发；随后进入 Go 服务端工程。每篇笔记追求“能理解、能实践、能复习”，而不是碎片收藏。

## 从这里开始

- 总入口：[00-Vault Map (知识库地图)](/00-Hub%20(学习入口)/00-Vault%20Map%20(知识库地图).md)
- 学习周期：[09-Study Cycle (学习周期)](/00-Hub%20(学习入口)/09-Study%20Cycle%20(学习周期).md)
- 到期复习：在根目录运行 `./List-ReviewDue.ps1`；查看完整队列运行 `./List-ReviewDue.ps1 -All`。

## 阅读约定

1. 先读分类导航，再按文件序号学习；不要跳过“基础”和“最小实践”。
2. 每篇以 `30 秒回答` 检验能否讲清概念，以“自测”检验是否真的掌握。
3. `review_due` 只表示下一次建议复习日，学完或复习后由你按实际节奏调整；它不是固定死线。
4. 文件末尾的“关联学习”只提供当前最值得去的两处入口：分类导航和相邻学习步骤，不堆砌链接。

## 笔记属性

所有笔记使用统一的 YAML 属性顺序；后两项按需存在：

```yaml
---
status: learning       # learning | stable
confidence: medium     # low | medium | high
verified: 2026-09-06   # 最近一次编辑审阅日
review_due: 2026-09-07 # 可选：下一次建议复习日
tags: [topic/example]  # 可选：检索标签
---
```

`verified` 表示笔记经过编辑审阅的日期，不等同于所有外部事实在当天重新验证；涉及版本、API、性能数据的内容仍应优先查官方文档或本机测量。

## 知识地图

| 顺序 | 主题 | 入口 |
| --- | --- | --- |
| 00 | 学习入口 | [Vault Map](/00-Hub%20(学习入口)/00-Vault%20Map%20(知识库地图).md) |
| 01 | 基础能力 | [Foundations Map](/01-Foundations%20(基础能力)/00-Foundations%20Map%20(基础能力导航).md) |
| 02 | C++ 后端 | [C++ Backend Core Map](/02-C%2B%2B%20Backend%20(C%2B%2B%20后端)/00-C%2B%2B%20Backend%20Core%20Map%20(后端能力地图).md) |
| 03 | Python 工程 | [Python Map](/03-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/00-Python%20Map%20(Python导航).md) |
| 04 | Go 后端 | [Go Map](/04-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/00-Go%20Map%20(Go导航).md) |
| 05 | 运行时与网络 | [Runtime and Network MOC](/05-Runtime%20and%20Network%20(运行时与网络)/00-Runtime%20and%20Network%20MOC%20(运行时与网络导航).md) |
| 06 | 数据与分布式 | [Data and Distributed MOC](/06-Data%20and%20Distributed%20(数据与分布式)/00-Data%20and%20Distributed%20MOC%20(数据与分布式导航).md) |
| 07 | 工程工具 | [Tools Map](/07-Engineering%20Tools%20(工程工具)/00-Tools%20Map%20(工程工具导航).md) |
| 08 | 项目实践 | [Project Map](/08-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/00-Project%20Map%20(项目地图).md) |
| 09 | 求职与成长 | [Career Map](/09-Career%20(求职与成长)/00-Career%20Map%20(求职与成长导航).md) |

## 维护边界

- `.obsidian/`、`.git/`、`.trash/` 与 `assets/` 不属于笔记正文，不由内容整理流程改动。
- 历史深层目录的 Markdown 链接已清除；不要恢复旧路径。需要关联时，使用当前文件末尾的“关联学习”或分类导航。
- 新增文件沿用 `序号-English (中文).md`，目录最多三级；先决定它应归属的分类，再开始写正文。
