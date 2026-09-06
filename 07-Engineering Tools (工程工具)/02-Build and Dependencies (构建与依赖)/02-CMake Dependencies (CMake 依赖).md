---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-CMake Dependencies (CMake 依赖)

> [!abstract] 一句话结论：CMake 依赖管理应围绕可传递 target、可固定版本和可复现安装展开；“本机能找到库”不是构建成功的标准。

## CMake External Dependencies (CMake 外部依赖)

> [!note] 本节重点：核心考点：优先消费库提供的 Config package 与 imported target；只有缺少包配置时才考虑 Module mode 或自定义 Find 脚本。

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

---

## CMake Package Managers (CMake 包管理器)

> [!note] 本节重点：核心考点：包管理器解决什么问题、vcpkg 与 Conan 的使用流程对比

## 为什么需要包管理器

手动管理 C++ 依赖的痛点：

- 不同平台安装路径不同
- 版本冲突难以解决
- 源码构建耗时且繁琐

---

## vcpkg（Microsoft，与 CMake 深度集成）

```bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh

./vcpkg/vcpkg install fmt spdlog openssl

cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

之后 CMakeLists.txt 中正常使用 `find_package`，vcpkg 自动接管：

```cmake
find_package(fmt CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE fmt::fmt)
```

**Manifest 模式**（推荐，版本锁定）：在项目根目录创建 `vcpkg.json`：

```json
{
  "name": "my-project",
  "version": "1.0.0",
  "dependencies": [
    "fmt",
    { "name": "boost-filesystem", "version>=": "1.83.0" }
  ]
}
```

---

## Conan（跨平台，配置灵活）

```bash
pip install conan
conan profile detect   # 检测当前编译器环境
```

创建 `conanfile.txt`：

```ini
[requires]
fmt/10.2.1
spdlog/1.13.0

[generators]
CMakeDeps
CMakeToolchain
```

```bash
mkdir build && cd build
conan install .. --output-folder=. --build=missing
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .
```

CMakeLists.txt 中：

```cmake
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(myapp PRIVATE fmt::fmt spdlog::spdlog)
```

---

## vcpkg vs Conan 对比

| |vcpkg|Conan|
|---|---|---|
|维护方|Microsoft|JFrog / 社区|
|CMake 集成|极深（toolchain file 即可）|需要 generator 文件|
|二进制缓存|支持（GitHub Actions 缓存）|支持（Conan Center）|
|配置灵活性|一般|高（支持不同 ABI、编译选项）|
|库数量|2000+|1800+|
|适合场景|Windows/跨平台、CMake 项目|企业级、多平台、精细控制|

---

## 关联笔记 · 延伸要点 2
- [CMakeLists․txt Structure (项目结构模板)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)
- [target_link_libraries & include_directories (依赖管理)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/04-Engineering%20Tools%20(工程工具)/02-CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)



## 零基础阅读路径

先从最短命令路径跑通一次，再回来看配置字段与高级选项。每读一段命令，都要知道它读取什么、生成什么以及怎样撤销或诊断。

## 常见误区

- 只记命令，不理解它改变了哪些输入、产物或运行环境，发生故障时无法恢复。
- 没有在临时项目中亲自执行并保留输出，就把工具流程当成已经掌握。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-CMake Dependencies (CMake 依赖)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
