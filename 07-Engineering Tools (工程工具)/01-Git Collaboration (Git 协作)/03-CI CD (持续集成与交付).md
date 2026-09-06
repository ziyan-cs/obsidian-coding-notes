---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

# 30 秒回答

**核心结论**：学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

# CI CD for C Plus Plus (C Plus Plus 持续集成)

> [!note] 本节重点：核心考点：GitHub Actions / GitLab CI 配置、C++ 项目 CI 流水线、自动化测试与部署

# C++ CI/CD 流水线

## GitHub Actions 配置

```yaml
name: C++ CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  build-and-test:
    strategy:
      matrix:
        compiler: [gcc-12, clang-16]
        build_type: [Debug, Release]
      fail-fast: false  # 一个挂了不中断其他的

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          cmake ninja-build ccache clang-16 \
          libgtest-dev libbenchmark-dev \
          libspdlog-dev libfmt-dev

    - name: Configure CMake
      run: |
        cmake -B build \
          -G Ninja \
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
          -DCMAKE_CXX_COMPILER=${{ matrix.compiler }} \
          -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

    - name: Build
      run: cmake --build build --parallel $(nproc)

    - name: Run tests
      run: cd build && ctest --output-on-failure -j$(nproc)

    - name: Upload test results
      if: failure()
      uses: actions/upload-artifact@v4
      with:
        name: test-output-${{ matrix.compiler }}-${{ matrix.build_type }}
        path: build/Testing/
```

## 关键配置说明

| 配置 | 用途 |
|------|------|
| `strategy.matrix` | 多编译器 + 多构建类型并行测试 |
| `ninja-build` | 比 make 快 2-3 倍，增量编译更智能 |
| `ccache` | 缓存编译结果，CI 重复构建提速 5-10 倍 |
| `ctest` | CMake 原生测试框架，直接输出测试报告 |
| `fail-fast: false` | Debug 失败后继续跑 Release，拿到全部结果 |

## 流水线阶段

```
Commit/Push → Build → Unit Tests → Lint → Integration Tests → Deploy(可选)
  ↓            ↓         ↓           ↓           ↓               ↓
 trigger   编译成功   单测通过    clang-tidy   API 测试      Docker push
                              clang-format 压测            k8s apply
```

---

# 静态分析集成（clang-tidy）

```yaml
- name: Static analysis
  run: |
    cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    run-clang-tidy -p build -checks='*,-fuchsia-*,-google-*' \
      -header-filter='src/.*' src/
```

本地开发集成到 CMake：

```cmake
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--checks=*,-fuchsia-*,-google-*")
endif()
```

> 团队 C++ 项目常见标准用 `.clang-tidy` 文件统一配置，`git commit` 前自动检查。

---

# Code Review 自动化

使用 GitHub 的 CODEOWNERS + 自动标注：

```gitignore
src/handler/     @backend-team
src/service/     @backend-team
src/repository/  @backend-team
proto/           @api-team
CMakeLists.txt   @tech-lead
```

```yaml
- name: Lint check
  run: |
    git diff --name-only origin/main | xargs clang-format --dry-run --Werror
    echo "::error file=$file::Code style violation"
```

---

# C++ 项目典型 CI 技巧

| 场景 | 做法 |
|------|------|
| 增量编译 | `ccache --max-size=5G` + `G Ninja` |
| 依赖缓存 | GitHub Actions 的 `cache` action 缓存 `vcpkg`/`Conan` 包 |
| 并行测试 | `ctest -j$(nproc)` 多核并行 |
| 测试超时 | `ctest --timeout 60` 防止死循环卡住 CI |
| 代码覆盖率 | `gcovr` + `codecov` 自动上传报告 |
| Sanitizer CI | 单独跑一个 `-DCMAKE_BUILD_TYPE=Debug -DSANITIZER=ON` 的 job |

> [!tip]- **工程要点**：C++ CI 核心是**快**——用 ccache + ninja + 并行把 15 分钟的编译压到 2-3 分钟。CI 跑的检查越多越好，但不要让它成为开发的阻力。建议：PR CI < 10 分钟，超过这个时间团队就开始绕 CI 了。

---

# 关联笔记

- Core Concepts：Working Tree, Index, HEAD (三区模型)
- Conflict Resolution (冲突解决实操)
- reset vs revert vs restore (撤销三兄弟)
- stash, tag, reflog (实用命令)
- 01b1-merge vs rebase vs cherry-pick (三种合并对比)

# 零基础阅读路径

先从最短命令路径跑通一次，再回来看配置字段与高级选项。每读一段命令，都要知道它读取什么、生成什么以及怎样撤销或诊断。

# 常见误区

- 只记命令，不理解它改变了哪些输入、产物或运行环境，发生故障时无法恢复。
- 没有在临时项目中亲自执行并保留输出，就把工具流程当成已经掌握。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-CI CD (持续集成与交付)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Git Collaboration Map (Git 协作导航)](/07-Engineering%20Tools%20(工程工具)/01-Git%20Collaboration%20(Git%20协作)/00-Git%20Collaboration%20Map%20(Git%20协作导航).md)
- 下一步：[02-Conflicts and Recovery (冲突与恢复)](/07-Engineering%20Tools%20(工程工具)/01-Git%20Collaboration%20(Git%20协作)/02-Conflicts%20and%20Recovery%20(冲突与恢复).md)
