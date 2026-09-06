---
tags: [language/python, python/cli]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 04-CLI and Configuration (命令行与配置)

> [!abstract] 一句话结论：脚本从“改源码里的路径再运行”升级为工具的第一步，是用 `argparse` 接收参数、提供 `--help` 和明确退出码。

> [!tip] CLI 的用户也包括三个月后的自己
> 命令、输入、输出与失败方式都应能从 `--help` 和错误信息中看懂；把路径、阈值等配置暴露为参数，而不是埋在源码常量里。

```python
from argparse import ArgumentParser
from pathlib import Path

def parse_args():
    parser = ArgumentParser(description="Count lines in a text file")
    parser.add_argument("path", type=Path)
    parser.add_argument("--json", action="store_true", help="emit JSON")
    return parser.parse_args()

def main() -> int:
    args = parse_args()
    if not args.path.is_file():
        print(f"not a file: {args.path}")
        return 2
    print(sum(1 for _ in args.path.open(encoding="utf-8")))
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
```

## 设计清单

- 位置参数放必要输入；可选参数使用 `--long-name`。
- `--help` 必须说明输入、输出、示例和危险副作用。
- `main()` 返回整数退出码，方便 shell/CI 判断成功失败。
- 逻辑放到可测试函数，`argparse` 仅是边界层。

## 30 秒回答

一个可维护 CLI 由参数解析、可测试业务函数和退出码组成。`argparse` 只负责把 shell 输入变成结构化参数；`main()` 编排流程并返回状态，`SystemExit` 把状态交给 shell 或 CI。这样同一逻辑既能被测试，也能被人和自动化调用。

## 自测

1. 哪些参数应做位置参数，哪些应使用 `--option`？
2. 为什么业务函数不应直接读取 `sys.argv` 或调用 `sys.exit()`？
3. 为会覆盖文件的命令设计 `--dry-run` 时，输出中应包含哪些信息？

## 练习

给日志汇总工具加入 `--output`、`--min-level`、`--dry-run`，并测试非法路径返回非零状态。

## Sources

- [argparse 官方文档](https://docs.python.org/3/library/argparse.html)
- 验证日期：2026-09-05

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 04-CLI and Configuration (命令行与配置) 的问题、核心机制与边界。

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
