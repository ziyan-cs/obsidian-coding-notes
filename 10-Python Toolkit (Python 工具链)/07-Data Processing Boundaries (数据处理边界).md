---
tags: [language/python, python/data]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Data Processing Boundaries — 何时用 Python，何时不用

> **一句话结论**：Python 很适合离线分析、清洗、报告与自动化；线上高并发核心路径应基于性能、延迟、团队和已有系统选择语言，而非“Python 写得快”。

## 推荐场景

- 解析服务日志，统计 p50/p95/p99、错误码和热点接口。
- 合并 CSV/JSON，生成测试数据、报表或图表。
- 调用多个 API 做巡检、回归、批处理。
- 将压测、CI、数据库导出等重复流程变为 CLI。

## 边界和风险

- 数据比内存大：用流式处理、分块或交给数据库，不要直接 `read()` 全部内容。
- 需要精确并发/低尾延迟：先 profile；可能应放到 Go/C++ 服务。
- 处理不可信输入：限制文件大小、校验 schema、避免 `eval`、记录失败项。
- 分析脚本也要有输入版本、输出说明和测试，否则结论不可复现。

## 小练习

读取压测 CSV，按 endpoint 汇总请求数、错误率和 p95；输出 Markdown 表格。先用标准库 `csv`，数据量或分析需求明显增长后再学习 pandas。
