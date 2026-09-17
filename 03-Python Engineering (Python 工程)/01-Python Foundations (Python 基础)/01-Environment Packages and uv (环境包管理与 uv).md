---
status: stable
confidence: high
verified: 2026-09-17
review_due: 2026-09-21
---

> [!abstract] 一句话结论：项目依赖必须隔离、可复现、可声明；不要把“电脑上能跑”误当成项目环境正确。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：用项目级虚拟环境隔离解释器与依赖；用 `pyproject.toml` 声明需求、锁文件固定解析结果；在新机器或 CI 中从零安装验证复现。

# 最小工作流

1. 每个项目使用独立虚拟环境（virtual environment）。
2. 在项目元数据中声明直接依赖与 Python 版本约束。
3. 用 lockfile 锁定可复现的完整依赖集合。
4. 在干净环境中安装并运行测试，确认没有隐式全局依赖。

`uv` 是一个用于创建环境、解析依赖和运行命令的现代 Python 工具；具体命令随版本演进，以官方文档为准。理解的重点是“声明、锁定、同步”三件事，而不是背命令。

# 可复现的 uv 最小命令

```bash
uv init my-service
cd my-service
uv add httpx
uv run pytest
uv lock --check
uv sync --locked
```

`uv add` 更新项目元数据与 lockfile；`uv run` 会在默认配置下确保项目环境已锁定、同步后再运行命令。CI 中用 `uv lock --check` 或命令的 `--locked` 选项拒绝过期 lockfile，避免构建时悄悄重新解析版本。`uv sync` 默认做 exact sync，未列在 lockfile 的额外包可能被移除；不要把它当成无副作用的“安装一下”。

# 官方资料

- [uv：项目结构与 lockfile](https://docs.astral.sh/uv/concepts/projects/layout/)
- [uv：locking 与 syncing](https://docs.astral.sh/uv/concepts/projects/sync/)
- 核验日期：2026-09-06

# 从空目录到可复现项目

```text
project metadata -> resolve dependencies -> lockfile
       |                                      |
       +----------> isolated .venv <----------+
                              |
                           run tests
```

元数据表达“我直接依赖什么、支持哪个 Python”；lockfile 表达“本次解析后实际安装哪一组精确版本”；虚拟环境隔离项目解释器与包。三者各解决一个问题，缺任何一个都会让“换电脑能否重现”变得不确定。

# 验收动作

在干净目录克隆项目后，只使用项目文档提供的命令创建环境、同步依赖并运行测试。若必须先在本机手工安装某个包，说明依赖声明或 lockfile 还不完整。不要把 `.venv` 提交到版本库。

> [!warning]- 易错点
> - 把 `pip install` 过的全局环境当作项目依赖清单。
> - 只提交源码，不提交依赖声明或锁文件。
> - 将 `.venv` 作为源码提交；环境应可重建，而非复制。

> [!question]- 自测：先回答再展开
> 1. 为什么需要 lockfile，而只写依赖范围不够？
> 2. 新同学拿到项目后，怎样验证环境可复现？

> [!info]- 延伸阅读
> - 下一步：[02-Objects Types and Functions (对象类型与函数)](/03-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/02-Objects%20Types%20and%20Functions%20(对象类型与函数).md)
