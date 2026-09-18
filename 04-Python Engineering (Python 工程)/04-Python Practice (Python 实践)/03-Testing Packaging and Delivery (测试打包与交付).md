---
status: learning
confidence: low
content_verified: 2026-09-17
---

> [!note] 方法论坐标
> 通用测试证据与发布门禁见 [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md) 和 [Release Verification and Quality Gates (发布验证与质量门禁)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/07-Release%20Verification%20and%20Quality%20Gates%20(发布验证与质量门禁).md)；本篇聚焦 Python 包、wheel 与交付流程。

> [!abstract] 学习定位
> > Python 项目只有在依赖可复现、行为可测试、入口可执行、产物可安装时，才从“本机脚本”升级为可交付工程。

> [!summary] 核心摘要
>
> 用 `pyproject.toml` 描述项目与构建元数据，用锁文件固定解析结果，用测试覆盖稳定契约，用 console script 或 `python -m` 提供入口。交付前必须在干净环境重新安装和运行，避免把本机缓存、工作目录或隐式环境变量当成依赖。

# 项目最小结构

```text
project/
|-- pyproject.toml
|-- README.md
|-- src/
|   `-- app/
|       |-- __init__.py
|       `-- cli.py
`-- tests/
    `-- test_cli.py
```

`src` 布局能减少“因为当前目录恰好在导入路径里，所以测试错误地通过”的概率。

# 测试契约

测试应围绕可观察行为，而不是复制实现步骤：

- 正常输入给出什么输出。
- 边界输入和非法输入如何失败。
- 文件、网络、时间等副作用如何隔离。
- 修复过的缺陷是否有回归测试。

```python
def test_parse_rejects_negative_port() -> None:
    with pytest.raises(ValueError):
        parse_port("-1")
```

# 打包与入口

构建后得到 wheel 和 source distribution。wheel 更接近可直接安装的产物；源码包允许目标环境重新构建。命令行入口应调用一个可单测的函数，不要把全部逻辑堆在 `if __name__ == "__main__"` 中。

# 干净环境验收

```text
创建临时虚拟环境
  -> 从锁文件安装
  -> 运行静态检查与测试
  -> 构建 wheel
  -> 从 wheel 安装
  -> 执行 CLI 冒烟测试
```

不要发布密钥、虚拟环境、缓存、测试输出或仅适用于本机的绝对路径。

# 最小交付清单

- [ ] 新环境可以一次命令安装依赖。
- [ ] 测试不依赖执行顺序和开发机残留文件。
- [ ] CLI 的退出码、标准输出和错误输出有明确契约。
- [ ] README 包含安装、运行、测试和配置示例。
- [ ] 构建产物可在另一个干净环境安装运行。

> [!question]- 自测：先回答再展开
> 1. 为什么“在仓库根目录能 import”不能证明包配置正确？
> 2. 锁文件与版本范围分别解决什么问题？
> 3. 哪些依赖应注入或替换，才能让测试稳定？

> [!info]- 延伸阅读
> - 前置：[01-Runtime Modules Packages and uv (运行环境、模块与包管理)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/01-Runtime%20Modules%20Packages%20and%20uv%20(运行环境、模块与包管理).md)
> - 延伸：[02-Automation Project Template (自动化项目模板)](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/02-Automation%20Project%20Template%20(自动化项目模板).md)



