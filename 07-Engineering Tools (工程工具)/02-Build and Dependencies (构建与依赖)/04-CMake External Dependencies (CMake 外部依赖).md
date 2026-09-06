---
tags:
  - devtools/cmake
status: learning
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 04-CMake External Dependencies (CMake 外部依赖)

> [!abstract] 核心考点：find_package 的两种模式（Module/Config）、搜索路径、如何编写 Find 脚本

> [!warning] 依赖“能找到”不代表配置可复现
> 不要依赖某台机器碰巧安装了库。明确依赖版本、目标名和安装来源；CI 或全新环境能从零配置成功，才说明构建边界真正成立。

## find_package 基础

```cmake
find_package(OpenSSL REQUIRED)        # REQUIRED：找不到就报错
find_package(Boost 1.70 COMPONENTS filesystem system)  # 指定版本和组件

target_link_libraries(myapp PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    Boost::filesystem
)
```

CMake 会在以下位置搜索：

- 系统默认路径（`/usr/lib/cmake/`、`/usr/local/lib/cmake/`）
- `CMAKE_PREFIX_PATH` 指定的路径
- 各库自带的 `*Config.cmake` 或 `Find*.cmake` 文件

## 两种 find_package 模式

|模式|触发条件|文件来源|
|---|---|---|
|Config 模式（现代）|库自带 `FooConfig.cmake`|库安装时提供|
|Module 模式（兼容）|CMake 自带 `FindFoo.cmake`|CMake 内置或项目自定义|

## FetchContent（在线拉取依赖，CMake 3.11+）

无需手动安装第三方库，CMake 自动从网络下载并构建：


```cmake
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)   # 下载并添加到构建

target_link_libraries(my_test PRIVATE GTest::gtest_main)
```

## 30 秒回答

`find_package` 把已安装依赖暴露为可链接的 CMake target；优先使用库提供的 Config package 和 `Foo::Bar` 目标，让 include path、编译选项和传递依赖随 target 传播。`FetchContent` 能在配置期获取源码，但会引入网络、版本和供应链边界，需要固定版本并考虑离线/CI 场景。

## 自测

1. Config mode 与 Module mode 的来源分别是什么？
2. 为什么 `target_link_libraries(myapp PRIVATE Foo::Foo)` 优于手写库文件路径？
3. 使用 `FetchContent` 时，怎样避免构建结果随远端默认分支变化？

---

## 关联笔记

- [CMakeLists․txt Structure (项目结构模板)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)
- [target_link_libraries & include_directories (依赖管理)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [CMake with vcpkg & Conan (包管理器集成)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
