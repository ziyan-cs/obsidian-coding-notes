---
study_stage: learn
review_due: 2026-09-23
---

> [!abstract] 学习定位
> 项目依赖必须隔离、可复现、可声明；不要把“电脑上能跑”误当成项目环境正确。

> [!summary] 核心摘要
>
> 用项目级虚拟环境隔离解释器与依赖；用 `pyproject.toml` 声明需求、锁文件固定解析结果；在新机器或 CI 中从零安装验证复现。

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
uv add --dev pytest
uv run --locked pytest
uv lock --check
uv sync --locked
```

`uv add` 更新项目元数据与 lockfile；`uv run` 会在默认配置下确保项目环境已锁定、同步后再运行命令。CI 中用 `uv lock --check` 或命令的 `--locked` 选项拒绝过期 lockfile，避免构建时悄悄重新解析版本。`uv sync` 默认做 exact sync，未列在 lockfile 的额外包可能被移除；不要把它当成无副作用的“安装一下”。

# 解释器与模块边界

先确认“正在运行哪个 Python”。虚拟环境改变命令解析和导入路径，IDE、终端与 CI 可能选择不同解释器。

```python
import sys
print(sys.executable)
print(sys.version)
print(sys.path)
```

`sys.executable` 用于核对解释器位置；`sys.path` 是模块查找路径，不应通过在代码里临时 append 机器绝对路径来修复包结构。

一个 `.py` 文件是 module；含多个 module 的可导入目录是 package。导入会执行模块顶层代码并缓存到 `sys.modules`，因此顶层只放常量、定义和轻量初始化，不在 import 时发网络请求或启动任务。

```text
src/example_app/
├─ __init__.py
├─ cli.py
└─ service.py
```

配置并安装 `src/` 包后，从项目根目录运行 `python -m example_app.cli`，让解释器按模块语义定位入口。直接运行包内部文件会改变 import 上下文，容易出现“脚本能跑，安装后不能跑”的问题。模块间优先使用明确的绝对导入；循环导入通常说明职责或共享类型放错层。

# 依赖声明与解析

`pyproject.toml` 中只声明项目直接依赖和兼容范围；解析器会计算传递依赖，lockfile 再记录本次完整解析结果。不要手工把 `pip freeze` 的所有包都当成直接依赖，否则无法区分项目意图与偶然环境状态。

运行依赖是用户执行软件所需；开发依赖包括测试、类型检查和构建工具。可选依赖用于真正可选的功能，不应用来掩盖环境组合没有经过测试。

升级依赖时阅读 release notes，并在锁文件变化后运行测试和构建。安全更新仍可能改变行为；“最新”不是跳过验证的理由。

# 从空目录到可复现项目

1. 创建项目与独立环境，打印 `sys.executable`，确认运行的是项目解释器。
2. 将运行依赖与开发依赖分别声明，检查 `pyproject.toml` 和 `uv.lock` 的变化。
3. 配置可安装的 `src/` 包，再用 `python -m package.module` 运行；仅有目录结构而未安装包时，不能假设模块可导入。
4. 删除虚拟环境，在干净环境运行 `uv sync --locked`、测试和 CLI；若还需手工安装隐藏依赖，说明项目尚不可复现。

`uv sync` 默认精确同步，可能移除 lockfile 之外的包；`uv run` 默认保持必要依赖同步，但不是同一种清理模式。检查锁文件是否过期用 `uv lock --check`。不要提交 `.venv`，它应能由仓库文件重建。

官方资料：[uv 项目布局](https://docs.astral.sh/uv/concepts/projects/layout/) · [锁定与同步](https://docs.astral.sh/uv/concepts/projects/sync/)

> [!info]- 延伸阅读
> - 下一步：[02-Objects Names Mutability and Copying (对象、名称、可变性与复制)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/02-Objects%20Names%20Mutability%20and%20Copying%20(对象、名称、可变性与复制).md)


