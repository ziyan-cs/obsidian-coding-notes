---
tags:
  - devtools/cmake
status: learning
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# CMakeLists.txt Structure — 项目结构模板

> [!abstract] 核心考点：最小可用结构、各指令的作用与顺序

> [!tip] CMake 的核心单位是 target
> 把可执行文件和库声明为明确的 target，并让 include path、编译选项和依赖跟随 target 传播。全局变量和全局 `include_directories()` 在小项目能工作，却会在工程变大后制造隐式耦合。

## 最小工程模板

```cmake
cmake_minimum_required(VERSION 3.20)         # 声明最低 CMake 版本
project(MyProject VERSION 1.0 LANGUAGES CXX) # 项目名、版本、语言

set(CMAKE_CXX_STANDARD 17)                 # C++ 标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)        # 强制要求，找不到就报错
set(CMAKE_CXX_EXTENSIONS OFF)              # 禁用 GNU 扩展，使用纯标准

# 收集源文件
add_executable(myapp
    src/main.cpp
    src/utils.cpp
)

# 或者构建库
add_library(mylib STATIC
    src/mylib.cpp
)
```

## 典型多目录项目结构

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

## 常用变量

```cmake
${PROJECT_NAME}           # 项目名
${PROJECT_SOURCE_DIR}     # 根 CMakeLists.txt 所在目录
${CMAKE_CURRENT_SOURCE_DIR}  # 当前 CMakeLists.txt 所在目录
${CMAKE_BINARY_DIR}       # 构建目录（通常是 build/）
${CMAKE_INSTALL_PREFIX}   # 安装路径（默认 /usr/local）
```

## 构建流程

```bash
mkdir build && cd build
cmake ..                  # 配置阶段：生成 Makefile / Ninja 文件
cmake --build .           # 构建阶段：实际编译
cmake --install .         # 安装（可选）
```

## 30 秒回答

CMake 的配置阶段读取 `CMakeLists.txt` 并生成构建系统，构建阶段再实际编译。一个可维护项目从 `add_executable` / `add_library` 定义 target 开始，子目录用 `add_subdirectory` 组织；依赖与编译属性应尽量挂在具体 target 上，而不是散落在全局变量里。

## 自测

1. `cmake -S . -B build` 与在 `build/` 中运行 `cmake ..` 有什么关系？为什么前者更明确？
2. 什么信息应属于一个 library target，而不应写成全局设置？
3. `add_subdirectory` 为什么比在根文件里堆所有源文件更利于维护？

---

## 关联笔记

- [target_link_libraries & include_directories (依赖管理)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [CMake with vcpkg & Conan (包管理器集成)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
