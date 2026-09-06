---
tags: [language/python, python/cli]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

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

# 设计清单

- 位置参数放必要输入；可选参数使用 `--long-name`。
- `--help` 必须说明输入、输出、示例和危险副作用。
- `main()` 返回整数退出码，方便 shell/CI 判断成功失败。
- 逻辑放到可测试函数，`argparse` 仅是边界层。

# 30 秒回答

一个可维护 CLI 由参数解析、可测试业务函数和退出码组成。`argparse` 只负责把 shell 输入变成结构化参数；`main()` 编排流程并返回状态，`SystemExit` 把状态交给 shell 或 CI。这样同一逻辑既能被测试，也能被人和自动化调用。

# 自测

1. 哪些参数应做位置参数，哪些应使用 `--option`？
2. 为什么业务函数不应直接读取 `sys.argv` 或调用 `sys.exit()`？
3. 为会覆盖文件的命令设计 `--dry-run` 时，输出中应包含哪些信息？

# 练习

给日志汇总工具加入 `--output`、`--min-level`、`--dry-run`，并测试非法路径返回非零状态。

# Sources

- [argparse 官方文档](https://docs.python.org/3/library/argparse.html)
- 验证日期：2026-09-05

# 从零建立模型

本页主题是 **04-CLI and Configuration (命令行与配置)**。Python 对初学者最重要的是区分“值、名称、对象”和“副作用”。函数拿到什么输入、返回什么值、会读写哪些文件/网络资源，应该从签名和小例子中一眼可见。先写可读的同步代码，再为真实 I/O 或批量任务引入并发。

# 最小实践

把本页概念做成一个可运行函数或 CLI：准备正常、空值和错误输入各一份；打印或断言结果。若涉及文件和网络，使用临时目录或 test double，不能依赖本机隐式状态。

# 工程检查点

Python 的动态性不等于不需要契约。公共函数应写类型标注、异常语义和示例；密钥、绝对路径、真实生产数据都不应写死在示例里。

# 常见误区

- 把脚本一次跑通当成工程正确，忽略环境隔离、输入校验、错误分类和可重复运行。
- 只读 API 名称而不为文件、网络、时间等外部边界写一个可控测试。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **04-CLI and Configuration (命令行与配置)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
