---
tags:
  - devtools/cmake
status: 🌱
---


> [!important] **核心考点**：最小可用结构、各指令的作用与顺序

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

---

## 关联笔记

- [target_link_libraries & include_directories (依赖管理)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [CMake with vcpkg & Conan (包管理器集成)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
