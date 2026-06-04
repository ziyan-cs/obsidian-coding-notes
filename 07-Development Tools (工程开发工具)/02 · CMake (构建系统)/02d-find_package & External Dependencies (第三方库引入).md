---
tags:
  - devtools
  - cmake
---

> **核心考点**：find_package 的两种模式（Module/Config）、搜索路径、如何编写 Find 脚本

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

---

## 关联笔记

- [CMakeLists․txt Structure (项目结构模板)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)
- [target_link_libraries & include_directories (依赖管理)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [CMake with vcpkg & Conan (包管理器集成)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
