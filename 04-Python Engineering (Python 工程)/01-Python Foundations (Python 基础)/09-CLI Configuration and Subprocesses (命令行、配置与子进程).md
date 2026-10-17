---
study_stage: learn
review_due: 2026-11-25
tags: [language/python, python/cli]
---

> [!abstract] 学习定位
> 脚本从“改源码里的路径再运行”升级为工具的第一步，是用 `argparse` 接收参数、提供 `--help` 和明确退出码。

> [!tip] CLI 的用户也包括三个月后的自己
> 命令、输入、输出与失败方式都应能从 `--help` 和错误信息中看懂；把路径、阈值等配置暴露为参数，而不是埋在源码常量里。

```python
import argparse
import json
import sys
from pathlib import Path

def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Count lines in a text file")
    parser.add_argument("path", type=Path)
    parser.add_argument("--json", action="store_true", help="emit JSON")
    return parser.parse_args(argv)

def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    try:
        with args.path.open(encoding="utf-8") as stream:
            count = sum(1 for _ in stream)
    except OSError as exc:
        print(f"cannot read {args.path}: {exc.strerror}", file=sys.stderr)
        return 1
    except UnicodeError:
        print(f"not valid UTF-8: {args.path}", file=sys.stderr)
        return 1
    if args.json:
        print(json.dumps({"lines": count}))
    else:
        print(count)
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
```

# 配置来源与优先级

配置来源需要稳定、可解释的覆盖顺序，例如：命令行参数 > 环境变量 > 配置文件 > 安全默认值。启动时将它们解析成一个经过验证的配置对象，业务代码不应到处读取 `os.environ`。

密钥适合由环境或秘密系统注入，但不要在 `--help`、异常或日志中打印。布尔环境变量不能直接用 `bool(text)`，因为 `bool("false")` 仍为真；应显式解析允许值并拒绝拼写错误。

配置失败应发生在启动阶段，信息指出来源和字段，但隐藏敏感值。对最终生效配置可输出脱敏摘要，方便判断“为什么本机和 CI 行为不同”。

# stdout、stderr 与退出码

stdout 是正常机器可消费结果，stderr 是诊断信息；日志默认写 stderr，避免破坏管道中的 JSON/CSV 输出。退出码 `0` 表示成功，非零表示失败；可为用法错误、输入错误和外部服务失败定义少量稳定类别，不要为每种异常随意编号。

```python
import sys

def main() -> int:
    try:
        result = run(parse_args())
    except UsageError as exc:
        print(f"usage error: {exc}", file=sys.stderr)
        return 2
    except RuntimeError as exc:
        print(f"failed: {exc}", file=sys.stderr)
        return 1
    print(result)
    return 0
```

库函数不要调用 `sys.exit()`；让异常或返回值到达应用入口，再统一映射为用户信息和退出码。

# 安全调用子进程

优先用 Python 库完成任务；确需启动外部程序时，使用参数列表、`check=True`、timeout 和受控环境，不拼接 shell 字符串。

```python
from pathlib import Path
import subprocess

def probe(repository: Path) -> str:
    completed = subprocess.run(
        ["git", "status", "--short"],
        cwd=repository,
        text=True,
        capture_output=True,
        check=True,
        timeout=10,
    )
    return completed.stdout
```

参数列表绕过 shell 解析，可避免空格、引号和大部分注入问题。只有确需管道、重定向或 shell builtin 时才考虑 `shell=True`，并且不能把不可信输入拼入命令。

子进程可能返回非零、超时、输出无限增长或找不到可执行文件。捕获输出适合小结果；大量输出应流式读取或写入文件。timeout 后还要确认子进程及其子进程是否被终止，跨平台行为需在目标系统验证。

# 幂等与 dry-run

会修改文件、数据库或远端系统的 CLI 应支持 dry-run，展示目标、数量和关键差异。dry-run 与真实执行必须共享同一规划逻辑，不能维护两套逐渐分叉的实现。

对部分成功的批处理，输出成功、跳过、失败与可重试清单；重复执行不应再次破坏已经完成的项目。危险操作增加明确确认或 `--force`，但自动化环境不能依赖交互式提示。

# 动手验证

实现批量文件转换 CLI：配置可由文件、环境变量和命令行覆盖；`--dry-run` 展示计划；真实运行通过临时文件安全替换。验证正常结果只写 stdout，非法参数和运行失败写 stderr 并返回稳定的非零退出码；子进程 timeout 不留下仍在运行的直接子进程。若工具会创建子进程树，还需针对目标操作系统验证子进程树的清理方式。

官方参考：[argparse](https://docs.python.org/3/library/argparse.html) · [subprocess](https://docs.python.org/3/library/subprocess.html)。
