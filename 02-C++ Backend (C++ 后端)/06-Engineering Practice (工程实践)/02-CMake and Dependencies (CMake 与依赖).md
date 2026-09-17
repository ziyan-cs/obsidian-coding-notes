---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 阅读方式：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

> [!summary]- 复述检查：学完后再展开
>
> **核心结论**：CMake 应围绕 target 声明源文件、编译选项和依赖；让 PUBLIC、PRIVATE、INTERFACE 的传播关系表达真实接口边界，才能得到可复现且可维护的构建。

# 工程结论

CMake 的核心不是生成命令，而是声明 target 及其源文件、编译选项、包含路径和依赖。依赖应沿 target 边界传播；避免全局目录命令把配置偷偷影响到其他模块。

## 依赖模型

```text
library target
  - PUBLIC: dependency required by consumers of its interface
  - PRIVATE: dependency required only by its implementation
executable target -> links the library target
```

## 选型边界

- 优先 `target_link_libraries`、`target_include_directories`，少用全局 `include_directories`。
- Debug/Release 差异由 target 属性与 toolchain 明确表达，不靠手改宏。
- 第三方库优先使用导入 target；`find_package` 失败时记录版本、来源和可复现安装步骤。

> [!question]- 自测：先回答再展开
> 1. PUBLIC、PRIVATE、INTERFACE 依赖分别向谁传播？
> 2. 为什么 target-based CMake 比全局 include path 更易维护？
> 3. 一个可复现构建至少需要声明哪些输入？

# CMake Build System (CMake构建系统)

> [!note] 本节重点：CMake 是现代 C++ 的标准构建工具、目标导向的声明式构建、库的创建与依赖管理

## 最小 CMake 项目

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 不使用编译器扩展

add_executable(main main.cpp)
```

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

## 目标导向（Target-Based）设计

```cmake

add_library(mylib STATIC
    src/mylib.cpp
    src/helper.cpp
)

add_executable(main src/main.cpp)

target_include_directories(mylib
    PUBLIC  include      # 使用者也会获得此包含路径
    PRIVATE src          # 仅 mylib 自己可见
)

target_link_libraries(main PRIVATE mylib)
```

**PUBLIC vs PRIVATE vs INTERFACE**：

| 关键字 | 对目标自身 | 对链接者 |
|--------|-----------|---------|
| `PRIVATE` | ✅ 应用 | ❌ 不传递 |
| `PUBLIC` | ✅ 应用 | ✅ 传递 |
| `INTERFACE` | ❌ 不应用 | ✅ 仅传递（适合头文件库）|

## 常用构建配置

```cmake
cmake -DCMAKE_BUILD_TYPE=Debug ..

cmake --install . --prefix /usr/local

option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_test(NAME unit_test COMMAND test_runner)
endif()
```

## 查找与使用外部库

```cmake
find_package(Boost REQUIRED COMPONENTS filesystem system)
target_link_libraries(main PRIVATE Boost::filesystem Boost::system)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)
```

## 现代 CMake 最佳实践

```cmake
set(CMAKE_CXX_FLAGS "-O2 -Wall")  # 全局修改，不模块化
include_directories(include)       # 全局包含路径
link_directories(/usr/lib)         # 全局链接路径
add_definitions(-DDEBUG)           # 全局宏

target_include_directories(mylib PUBLIC include)
target_compile_options(mylib PRIVATE -Wall -Wextra)
target_compile_definitions(mylib PRIVATE DEBUG)
```

## 项目目录结构

```text
project/
├── CMakeLists.txt          # 根构建文件
├── cmake/                  # 自定义 CMake 模块
│   └── FindMyLib.cmake
├── include/project/        # 公共头文件
│   └── module.h
├── src/                    # 实现
│   ├── CMakeLists.txt
│   ├── main.cpp
│   └── module.cpp
├── tests/                  # 测试
│   ├── CMakeLists.txt
│   └── test_module.cpp
├── third_party/            # 第三方依赖（git submodule）
│   └── fmt
└── examples/               # 示例代码
    └── example.cpp
```

根 CMakeLists.txt 通过 `add_subdirectory` 组织子目录：

```cmake
add_subdirectory(src)
add_subdirectory(tests)
```

## 常用 CMake 变量

```cmake
${PROJECT_NAME}          # 项目名
${PROJECT_SOURCE_DIR}    # 源码根目录
${PROJECT_BINARY_DIR}    # 构建目录

${CMAKE_CXX_COMPILER}    # C++ 编译器路径
${CMAKE_CXX_COMPILER_ID} # GNU / Clang / AppleClang / MSVC

${CMAKE_SYSTEM_NAME}     # Linux / Windows / Darwin
```

> [!tip]- **工程要点**：现代 CMake 是"**声明式**"而非"脚本式"。核心思想：**描述你的目标及其依赖关系**，CMake 自行推导构建顺序和编译选项。避免全局函数（`include_directories`、`add_definitions` 等），改用目标属性（`target_*` 系列）。

---


> [!info]- 延伸阅读
> - 下一步：[03-Exceptions and Debugging (异常与调试)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/03-Exceptions%20and%20Debugging%20(异常与调试).md)
