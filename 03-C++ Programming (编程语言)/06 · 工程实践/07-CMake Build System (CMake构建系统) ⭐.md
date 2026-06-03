> **核心考点**：CMake 是现代 C++ 的标准构建工具、目标导向的声明式构建、库的创建与依赖管理

## 最小 CMake 项目

```cmake
# CMakeLists.txt — 项目根目录
cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 不使用编译器扩展

# 添加可执行文件
add_executable(main main.cpp)
```

```bash
# 构建（out-of-source build，推荐）
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

## 目标导向（Target-Based）设计

```cmake
# CMake 3.x 的基本单元是"目标"（target）
# 每个库/可执行文件就是一个目标

# 添加库
add_library(mylib STATIC
    src/mylib.cpp
    src/helper.cpp
)

# 添加可执行文件
add_executable(main src/main.cpp)

# 链接库和指定包含目录
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
# Debug / Release / RelWithDebInfo / MinSizeRel
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 安装路径
cmake --install . --prefix /usr/local

# 测试开关
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_test(NAME unit_test COMMAND test_runner)
endif()
```

## 查找与使用外部库

```cmake
# 方法 1：find_package（系统已安装的库）
find_package(Boost REQUIRED COMPONENTS filesystem system)
target_link_libraries(main PRIVATE Boost::filesystem Boost::system)

# 方法 2：FetchContent（自动下载，CMake 3.11+）
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
# ⚠️ 不要这样做：
set(CMAKE_CXX_FLAGS "-O2 -Wall")  # 全局修改，不模块化
include_directories(include)       # 全局包含路径
link_directories(/usr/lib)         # 全局链接路径
add_definitions(-DDEBUG)           # 全局宏

# ✅ 应该这样做：
# 每个目标各自声明依赖
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
# 项目信息
${PROJECT_NAME}          # 项目名
${PROJECT_SOURCE_DIR}    # 源码根目录
${PROJECT_BINARY_DIR}    # 构建目录

# 编译器信息
${CMAKE_CXX_COMPILER}    # C++ 编译器路径
${CMAKE_CXX_COMPILER_ID} # GNU / Clang / AppleClang / MSVC

# 平台
${CMAKE_SYSTEM_NAME}     # Linux / Windows / Darwin
```

> **工程要点**：现代 CMake 是"**声明式**"而非"脚本式"。核心思想：**描述你的目标及其依赖关系**，CMake 自行推导构建顺序和编译选项。避免全局函数（`include_directories`、`add_definitions` 等），改用目标属性（`target_*` 系列）。
