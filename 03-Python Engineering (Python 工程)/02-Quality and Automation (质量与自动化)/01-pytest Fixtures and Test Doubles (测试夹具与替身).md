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

**核心结论**：一句话结论：pytest 测试行为而不是打印结果；对文件、时间、网络等外部依赖，用临时目录或 test double 控制输入。


## 命令与规则

```text
python -m pytest
python -m pytest -q
```

- 测试命名 `test_*.py`；每个 test 只验证一个可读行为。
- 使用 `tmp_path`，不要把真实 Vault、下载目录或用户文件当测试夹具。
- HTTP 层使用 mock/fake 或本地测试 server，避免单元测试依赖互联网。
- 失败断言要包含实际值和期望值，便于定位。

## Fixture 只负责准备条件

```python
def test_loader_rejects_bad_json(tmp_path):
    config = tmp_path / "config.json"
    config.write_text("{bad", encoding="utf-8")
    with pytest.raises(ValueError, match="invalid"):
        load_config(config)
```

`tmp_path` 为每个测试提供独立临时目录，不会污染真实文件。fixture 应只建立可复用前置条件；断言仍写在 test 内，使读者一眼能看出该案例的行为契约。mock 用在不可控边界（HTTP、时钟、支付）而不是为了检查函数内部调用了几次。

## 失败案例优先

先列出空输入、非法格式、外部超时与重复执行，再写正常路径。自动化脚本最危险的往往不是报错，而是无声地生成错误结果；因此测试也要验证错误信息、输出文件和副作用是否符合预期。

## 自测

给 JSON config loader 写：正确对象、顶层数组、缺失文件、非法 JSON 四个测试。

## Sources

- [pytest 入门文档](https://docs.pytest.org/en/stable/getting-started.html)
- 验证日期：2026-09-05

## 常见误区

- 把脚本一次跑通当成工程正确，忽略环境隔离、输入校验、错误分类和可重复运行。
- 只读 API 名称而不为文件、网络、时间等外部边界写一个可控测试。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
01-pytest Fixtures and Test Doubles (测试夹具与替身)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
