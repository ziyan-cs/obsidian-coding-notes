---
status: learning
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 02-Automation Project Template (自动化项目模板)

> [!abstract] 一句话结论：Python 的第一批项目应该解决真实重复劳动，并留下输入、输出、日志、测试和失败处理的工程证据。

## 30 秒回答

**核心结论**：一句话结论：Python 的第一批项目应该解决真实重复劳动，并留下输入、输出、日志、测试和失败处理的工程证据。


## 推荐题目

- API health checker：并发检查接口状态与延迟，输出 JSON/Markdown 报告。
- Log summarizer：解析服务日志，统计错误类型与时间窗口。
- Data migration verifier：比对源端和目标端的行数、主键覆盖与校验和。

## 每个项目必须有

1. 可配置的输入与输出目录。
2. dry-run 模式，先展示将执行什么。
3. 可重试的瞬态失败与明确的失败报告。
4. 最少三条测试：正常、空输入、异常输入。

## 推荐目录与执行路径

```text
src/       command and domain code
tests/     behavior-focused tests
README.md  setup, examples, failure handling

input -> validate -> dry-run preview -> execute -> report
```

先让 `dry-run` 输出“将处理多少条、将写入哪里、哪些项会跳过”，确认后才执行真实副作用。每次运行输出结构化摘要：成功数、失败数、重试数和失败样本；这比只说“完成”更适合复盘与定位。

## 交付证据

为一个真实重复任务保留：一份匿名输入样例、一条正常运行记录、一条失败处理记录和一组自动化测试。项目规模可以小，但证据链要完整。

## 常见误区

- 把脚本一次跑通当成工程正确，忽略环境隔离、输入校验、错误分类和可重复运行。
- 只读 API 名称而不为文件、网络、时间等外部边界写一个可控测试。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
02-Automation Project Template (自动化项目模板)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
