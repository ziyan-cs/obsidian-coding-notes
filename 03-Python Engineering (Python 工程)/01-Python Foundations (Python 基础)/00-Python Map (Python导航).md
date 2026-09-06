---
tags: [language/python, career/backend]
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 00-Python Map (Python导航)

> [!abstract] 一句话结论：Python 的第一价值是让你迅速把重复工作变成可靠工具：处理文件和日志、验证接口、生成数据、写测试。

## 学习边界

```text
环境与基础 → 文件 / 配置 / 异常 → 测试与质量 → HTTP / 数据 / 异步
                                                ↓
                                      可交付的自动化工具与项目辅助
```

暂不以 Web 框架、爬虫规模化或 AI 训练为目标。先掌握标准库和小脚本，再按项目需要扩展。

## 环境基线（Windows PowerShell）

```powershell
py --version
py -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
```

当前学习基线为 Python 3.14。每个项目使用独立 `.venv`；永远用 `python -m pip` 或统一的项目工具，避免把包装到另一个解释器。依赖声明、lockfile 与干净环境验证见 `01-Environment Packages and uv`。

## 每个脚本项目的最小结构

```text
tool-name/
├── .venv/          # 不纳入版本控制
├── src/tool_name/
├── tests/
├── pyproject.toml  # 项目和工具配置（后续学习）
└── README.md
```

## 首月交付物

- [ ] 一个命令行文件整理器。
- [ ] 一个日志/CSV 汇总器。
- [ ] 一个 HTTP 接口健康检查工具，带 pytest 测试、超时与失败报告。

## Sources

- [Python venv 官方教程](https://docs.python.org/3/tutorial/venv.html)
- [Python Windows 下载页](https://www.python.org/downloads/windows/)
- 验证日期：2026-09-06
