---
study_stage: backlog
review_due:
---

> [!abstract] 本篇要点
> Python 项目只有在依赖可复现、行为可测试、入口可执行、产物可安装时，才从“本机脚本”升级为可交付工程。

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
import pytest

def parse_port(text: str) -> int:
    value = int(text)
    if not 1 <= value <= 65535:
        raise ValueError("port out of range")
    return value

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

**参考资料：** 前置：[01-Runtime Modules Packages and uv (运行环境、模块与包管理)](../01-Python%20Foundations%20%28Python%20%E5%9F%BA%E7%A1%80%29/01-Runtime%20Modules%20Packages%20and%20uv%20%28%E8%BF%90%E8%A1%8C%E7%8E%AF%E5%A2%83%E3%80%81%E6%A8%A1%E5%9D%97%E4%B8%8E%E5%8C%85%E7%AE%A1%E7%90%86%29.md) · 延伸：[02-Automation Project Template (自动化项目模板)](02-Automation%20Project%20Template%20%28%E8%87%AA%E5%8A%A8%E5%8C%96%E9%A1%B9%E7%9B%AE%E6%A8%A1%E6%9D%BF%29.md)

> [!info]- 关联笔记
> 通用测试证据与发布门禁见 [Testing Strategy and Evidence (测试策略与证据)](../../02-Engineering%20Fundamentals%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%29/03-Verification%20and%20Diagnostics%20%28%E9%AA%8C%E8%AF%81%E4%B8%8E%E8%AF%8A%E6%96%AD%29/01-Testing%20Strategy%20and%20Evidence%20%28%E6%B5%8B%E8%AF%95%E7%AD%96%E7%95%A5%E4%B8%8E%E8%AF%81%E6%8D%AE%29.md) 和 [Release Verification and Quality Gates (发布验证与质量门禁)](../../02-Engineering%20Fundamentals%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%29/03-Verification%20and%20Diagnostics%20%28%E9%AA%8C%E8%AF%81%E4%B8%8E%E8%AF%8A%E6%96%AD%29/07-Release%20Verification%20and%20Quality%20Gates%20%28%E5%8F%91%E5%B8%83%E9%AA%8C%E8%AF%81%E4%B8%8E%E8%B4%A8%E9%87%8F%E9%97%A8%E7%A6%81%29.md)；本篇聚焦 Python 包、wheel 与交付流程。

# 考察题目

## 1. 动手题

将项目构建为可安装包，在全新虚拟环境安装后运行 CLI 和测试资源；从非源码目录执行。

> [!note]- 验收要点
> 入口、依赖和数据文件都可用，不依赖源码路径或开发环境变量。

## 2. 机制追问

本地 `python script.py` 能运行，为什么仍可能不是可交付包？

> [!note]- 参考答案
> 需验证依赖声明、包入口、安装方式和资源文件在干净环境可用；测试从安装后运行而非依赖源码目录与偶然的 PYTHONPATH。
