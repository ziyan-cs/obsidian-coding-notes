---
tags:
  - devtools/tools
status: 🌱
---

# Code Quality & Build Optimization：clang-tidy, ccache, Ninja, Benchmark — 代码质量与构建加速

> [!important] **核心考点**：clang-tidy 静态分析、clang-format 格式化、ccache 编译缓存、Ninja 构建系统、Google Benchmark

## 代码质量工具

### clang-format（自动格式化）

团队统一代码风格，告别格式争论：

```bash
# 格式化单个文件
clang-format -i src/*.cpp src/*.h

# 检查但不修改（CI 用）
clang-format --dry-run --Werror src/*.cpp

# 递归格式化整个项目
find src/ -name '*.cpp' -o -name '*.h' | xargs clang-format -i
```

```yaml
# .clang-format（项目根目录）
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: Inline
PointerAlignment: Right
```

### clang-tidy（静态分析）

比编译器更聪明的 lint 工具，能发现潜在 bug：

```bash
# 生成 compile_commands.json（CMake 需要）
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# 运行检查
clang-tidy src/server.cpp -checks='modernize-*,performance-*'

# 自动修复
clang-tidy src/server.cpp -checks='modernize-*' --fix
```

**值得开启的 check：**

| Check | 发现什么问题 |
|-------|-------------|
| `modernize-use-override` | 漏写 `override` 关键字 |
| `performance-unnecessary-copy` | 不必要的拷贝（加 `const&`） |
| `bugprone-unused-raii` | RAII 对象未使用（临时对象立即析构） |
| `clang-analyzer-*` | 空指针解引用、内存泄漏 |
| `cppcoreguidelines-*` | C++ Core Guidelines 违规 |

### include-what-you-use（IWYU）

检查冗余/缺失的头文件包含：

```bash
# 生成建议
include-what-you-use src/server.cpp -- -Iinclude

# 配合 CMake 自动修正
cmake -B build -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=include-what-you-use
```

---

## 构建加速

### ccache（编译缓存）

编译器输出的缓存，第二次编译相同文件直接命中缓存：

```bash
# 安装
sudo apt install ccache

# 使用（CMake）
cmake -B build -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# 查看命中率
ccache --show-stats
# cache hit rate: 92.3%  ← 增量编译的常态
```

### Ninja（比 make 更快的构建系统）

```bash
# CMake + Ninja
cmake -B build -G Ninja
cmake --build build --parallel $(nproc)
# 增量编译：Ninja 只重编译修改的文件和依赖者

# 相比 Make 的优势：
# - 自动并行（无需 -j 参数）
# - 更快的依赖解析（Ninja 文件直接，无 Make 的递归）
# - 更短的错误信息（一行一个错误，不输出冗余）
```

**实测对比（大型 C++ 项目，增量编译）：**

| 构建系统 | 首次 | 改 1 个 .cpp | 改 1 个 .h |
|---------|------|-------------|------------|
| Make | 5:20 | 1:45 | 3:10 |
| Ninja | 4:50 | 0:35 | 1:20 |

---

## Google Benchmark（微基准测试）

精确测量函数的纳秒级/微秒级性能，防止性能退化：

### 基础用法

```cpp
#include <benchmark/benchmark.h>

// 测试 std::vector 遍历速度
static void BM_VectorIterate(benchmark::State& state) {
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);

    for (auto _ : state) {
        long long sum = 0;
        for (int x : v) sum += x;
        benchmark::DoNotOptimize(sum);  // 防止编译器优化掉
    }
}
BENCHMARK(BM_VectorIterate)->Arg(100)->Arg(1000)->Arg(10000);

// 测试 unordered_map vs map
static void BM_MapInsert(benchmark::State& state) {
    for (auto _ : state) {
        std::unordered_map<int, int> m;
        for (int i = 0; i < state.range(0); i++)
            m[i] = i;
    }
}
BENCHMARK(BM_MapInsert)->Range(8, 8<<10);

BENCHMARK_MAIN();
```

```bash
# 编译并运行
g++ -O2 -std=c++17 bench.cpp -lbenchmark -lpthread -o bench
./bench

# 输出示例：
# BM_VectorIterate/100     25.3 ns    ← 100 个元素
# BM_VectorIterate/1000     187 ns
# BM_VectorIterate/10000   1821 ns
```

### 对比测试

```cpp
// 比较 std::shared_ptr 和 raw ptr
static void BM_SharedPtr(benchmark::State& state) {
    for (auto _ : state) {
        auto p = std::make_shared<int>(42);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK(BM_SharedPtr);

static void BM_RawPtr(benchmark::State& state) {
    for (auto _ : state) {
        auto p = new int(42);
        delete p;
    }
}
BENCHMARK(BM_RawPtr);

// 输出：shared_ptr ≈ raw ptr 的 1.5x（make_shared 一次分配优化）
```

---

## 环境一致性：DevContainer

VSCode DevContainer 或 Docker Compose，保证团队开发环境一致：

```dockerfile
# .devcontainer/Dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    cmake ninja-build ccache clang-16 clang-tidy \
    lldb gdb valgrind \
    libgtest-dev libbenchmark-dev \
    libspdlog-dev libfmt-dev

# 使用 CMake Presets
COPY .devcontainer/CMakePresets.json /workspace/
```

```json
// CMakePresets.json（CMake 3.21+）
{
  "version": 3,
  "configurePresets": [
    {
      "name": "debug",
      "generator": "Ninja",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_CXX_COMPILER_LAUNCHER": "ccache",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "release",
      "generator": "Ninja",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "CMAKE_CXX_COMPILER_LAUNCHER": "ccache"
      }
    }
  ],
  "buildPresets": [
    { "name": "debug", "configurePreset": "debug" },
    { "name": "release", "configurePreset": "release" }
  ]
}
```

---

## 工程要点汇总

| 工具 | 用途 | 优先级 |
|------|------|--------|
| clang-format | 统一代码风格 | ★★★ 马上加 |
| clang-tidy | 静态分析，发现潜在 bug | ★★★ 马上加 |
| ccache | 编译缓存，省 50-90% 编译时间 | ★★★ 必须 |
| Ninja | 更快的构建系统 | ★★☆ 推荐 |
| Google Benchmark | 微基准测试，防性能退化 | ★★☆ 推荐 |
| DevContainer | 团队环境一致 | ★☆☆ 有余力再加 |
| IWYU | 头文件管理 | ★☆☆ 有余力再加 |

> [!tip]- **工程要点**：这些工具的价值排序是**频率 × 节省时间**。clang-format 每天触发几十次，ccache 每次编译都在省钱，值得第一时间配好。Google Benchmark 在 CI 中跑，对比每次 PR 的性能变化，发现退化立即告警。

---

## 关联笔记

- [Makefile Basics (Makefile基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04a-Makefile%20Basics%20(Makefile基础).md)
- [Docker Basics：Image & Container (Docker基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04b-Docker%20Basics：Image%20&%20Container%20(Docker基础).md)
- [Shell Tools：grep, sed, awk, tmux (Shell效率工具)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04c-Shell%20Tools：grep,%20sed,%20awk,%20tmux%20(Shell效率工具).md)
- [Docker Multi-stage Build for C++：Deploy Optimization (Docker多阶段构建与部署)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04e-Docker%20Multi-stage%20Build%20for%20C++：Deploy%20Optimization%20(Docker多阶段构建与部署).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
