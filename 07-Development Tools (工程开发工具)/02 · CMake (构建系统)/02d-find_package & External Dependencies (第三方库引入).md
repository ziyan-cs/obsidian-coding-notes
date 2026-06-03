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