---
tags: [vault/navigation, tools/engineering]
status: seed
verified: 2026-09-05
---

# Tools MOC — 工程开发工具导航

> 工具服务于「可复现的构建、可定位的调试、可度量的性能」；版本与命令类信息需随官方文档核对（NEEDS_VERIFY）。

## 主题分组
- Git：[[08-Development Tools (工程开发工具)/01 · Git (版本控制)/01a-Core Concepts：Working Tree, Index, HEAD (三区模型) ⭐|三区模型]] · [[08-Development Tools (工程开发工具)/01 · Git (版本控制)/01b-Branch Model & Merge Strategies (分支策略与合并)/01b1-merge vs rebase vs cherry-pick (三种合并对比) ⭐|合并三兄弟]] · [[08-Development Tools (工程开发工具)/01 · Git (版本控制)/01d-reset vs revert vs restore (撤销三兄弟) ⭐|撤销三兄弟]] · [[08-Development Tools (工程开发工具)/01 · Git (版本控制)/01f-CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线) ⭐|CI/CD]]
- CMake：[[08-Development Tools (工程开发工具)/02 · CMake (构建系统)/02a-CMakeLists․txt Structure (项目结构模板) ⭐|项目结构]] · [[08-Development Tools (工程开发工具)/02 · CMake (构建系统)/02b-target_link_libraries & include_directories (依赖管理) ⭐|依赖管理]] · [[08-Development Tools (工程开发工具)/02 · CMake (构建系统)/02c-Build Types：Debug, Release, RelWithDebInfo (构建类型) ⭐|构建类型]]
- 调试与性能：[[08-Development Tools (工程开发工具)/03 · Debug & Profiling (调试与性能分析)/03a-GDB Essentials：breakpoint, watch, backtrace (GDB核心用法) ⭐|GDB]] · [[08-Development Tools (工程开发工具)/03 · Debug & Profiling (调试与性能分析)/03d-AddressSanitizer & UBSan (编译期检测工具) ⭐|Sanitizer]] · [[08-Development Tools (工程开发工具)/03 · Debug & Profiling (调试与性能分析)/03e-perf：CPU Profiling & Flamegraph (性能火焰图)|perf 火焰图]]
- 其他工具：[[08-Development Tools (工程开发工具)/04 · Other Tools (其他工具速查)/04b-Docker Basics：Image & Container (Docker基础)|Docker 基础]] · [[08-Development Tools (工程开发工具)/04 · Other Tools (其他工具速查)/04c-Shell Tools：grep, sed, awk, tmux (Shell效率工具)|Shell 工具]]

## 学习顺序
1. Git 三区与分支 → 2. CMake 项目结构 → 3. GDB/Sanitizer 定位 → 4. perf 度量 → 5. Docker/Shell 提效。

## 与 C++ / 项目入口
- 构建与调试 → [[03-C++ Programming (编程语言)/06-Engineering Practice (工程实践)/07-CMake Build System (CMake构建系统) ⭐|CMake 构建]] · [[03-C++ Programming (编程语言)/06-Engineering Practice (工程实践)/05-Debugging gdb & Sanitizers (调试工具) ⭐|gdb 与 Sanitizer]]
- 项目 → [[12-Backend Projects (后端项目)/00-Project Map|项目地图]]
