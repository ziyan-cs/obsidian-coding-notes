---
tags:
  - git
status: 🌱
---

> **核心考点**：GitHub Actions / GitLab CI 配置、C++ 项目 CI 流水线、自动化测试与部署

## C++ CI/CD 流水线

### GitHub Actions 配置

```yaml
# .github/workflows/ci.yml
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

### 关键配置说明

| 配置 | 用途 |
|------|------|
| `strategy.matrix` | 多编译器 + 多构建类型并行测试 |
| `ninja-build` | 比 make 快 2-3 倍，增量编译更智能 |
| `ccache` | 缓存编译结果，CI 重复构建提速 5-10 倍 |
| `ctest` | CMake 原生测试框架，直接输出测试报告 |
| `fail-fast: false` | Debug 失败后继续跑 Release，拿到全部结果 |

### 流水线阶段

```
Commit/Push → Build → Unit Tests → Lint → Integration Tests → Deploy(可选)
  ↓            ↓         ↓           ↓           ↓               ↓
 trigger   编译成功   单测通过    clang-tidy   API 测试      Docker push
                              clang-format 压测            k8s apply
```

---

## 静态分析集成（clang-tidy）

```yaml
# CI 中增加静态分析步骤
- name: Static analysis
  run: |
    cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    run-clang-tidy -p build -checks='*,-fuchsia-*,-google-*' \
      -header-filter='src/.*' src/
```

本地开发集成到 CMake：

```cmake
# CMakeLists.txt
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--checks=*,-fuchsia-*,-google-*")
endif()
```

> 团队 C++ 项目常见标准用 `.clang-tidy` 文件统一配置，`git commit` 前自动检查。

---

## Code Review 自动化

使用 GitHub 的 CODEOWNERS + 自动标注：

```gitignore
# .github/CODEOWNERS
# 每行：文件模式 → 审核人
src/handler/     @backend-team
src/service/     @backend-team
src/repository/  @backend-team
proto/           @api-team
CMakeLists.txt   @tech-lead
```

```yaml
# CI 自动标注审查要求
- name: Lint check
  run: |
    git diff --name-only origin/main | xargs clang-format --dry-run --Werror
    echo "::error file=$file::Code style violation"
```

---

## C++ 项目典型 CI 技巧

| 场景 | 做法 |
|------|------|
| 增量编译 | `ccache --max-size=5G` + `G Ninja` |
| 依赖缓存 | GitHub Actions 的 `cache` action 缓存 `vcpkg`/`Conan` 包 |
| 并行测试 | `ctest -j$(nproc)` 多核并行 |
| 测试超时 | `ctest --timeout 60` 防止死循环卡住 CI |
| 代码覆盖率 | `gcovr` + `codecov` 自动上传报告 |
| Sanitizer CI | 单独跑一个 `-DCMAKE_BUILD_TYPE=Debug -DSANITIZER=ON` 的 job |

> **工程要点**：C++ CI 核心是**快**——用 ccache + ninja + 并行把 15 分钟的编译压到 2-3 分钟。CI 跑的检查越多越好，但不要让它成为开发的阻力。建议：PR CI < 10 分钟，超过这个时间团队就开始绕 CI 了。

---

## 关联笔记

- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [stash, tag, reflog (实用命令)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)
