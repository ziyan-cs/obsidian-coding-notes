---
tags: [language/python, python/files]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Path, Files, JSON & CSV — 脚本的真实输入输出

> **一句话结论**：用 `pathlib` 表达路径，用 context manager 保证关闭文件，用 `json`/`csv` 做显式的输入校验和输出格式。

```python
from pathlib import Path
import json

def load_config(path: Path) -> dict:
    with path.open(encoding="utf-8") as f:
        data = json.load(f)
    if not isinstance(data, dict):
        raise ValueError("config root must be an object")
    return data
```

## 规则

- 路径使用 `Path` 和 `/` 拼接，不手写 `\\` 或 `/`。
- `with path.open(...)` 确保异常时也关闭文件。
- 文本显式写 `encoding="utf-8"`；CSV 使用 `newline=""`。
- 不要直接覆盖重要输出：先写临时文件，验证后替换；批处理先提供 `--dry-run`。

## 常见坑

- 当前工作目录（CWD）不等于脚本所在目录；CLI 应接收明确路径。
- JSON 可解析不代表业务数据有效；仍需检查键、类型和范围。
- 一次 `read_text()` 读取大日志可能耗尽内存；大文件按行迭代。

## 练习

写 `summarize_logs.py <directory>`：递归读取 `.log`，统计每级日志数量，输出 JSON。先实现 `--dry-run`，再实现写文件。
