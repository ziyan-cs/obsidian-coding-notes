---
tags: [language/python, python/testing]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-pytest Fixtures and Test Doubles (测试夹具与替身)

> [!abstract] 一句话结论：pytest 测试行为而不是打印结果；对文件、时间、网络等外部依赖，用临时目录或 test double 控制输入。

## 30 秒回答

**01-pytest Fixtures and Test Doubles (测试夹具与替身)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## 命令与规则

```text
python -m pytest
python -m pytest -q
```

- 测试命名 `test_*.py`；每个 test 只验证一个可读行为。
- 使用 `tmp_path`，不要把真实 Vault、下载目录或用户文件当测试夹具。
- HTTP 层使用 mock/fake 或本地测试 server，避免单元测试依赖互联网。
- 失败断言要包含实际值和期望值，便于定位。

## 自测

给 JSON config loader 写：正确对象、顶层数组、缺失文件、非法 JSON 四个测试。

## Sources

- [pytest 入门文档](https://docs.pytest.org/en/stable/getting-started.html)
- 验证日期：2026-09-05

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 01-pytest Fixtures and Test Doubles (测试夹具与替身) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
