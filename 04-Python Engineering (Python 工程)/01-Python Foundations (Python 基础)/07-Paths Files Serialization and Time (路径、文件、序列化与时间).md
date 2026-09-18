---
status: learning
confidence: low
content_verified: 2026-09-17
verified: 2026-10-14
review_stage: learn
review_due: 2026-10-14
previous_review_due: 2026-09-23
tags: [language/python, python/files]
---

> [!abstract] 学习定位
> 用 `pathlib` 表达路径，用 context manager 保证关闭文件，用 `json`/`csv` 做显式的输入校验和输出格式。

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

# 路径解析与目录边界

相对路径相对于当前工作目录（CWD），而不是源码文件位置。CLI 应从参数或配置接收输入根目录；随包发布的只读资源使用 `importlib.resources`，不要依赖 `__file__` 拼出开发机目录结构。

处理用户提供的路径时，先明确允许访问的根目录。`resolve()` 后检查目标仍位于允许根下，防止 `..` 或符号链接越界。检查和使用之间仍可能发生 TOCTOU 竞争；安全敏感操作应使用操作系统提供的目录句柄、权限或隔离边界，而不是只做字符串前缀判断。

# 文本、字节与数据格式

文本是 Unicode 字符序列，文件和网络上传输的是 bytes；编码负责两者转换。打开文本文件显式指定 `encoding="utf-8"` 和合适的 `errors` 策略。遇到未知编码时应识别来源或拒绝，不能随意忽略损坏字节。

JSON 只有 object、array、string、number、boolean 和 null 等数据类型。解析成功只证明语法合法，业务仍需检查必填字段、类型、范围、枚举和未知字段。CSV 没有统一 schema，数字、时间和空值默认都是文本，需要显式转换。

日期时间应区分 naive 与 timezone-aware。跨系统存储和传输优先使用带时区的 ISO 8601 时间或 UTC 时间戳，并保留业务时区用于展示。不要把本地墙上时间直接当作全局顺序；夏令时切换会产生重复或不存在的本地时间。

# 原子写入与并发边界

重要文件不要直接覆盖。先在同一文件系统的临时文件写入、flush，按需要同步到磁盘，验证后使用 `Path.replace()` 原子替换目标。原子 rename 只保证读者不会看到半个文件，不自动保证断电持久性，也不能协调多个 writer 的业务冲突。

```python
from pathlib import Path
from tempfile import NamedTemporaryFile

def replace_text(target: Path, content: str) -> None:
    target.parent.mkdir(parents=True, exist_ok=True)
    with NamedTemporaryFile(
        "w", encoding="utf-8", dir=target.parent, delete=False
    ) as stream:
        stream.write(content)
        temporary = Path(stream.name)
    temporary.replace(target)
```

真实工具还要在异常时清理临时文件，并按风险决定备份、权限继承和 fsync。批量修改先生成变更计划，检测重名和越界，再执行；中途失败时报告已完成和未完成项目，使重跑保持幂等。

# 大文件与流式处理

`read_text()` 会把完整文件放入内存。大日志按行迭代，二进制大对象按块读取。流式只限制输入缓冲；若把每个唯一用户都放入 dict，聚合状态仍可能耗尽内存。此时需要限制维度、分区、外部排序或数据库。

练习：实现配置文件更新器。它校验 JSON schema，`--dry-run` 展示差异，写临时文件后替换，并通过故意制造非法 JSON、目标目录不存在和写入中断验证失败路径。
> [!warning]- 易错点
> - 当前工作目录（CWD）不等于脚本所在目录；CLI 应接收明确路径。
> - JSON 可解析不代表业务数据有效；仍需检查键、类型和范围。
> - 一次 `read_text()` 读取大日志可能耗尽内存；大文件按行迭代。
>

> [!summary] 核心摘要
>
> 文件脚本的边界包括：明确的输入路径、显式编码、资源关闭、数据校验和可控的输出副作用。`Path` 让路径操作跨平台且可读，`with` 确保文件在异常时关闭；JSON 或 CSV 的“能解析”不等于符合业务 schema，仍要检查字段与范围。

> [!question]- 自测：先回答再展开
> 1. 为什么 CLI 不能假设当前工作目录就是脚本目录？
> 2. 处理 10 GB 日志时，`read_text()` 与逐行迭代有什么本质差别？
> 3. 怎样设计一个“写前预览、写后可验证”的批量重命名工具？

# 练习

写 `summarize_logs.py <directory>`：递归读取 `.log`，统计每级日志数量，输出 JSON。先实现 `--dry-run`，再实现写文件。

