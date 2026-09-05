---
tags: [language/python, python/cli]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# CLI with argparse — 让脚本可复用而非一次性

> **一句话结论**：脚本从“改源码里的路径再运行”升级为工具的第一步，是用 `argparse` 接收参数、提供 `--help` 和明确退出码。

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

## 练习

给日志汇总工具加入 `--output`、`--min-level`、`--dry-run`，并测试非法路径返回非零状态。

## Sources

- [argparse 官方文档](https://docs.python.org/3/library/argparse.html)
- 验证日期：2026-09-05
