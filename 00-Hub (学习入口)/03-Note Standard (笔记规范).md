---
tags: [vault/standard, learning/note-taking]
status: solid
verified: 2026-09-05
---

# 03-Note Standard (笔记规范)

> [!abstract] 一篇好笔记不是信息堆积：不看正文时，你仍能说清它解决的问题、写出核心例子，并知道最容易错在哪里。

## 核心笔记模板

```md
---
tags: [language/cpp, topic/ownership]
status: seed
review_due: YYYY-MM-DD
confidence: 1
verified: YYYY-MM-DD # 仅工具/API/版本等易变主题必填
---

# English Topic — 中文主题

> [!abstract] 一句话结论：

## 解决的问题
## 心智模型
## 最小可运行例子
## 边界与误区
## 30 秒回答
## 自测
1. 
2. 
## 关联
- [[...]]
```

## 写作规则

- 一篇只回答一个核心问题；超过两个独立问题就拆分或改为导航页。
- 首次出现写“中文（English term）”，后续优先使用英文术语。
- 代码标注语言与版本；若未执行，明确写“未运行”。
- “高性能”“线程安全”“推荐”等结论必须写条件与代价。
- 工具/API/版本类信息记录一手来源与 `verified` 日期。
- 一级标题一篇只能有一个；正文从二级标题开始。使用 `abstract`、`warning`、`tip` callout 时，每段只服务一个阅读目的。

## 旧笔记渐进改造顺序

1. 先保留正确知识，删掉重复、无来源数字和错误结论。
2. 重写一句话结论、问题、心智模型、最小例子与边界。
3. 给最关键的 5–15 行代码补输入约束、失败场景或解释。
4. 需要时再加跨语言对照；最后按新知识依赖重建关联链接。

## 跨语言对照最小表

| 维度 | C++ | Go | Python |
| --- | --- | --- | --- |
| 解决的问题 |  |  |  |
| 主要机制 |  |  |  |
| 常见坑 |  |  |  |
| 适用边界 |  |  |  |

不要为了填表硬做三语言比较；只有概念确实对应时才填写。

## 关联

- [[00-Vault Map (知识库地图)]]
- [[02-Spaced Review System (间隔复习系统)]]
- [[06-Sources & Freshness Policy (来源与时效规则)]]
- [[08-Vault Design System (知识库设计系统)]]
