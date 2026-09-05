---
tags: [language/python, python/testing]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# pytest & Test Doubles — 把脚本变成可维护工具

> **一句话结论**：pytest 测试行为而不是打印结果；对文件、时间、网络等外部依赖，用临时目录或 test double 控制输入。

```python
from pathlib import Path

def count_nonempty(path: Path) -> int:
    return sum(bool(line.strip()) for line in path.read_text(encoding="utf-8").splitlines())

def test_count_nonempty(tmp_path: Path):
    path = tmp_path / "sample.txt"
    path.write_text("a\n\n b \n", encoding="utf-8")
    assert count_nonempty(path) == 2
```

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
