---
tags: [language/python, python/files]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 03-Path Files and Serialization (路径文件与序列化)

> [!abstract] 一句话结论：用 `pathlib` 表达路径，用 context manager 保证关闭文件，用 `json`/`csv` 做显式的输入校验和输出格式。

> [!warning] 写文件是有副作用的操作
> 只要脚本会覆盖、移动或批量生成文件，就应让目标路径可见、先提供 `--dry-run`，并在真正替换前校验输出。不要把“能写入”误当作“写入正确”。

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

## 30 秒回答

文件脚本的边界包括：明确的输入路径、显式编码、资源关闭、数据校验和可控的输出副作用。`Path` 让路径操作跨平台且可读，`with` 确保文件在异常时关闭；JSON 或 CSV 的“能解析”不等于符合业务 schema，仍要检查字段与范围。

## 自测

1. 为什么 CLI 不能假设当前工作目录就是脚本目录？
2. 处理 10 GB 日志时，`read_text()` 与逐行迭代有什么本质差别？
3. 怎样设计一个“写前预览、写后可验证”的批量重命名工具？

## 练习

写 `summarize_logs.py <directory>`：递归读取 `.log`，统计每级日志数量，输出 JSON。先实现 `--dry-run`，再实现写文件。
