---
status: learning
confidence: low
content_verified: 2026-09-17
verified: 2026-10-18
review_stage: learn
review_due: 2026-10-18
previous_review_due: 2026-09-28
tags: [language/python, python/testing]
---

> [!note] 方法论坐标
> 测试分层与证据标准见 [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)；本篇聚焦 pytest fixture、参数化与 Python 测试替身。

> [!abstract] 学习定位
> pytest 测试行为而不是打印结果；对文件、时间、网络等外部依赖，用临时目录或 test double 控制输入。

> [!summary] 核心摘要
>
> fixture 负责可复用的准备与清理；临时目录隔离文件副作用；mock、fake 等 test double 替代不可控依赖，断言最终行为而非实现过程。

# 命令与规则

```text
python -m pytest
python -m pytest -q
```

- 测试命名 `test_*.py`；每个 test 只验证一个可读行为。
- 使用 `tmp_path`，不要把真实 Vault、下载目录或用户文件当测试夹具。
- HTTP 层使用 mock/fake 或本地测试 server，避免单元测试依赖互联网。
- 失败断言要包含实际值和期望值，便于定位。

# Fixture 只负责准备条件

```python
def test_loader_rejects_bad_json(tmp_path):
    config = tmp_path / "config.json"
    config.write_text("{bad", encoding="utf-8")
    with pytest.raises(ValueError, match="invalid"):
        load_config(config)
```

`tmp_path` 为每个测试提供独立临时目录，不会污染真实文件。fixture 应只建立可复用前置条件；断言仍写在 test 内，使读者一眼能看出该案例的行为契约。mock 用在不可控边界（HTTP、时钟、支付）而不是为了检查函数内部调用了几次。

# Arrange、Act、Assert

测试先准备最小前提（Arrange），执行一个行为（Act），再断言可观察结果（Assert）。测试名描述场景与结果，例如 `test_loader_rejects_array_root`，失败时不用阅读实现也能知道契约。

参数化适合相同规则的输入表；不要把互不相关的行为塞进一个巨大参数表。测试正常值、边界值和越界值，而不是只复制文档示例。

```python
import pytest

@pytest.mark.parametrize(
    ("text", "expected"),
    [("1", 1), ("65535", 65535)],
)
def test_parse_port_accepts_valid_range(text, expected):
    assert parse_port(text) == expected
```

# Fixture 生命周期

fixture 表达测试依赖及清理。默认 function scope 隔离最好；扩大到 module/session 会提高速度，也会增加共享状态和顺序依赖。数据库、临时进程等资源使用 `yield` fixture，在 yield 后清理。

fixture 不应隐藏断言或创建与当前测试无关的大量对象。若读者必须跳转多层 fixture 才能知道输入，测试已经失去文档价值。

# Dummy、Stub、Fake、Mock

- dummy 只填参数，不参与行为；
- stub 返回预设响应；
- fake 有可工作的简化实现，如内存 repository；
- mock/spy 记录交互，用于确实属于契约的调用。

优先断言最终状态或返回值。只有“是否向支付网关发送一次带幂等键的请求”这种交互本身就是契约时，才断言调用。过度 mock 内部函数会让重构在行为没变时仍打碎测试。

`monkeypatch` 可替换环境变量、属性和函数，但补丁应作用于被测模块查找名称的位置。更清晰的设计是把时钟、HTTP client、文件系统边界作为依赖传入。

# 可重复与可诊断

测试不能依赖互联网、真实用户目录、执行顺序或当前时间。随机测试记录 seed，时间使用 fake clock，文件使用 `tmp_path`。失败时保留最小输入、预期、实际和关键上下文，避免只断言 `result is True`。

练习：为批量文件转换器写参数化字段校验、临时目录集成测试、写入失败 fake 和“重复运行不产生重复副作用”的测试。
# 失败案例优先

先列出空输入、非法格式、外部超时与重复执行，再写正常路径。自动化脚本最危险的往往不是报错，而是无声地生成错误结果；因此测试也要验证错误信息、输出文件和副作用是否符合预期。

> [!question]- 自测：先回答再展开
> 给 JSON config loader 写：正确对象、顶层数组、缺失文件、非法 JSON 四个测试。

# 官方参考

- [pytest 入门文档](https://docs.pytest.org/en/stable/getting-started.html)
- 验证日期：2026-09-05

