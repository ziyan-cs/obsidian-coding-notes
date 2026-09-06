---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

# CMakeLists Structure (CMake 项目结构)

> [!note] 本节重点：核心考点：最小可用结构、各指令的作用与顺序

> [!tip] CMake 的核心单位是 target
> 把可执行文件和库声明为明确的 target，并让 include path、编译选项和依赖跟随 target 传播。全局变量和全局 `include_directories()` 在小项目能工作，却会在工程变大后制造隐式耦合。

# 最小工程模板

```cmake
cmake_minimum_required(VERSION 3.20)         # 声明最低 CMake 版本
project(MyProject VERSION 1.0 LANGUAGES CXX) # 项目名、版本、语言

set(CMAKE_CXX_STANDARD 17)                 # C++ 标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)        # 强制要求，找不到就报错
set(CMAKE_CXX_EXTENSIONS OFF)              # 禁用 GNU 扩展，使用纯标准

add_executable(myapp
    src/main.cpp
    src/utils.cpp
)

add_library(mylib STATIC
    src/mylib.cpp
)
```

# 典型多目录项目结构

```
MyProject/
├── CMakeLists.txt          ← 根 CMakeLists
├── src/
│   ├── CMakeLists.txt      ← 子目录
│   └── main.cpp
├── lib/
│   ├── CMakeLists.txt
│   └── mylib.cpp
├── include/
│   └── mylib.h
└── tests/
    ├── CMakeLists.txt
    └── test_main.cpp
```

根 CMakeLists.txt：

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject)

add_subdirectory(lib)    # 处理 lib/CMakeLists.txt
add_subdirectory(src)    # 处理 src/CMakeLists.txt
add_subdirectory(tests)
```

# 常用变量

```cmake
${PROJECT_NAME}           # 项目名
${PROJECT_SOURCE_DIR}     # 根 CMakeLists.txt 所在目录
${CMAKE_CURRENT_SOURCE_DIR}  # 当前 CMakeLists.txt 所在目录
${CMAKE_BINARY_DIR}       # 构建目录（通常是 build/）
${CMAKE_INSTALL_PREFIX}   # 安装路径（默认 /usr/local）
```

# 构建流程

```bash
mkdir build && cd build
cmake ..                  # 配置阶段：生成 Makefile / Ninja 文件
cmake --build .           # 构建阶段：实际编译
cmake --install .         # 安装（可选）
```

# 30 秒回答

CMake 的配置阶段读取 `CMakeLists.txt` 并生成构建系统，构建阶段再实际编译。一个可维护项目从 `add_executable` / `add_library` 定义 target 开始，子目录用 `add_subdirectory` 组织；依赖与编译属性应尽量挂在具体 target 上，而不是散落在全局变量里。

# 自测

1. `cmake -S . -B build` 与在 `build/` 中运行 `cmake ..` 有什么关系？为什么前者更明确？
2. 什么信息应属于一个 library target，而不应写成全局设置？
3. `add_subdirectory` 为什么比在根文件里堆所有源文件更利于维护？

---

# 关联笔记

- [target_link_libraries & include_directories (依赖管理)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [CMake with vcpkg & Conan (包管理器集成)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

---

# CMake Target Dependencies (CMake 目标依赖)

> [!note] 本节重点：核心考点：PRIVATE / PUBLIC / INTERFACE 的区别、现代 CMake 的 target-based 思想

# 现代 CMake 的核心思想

**以 target 为中心，而非以目录为中心。** 每个 target（可执行文件或库）管理自己的属性，依赖关系通过 target 之间传递。

```cmake
include_directories(include/)
link_libraries(mylib)

target_include_directories(myapp PRIVATE include/)
target_link_libraries(myapp PRIVATE mylib)
```

---

# PRIVATE / PUBLIC / INTERFACE

这是现代 CMake 中最重要的概念，控制属性的**传播范围**：

|关键字|对当前 target 生效|传播给依赖当前 target 的 target|
|---|---|---|
|PRIVATE|✅|❌|
|PUBLIC|✅|✅|
|INTERFACE|❌|✅|

## 示例场景

```cmake
target_include_directories(mylib
    PUBLIC  include/        # mylib 自己用，链接 mylib 的 target 也自动获得
    PRIVATE src/internal/   # 只有 mylib 自己的编译单元能看到
)

target_link_libraries(mylib
    PUBLIC  fmt::fmt         # mylib 和所有链接 mylib 的 target 都链接 fmt
    PRIVATE spdlog::spdlog  # 只有 mylib 自己链接 spdlog
)

target_link_libraries(myapp PRIVATE mylib)
```

**判断用哪个的经验法则：**

- 头文件在 `include/`（对外暴露）→ `PUBLIC`
- 头文件在 `src/`（内部实现）→ `PRIVATE`
- 纯头文件库（header-only）→ `INTERFACE`

---

# target_compile_options & target_compile_definitions

```cmake
target_compile_options(myapp PRIVATE
    -Wall -Wextra -Wpedantic   # 开启警告
    $<$<CONFIG:Debug>:-g -O0>  # Debug 模式额外选项（生成器表达式）
)

target_compile_definitions(myapp PRIVATE
    VERSION_MAJOR=1
    $<$<CONFIG:Debug>:DEBUG_MODE>   # Debug 模式下定义 DEBUG_MODE 宏
)
```

---

# 关联笔记 · 延伸要点 2
- [CMakeLists․txt Structure (项目结构模板)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [CMake with vcpkg & Conan (包管理器集成)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

---

# CMake Build Types (CMake 构建类型)

> [!note] 本节重点：核心考点：四种构建类型的使用场景、优化级别、常用配置

# 四种标准构建类型

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
```

|类型|编译器标志（GCC/Clang）|用途|
|---|---|---|
|Debug|`-g -O0`|开发调试，包含符号表，不优化|
|Release|`-O3 -DNDEBUG`|生产发布，最大优化，禁用 assert|
|RelWithDebInfo|`-O2 -g -DNDEBUG`|生产环境调试，有符号表但也优化|
|MinSizeRel|`-Os -DNDEBUG`|嵌入式/资源受限，最小体积|

> `NDEBUG` 宏会禁用 `assert()`，Release 模式下断言失效，需注意。

# 在 CMake 中按构建类型设置行为

```cmake
target_compile_options(myapp PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>       # Debug 下开 ASan
    $<$<CONFIG:Release>:-march=native>          # Release 下针对本机 CPU 优化
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(myapp PRIVATE ENABLE_LOGGING)
endif()
```

# 多配置生成器（Visual Studio / Xcode / Ninja Multi-Config）

```bash
cmake -G "Ninja Multi-Config" ..
cmake --build . --config Release
cmake --build . --config Debug
```

---

# 关联笔记 · 延伸要点 3
- [CMakeLists․txt Structure (项目结构模板)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)
- [target_link_libraries & include_directories (依赖管理)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [CMake with vcpkg & Conan (包管理器集成)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

# 零基础阅读路径

先从最短命令路径跑通一次，再回来看配置字段与高级选项。每读一段命令，都要知道它读取什么、生成什么以及怎样撤销或诊断。

# 常见误区

- 只记命令，不理解它改变了哪些输入、产物或运行环境，发生故障时无法恢复。
- 没有在临时项目中亲自执行并保留输出，就把工具流程当成已经掌握。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-CMake Project and Targets (CMake 项目与目标)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
