---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 一句话结论：工具不是待背的命令清单；它们分别服务于可追溯协作、可复现构建、可定位故障和可度量性能。

# 学习顺序

1. **Git Collaboration（Git 协作）**：工作区、暂存区、提交、分支、合并与可恢复撤销。
2. **Build and Dependencies（构建与依赖）**：CMake target 思维、依赖边界、构建类型与包管理。
3. **Debugging and Performance（调试与性能）**：GDB、Sanitizer、core dump、perf 与火焰图。
4. **Tool Practice（工具实践）**：Shell、Docker、Make、代码质量与发布前检查。

# 每次动手的闭环

1. 用最小可运行项目复现问题。
2. 先保存证据：命令、版本、输入、日志或 profile。
3. 修改一个变量后再次验证，避免把相关性当因果。
4. 把“触发条件—定位步骤—修复—预防”写回错误记录。

# 本模块出口

能独立从一个 C++ 或 Go 项目的构建失败、内存错误、性能回退中拿到证据并提出可验证的修复。
