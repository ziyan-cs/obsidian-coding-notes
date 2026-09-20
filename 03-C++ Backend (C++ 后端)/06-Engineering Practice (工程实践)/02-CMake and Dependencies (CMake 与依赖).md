---
study_stage: backlog
---

> [!abstract] 学习目标
> 能从零构建一个库与可执行文件，并解释编译要求如何沿 target 传播。本文用 CMake 3.20+ 和 C++17；终端命令与 CMake 语法分开。

# 从一个可构建项目开始

目录中的头文件声明接口，源文件实现接口；`CMakeLists.txt` 描述构建关系：

```text
demo/
├── CMakeLists.txt
├── include/demo/greeter.hpp
└── src/
    ├── greeter.cpp
    └── main.cpp
```

`include/demo/greeter.hpp`：

```cpp
#pragma once
#include <string>
#include <string_view>
namespace demo { std::string greet(std::string_view name); }
```

`src/greeter.cpp`：

```cpp
#include "demo/greeter.hpp"
namespace demo {
std::string greet(std::string_view name) {
    return "Hello, " + std::string{name};
}
}
```

`src/main.cpp`：

```cpp
#include "demo/greeter.hpp"
#include <iostream>
int main() { std::cout << demo::greet("backend") << '\n'; }
```

`CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)
project(BackendDemo VERSION 1.0 LANGUAGES CXX)

add_library(greeter src/greeter.cpp)
target_include_directories(greeter PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(greeter PUBLIC cxx_std_17)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE greeter)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME smoke COMMAND app)
endif()
```

在项目根目录的**终端**运行：

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

`-S` 指源码目录，`-B` 指构建目录，避免把生成文件混进源码。`CMAKE_BUILD_TYPE` 对 Ninja、Unix Makefiles 等**单配置**生成器有意义；Visual Studio、Xcode、Ninja Multi-Config 等多配置生成器则在构建/测试时通过 `--config`、`-C` 选配置。上述命令同时写出两者，是为了说明差别，并非要求一个生成器同时采用两套机制。

# 把依赖与接口写在 target 上

| 作用域 | 目标自身使用 | 使用该目标的下游继承 |
|---|---|---|
| `PRIVATE` | 是 | 否 |
| `PUBLIC` | 是 | 是 |
| `INTERFACE` | 否 | 是 |

`greeter` 的公共头文件在 `include/`，消费者也要能找到它，所以路径是 `PUBLIC`。`greeter` 的接口使用 C++17 的 `string_view`，语言要求也标为 `PUBLIC`。`app` 仅自己依赖 `greeter`，所以链接关系写 `PRIVATE`。静态库的私有链接依赖在最终链接时仍可能以 link-only 形式被传递，不能把上表误解为“下游绝对不会链接该库”。

`target_compile_features(greeter PUBLIC cxx_std_17)` 表达目标的**最低**语言版本。项目希望彻底禁用编译器扩展时，还可针对目标设置 `CXX_EXTENSIONS OFF`；不要误以为仅写 `cxx_std_17` 就必然禁用扩展。若安装并导出库，公共 include 路径要区分 build/install interface，不能把源码目录绝对路径直接写进导出接口。

# 测试与第三方依赖

`include(CTest)` 提供 `BUILD_TESTING` 选项并启用测试；上面的 smoke test 只检查进程能否成功退出，不检查输出内容。真正的单元测试应验证断言、边界输入和失败路径。`add_test` 的 `COMMAND app` 会解析当前构建中的 executable target。

第三方库优先使用其提供的 CMake package 和导入 target，而不是写死本机 `/usr/lib`：

```cmake
find_package(fmt CONFIG REQUIRED)
target_link_libraries(app PRIVATE fmt::fmt)
```

这段是**可选增量**，只有安装了提供 `fmtConfig.cmake` 的包后才能加入前述项目。若使用 `FetchContent`，应固定可审计的提交哈希或带校验的归档，并记录来源、许可证与离线构建策略；不要在长期维护的项目中跟随浮动分支。项目是否能重现还依赖编译器、生成器、系统库和包版本，CMake 文件本身不是完整 lockfile。

## 验证清单

1. 运行三条终端命令，确认 `app` 输出 `Hello, backend` 且 CTest 成功。
2. 把 `target_include_directories(greeter PUBLIC ...)` 改成 `PRIVATE`，观察 `main.cpp` 的头文件查找错误，再解释传播链。
3. 将公共头文件里的 `string_view` 换成只在实现中使用的特性，判断语言标准要求能否改成 `PRIVATE`。
4. 故意让 `greet` 只有声明没有定义，区分编译阶段与链接阶段的报错。

资料：[CMake 官方教程：目标命令](https://cmake.org/cmake/help/latest/guide/tutorial/In-Depth%20CMake%20Target%20Commands.html)、[CMake 官方教程：链接作用域](https://cmake.org/cmake/help/latest/guide/tutorial/Getting%20Started%20with%20CMake.html)。

> [!info]- 延伸阅读
> - [03-Exceptions and Debugging (异常与调试)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/06-Engineering%20Practice%20(工程实践)/03-Exceptions%20and%20Debugging%20(异常与调试).md)
