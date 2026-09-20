---
study_stage: learn
review_due: 2026-10-22
tags: [engineering/build, cpp/cmake]
---

> [!abstract] 学习目标
> 能从源码、目标和使用要求构建一个可移植的 C++ 工程，区分 CMake 的配置、生成、编译和测试阶段，并解释 Debug/Release 与单配置/多配置生成器。

# CMake 描述的是构建图

CMake 读取 `CMakeLists.txt`，在配置（configure）阶段检测工具链、解析依赖并定义目标，再在生成（generate）阶段生成 Ninja/Make/IDE 工程；实际编译、链接由构建工具在 build 阶段完成。CMake 自身不是 C++ 编译器。

```text
源码 + CMakeLists + 工具链 + 配置
  → configure/generate → build graph
  → 编译目标文件 → 链接产物
  → ctest 验证 → install（如果声明）
```

构建图的节点是可执行文件、库和自定义目标；边表示依赖及其使用要求（usage requirements），而非“把库文件名附加到命令末尾”。

# 一个可运行的最小工程

```text
demo/
├── CMakeLists.txt
├── include/demo/math.hpp
├── src/math.cpp
├── src/main.cpp
└── tests/math_test.cpp
```

```cmake
cmake_minimum_required(VERSION 3.20)
project(Demo VERSION 1.0 LANGUAGES CXX)

add_library(demo_math src/math.cpp)
target_compile_features(demo_math PUBLIC cxx_std_20)
target_include_directories(demo_math PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

add_executable(demo_app src/main.cpp)
target_link_libraries(demo_app PRIVATE demo_math)

include(CTest)
if(BUILD_TESTING)
    add_executable(math_test tests/math_test.cpp)
    target_link_libraries(math_test PRIVATE demo_math)
    add_test(NAME math_test COMMAND math_test)
endif()
```

`src/main.cpp` 包含公共头文件 `<demo/math.hpp>`，`demo_app` 通过链接 `demo_math` 继承它的公共 include 路径与 C++20 要求。上例中 `INSTALL_INTERFACE` 只描述安装后路径；若真要对外安装，还需正确的 `install(TARGETS ... EXPORT ...)`、头文件和包配置，不能仅凭这一行宣布库可被 `find_package` 使用。

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

源目录和构建目录分离，构建产物不污染源码；不同工具链、配置和依赖图宜使用不同 build 目录，避免旧缓存混入。

# PUBLIC、PRIVATE、INTERFACE

| 作用域 | 当前目标需要 | 消费者需要 |
|---|---:|---:|
| `PRIVATE` | 是 | 通常不作为编译使用要求传播 |
| `PUBLIC` | 是 | 是 |
| `INTERFACE` | 否 | 是 |

判断依据是**消费者构建时是否需要这项使用要求**，而不是“头文件在哪个目录”。公共头文件若暴露依赖库类型，消费者可能也需要该库的头文件与链接要求；仅在 `.cpp` 内使用的编译定义通常为 `PRIVATE`。

> [!warning] 静态库的传播细节
> 静态库没有最终链接步骤；即使其依赖在源代码层面为 `PRIVATE`，最终消费者仍可能需要该库的链接项。CMake 对静态库的 link-only 传递有专门规则。不要把表格中的“PRIVATE 不传播”误解为“链接期绝不会出现”。

header-only 库通常用 `add_library(name INTERFACE)`，并以 `INTERFACE` 声明 include 目录与编译特性。优先使用目标 API，不在根文件滥用 `include_directories()`、`link_libraries()` 和全局编译参数。

# 源码目录、生成文件与安装边界

项目拆目录时用 `add_subdirectory(lib)` 等组织局部目标，但拆分本身不提升正确性。生成文件（protobuf、版本头）应输出到 binary dir，并显式声明产物及依赖，避免多人并行构建时共享写同一文件。

`CMAKE_CURRENT_SOURCE_DIR` 与 `CMAKE_CURRENT_BINARY_DIR` 分别表示当前 CMake 文件对应的源/构建目录；`CMAKE_SOURCE_DIR` 指顶层源目录，作为子项目时通常不等于当前项目根。

生成可安装库时还需区分 build-tree 与 install-tree 的路径，避免把开发者本机绝对路径写入导出的 target。

# 构建配置与工具链

单配置生成器（如 Ninja、Unix Makefiles）通常在配置时选 `CMAKE_BUILD_TYPE`；多配置生成器（Visual Studio、Ninja Multi-Config 等）在构建时用 `--config`：

```bash
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

cmake -S . -B build-multi -G "Ninja Multi-Config"
cmake --build build-multi --config Debug
ctest --test-dir build-multi -C Debug --output-on-failure
```

`Debug`、`Release`、`RelWithDebInfo` 和 `MinSizeRel` 的实际编译选项随编译器、平台、工具链文件与项目配置变化，不能背成固定的 `-O0/-O3/-g` 表。部分 Release 配置定义 `NDEBUG`，会关闭标准 `assert`；生产输入验证不能只靠 `assert`。

交叉编译的目标平台、sysroot 与编译器一般通过 toolchain file 在首次配置前确定；改变工具链后应使用独立构建目录。不要把 `-march=native` 硬塞进通用 Release 制品：它可能在其他 CPU 上无法运行。

# 可共享的配置

`CMakePresets.json` 可把生成器、binaryDir、cacheVariables 等项目配置纳入版本管理；个人本机专用选项适合 `CMakeUserPresets.json`，不要把个人路径提交给团队。

```json
{
  "version": 2,
  "configurePresets": [
    {
      "name": "dev",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/dev",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Debug" }
    }
  ]
}
```

```bash
cmake --preset dev
cmake --build build/dev
ctest --test-dir build/dev --output-on-failure
```

预设文件的 schema version 要与项目声明的最低 CMake 版本兼容；本例版本 2 可由 CMake 3.20 读取，版本 3 需要至少 3.21。团队采用预设前先在最低版本上实际运行。

# 排查与实践

1. 空 build 目录配置，观察 `CMakeCache.txt` 中工具链和包路径。
2. 修改 `math.hpp`，确认库及消费者按依赖关系重编；只改 `main.cpp` 不应触发库重编。
3. 用 `cmake --build build --verbose` 检查实际命令；若头文件找不到，追踪它应由哪个 target 的 usage requirement 提供。
4. 用单配置与多配置生成器分别构建并运行测试，记录差异。

# 检查理解

1. 为什么 `target_link_libraries(app PRIVATE lib)` 可能同时带来 include 路径？
2. 公共头文件若暴露依赖库类型，应怎样决定 PUBLIC/PRIVATE？
3. 多配置生成器为什么不能仅靠 `CMAKE_BUILD_TYPE` 切换配置？
4. 为什么切换编译器或 toolchain 后建议使用新的 build 目录？

> [!summary] 本篇结论
> CMake 的核心是目标与使用要求构成的构建图。把属性挂到正确目标、分离源码与构建、区分配置模式和安装边界，才能让项目在另一台机器上正确生成并构建。

## 权威依据

- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [CMake target commands](https://cmake.org/cmake/help/latest/guide/tutorial/In-Depth%20CMake%20Target%20Commands.html)
- [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)

下一步：[02-CMake Dependencies (CMake 依赖)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/02-CMake%20Dependencies%20(CMake%20依赖).md)
