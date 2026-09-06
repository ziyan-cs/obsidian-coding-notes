---
review_due: 2026-09-23
tags: [language/python, python/files]
status: seed
confidence: 1
verified: 2026-09-05
---

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

# 规则

- 路径使用 `Path` 和 `/` 拼接，不手写 `\\` 或 `/`。
- `with path.open(...)` 确保异常时也关闭文件。
- 文本显式写 `encoding="utf-8"`；CSV 使用 `newline=""`。
- 不要直接覆盖重要输出：先写临时文件，验证后替换；批处理先提供 `--dry-run`。

# 常见坑

- 当前工作目录（CWD）不等于脚本所在目录；CLI 应接收明确路径。
- JSON 可解析不代表业务数据有效；仍需检查键、类型和范围。
- 一次 `read_text()` 读取大日志可能耗尽内存；大文件按行迭代。

# 30 秒回答

文件脚本的边界包括：明确的输入路径、显式编码、资源关闭、数据校验和可控的输出副作用。`Path` 让路径操作跨平台且可读，`with` 确保文件在异常时关闭；JSON 或 CSV 的“能解析”不等于符合业务 schema，仍要检查字段与范围。

# 自测

1. 为什么 CLI 不能假设当前工作目录就是脚本目录？
2. 处理 10 GB 日志时，`read_text()` 与逐行迭代有什么本质差别？
3. 怎样设计一个“写前预览、写后可验证”的批量重命名工具？

# 练习

写 `summarize_logs.py <directory>`：递归读取 `.log`，统计每级日志数量，输出 JSON。先实现 `--dry-run`，再实现写文件。

# 从零建立模型

本页主题是 **03-Path Files and Serialization (路径文件与序列化)**。Python 对初学者最重要的是区分“值、名称、对象”和“副作用”。函数拿到什么输入、返回什么值、会读写哪些文件/网络资源，应该从签名和小例子中一眼可见。先写可读的同步代码，再为真实 I/O 或批量任务引入并发。

# 最小实践

把本页概念做成一个可运行函数或 CLI：准备正常、空值和错误输入各一份；打印或断言结果。若涉及文件和网络，使用临时目录或 test double，不能依赖本机隐式状态。

# 工程检查点

Python 的动态性不等于不需要契约。公共函数应写类型标注、异常语义和示例；密钥、绝对路径、真实生产数据都不应写死在示例里。

# 常见误区

- 把脚本一次跑通当成工程正确，忽略环境隔离、输入校验、错误分类和可重复运行。
- 只读 API 名称而不为文件、网络、时间等外部边界写一个可控测试。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Path Files and Serialization (路径文件与序列化)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
