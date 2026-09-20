---
study_stage: learn
review_due: 2026-10-22
tags: [engineering/build, cpp/dependencies]
---

> [!abstract] 学习目标
> 区分“发现已有包”“配置时引入源码”和“解析安装包”，能安全地消费 imported target，并解释依赖版本、ABI 与网络边界。

# 先看依赖究竟从哪来

| 路径 | 做什么 | 负责固定依赖图的是谁 |
|---|---|---|
| `find_package` | 发现配置好的包并导入 target | 外部安装/包管理流程 |
| `FetchContent` | 配置期获取源码并纳入当前构建 | 项目声明的来源与精确修订 |
| vcpkg/Conan 等 | 解析、构建或安装依赖 | manifest、baseline、profile/lock 等 |

`find_package` 本身不从包仓库“安装指定版本”。它可能使用 package-provided Config 文件、CMake 自带或项目提供的 Find 模块，也可能被 dependency provider 拦截；搜索顺序随调用形式、变量和 CMake 版本变化。不能声称 Config 模式总是自动优先。

# 消费已经安装的包

```cmake
find_package(fmt CONFIG REQUIRED)
add_executable(myapp src/main.cpp)
target_link_libraries(myapp PRIVATE fmt::fmt)
```

`fmt::fmt` 是包导出的 imported target，可携带 include 目录、编译定义和链接要求；`find_package` 找到包不意味着它兼容当前 ABI 或一定定义想象中的目标名，须以包文档和实际配置为准。

环境确定后可通过 `CMAKE_PREFIX_PATH` 或相应 toolchain 告诉 CMake 安装前缀。查找失败时先核对：包是否安装、Config 文件在哪、配置架构是否一致、当前使用哪个 CMake 与编译器；必要时启用 `cmake --debug-find`，不要盲目向源码写死 `/usr/local/lib`。

对外发布库时，导出的 Config 应能带入传递依赖；消费端不应自行猜出上游库需要哪些 include 与 link flags。

# FetchContent：在配置期引入源码

```cmake
include(FetchContent)
FetchContent_Declare(
    SomeLib
    GIT_REPOSITORY https://example.invalid/somelib.git
    GIT_TAG        <audited-full-commit-id>
)
FetchContent_MakeAvailable(SomeLib)
```

以上地址和哈希是**结构示意**，不可原样执行。实际项目应使用经核实的上游来源和不可变完整修订，记录许可与安全审查。固定 tag 名称不等于固定内容；用分支名更会随时间漂移。

FetchContent 的源码在配置期即可加入构建；它不是通用“包管理器”，不会自动提供完整版本解析、二进制包和 lock 机制。CI 应考虑网络失败、离线镜像和缓存可信性。较新 CMake 支持 `FIND_PACKAGE_ARGS`、dependency provider 等组合方式；具体行为应以项目最低支持版本对应文档验证。

# vcpkg：项目 manifest 与 registry baseline

```json
{
  "name": "example-app",
  "version-string": "0.1.0",
  "builtin-baseline": "<pinned-registry-commit>",
  "dependencies": ["fmt"]
}
```

`vcpkg.json` 描述直接依赖；baseline 约束 registry 视图，`version>=` 是版本下限，不是精确锁定。要重建还需保留 registry 来源/修订、triplet、features、工具链和必要构建选项。不要把“manifest 模式”直接写成“已经锁死全部二进制”。

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

toolchain file 应在**首次配置**时提供；已配置目录中事后更换它可能继续使用旧缓存。示例路径由使用者替换，非真实命令行路径。

# Conan 2：profile 与生成文件

Conan 2 可用 `CMakeToolchain`、`CMakeDeps` 生成 CMake 可消费的工具链与 Config 文件，并用 host/build profile 表达目标与构建机器的环境差异。lockfile 用于约束依赖解析结果；仅写版本范围仍会变化。

```ini
[requires]
fmt/<chosen-version>

[generators]
CMakeDeps
CMakeToolchain
```

上面的版本为占位符：实践时从包仓库确认版本及许可证，并按 Conan 2 当前文档执行 `conan install`；不同 generator、平台和配置的生成目录需核对。不要把 Conan 1 的命令或长期过时的包版本当作通用模板。

# ABI 与冲突排查

依赖“版本相同”也可能因编译器、标准库、Debug/Release、目标架构、C++ ABI 或静态/动态链接选项不同而不能混用。典型症状：头文件正常但 undefined reference、运行时找不到动态库、链接成功后崩溃。

排查顺序：

1. 画出直接与传递依赖图，确认版本是否真的解析为同一组；
2. 记录目标平台、编译器、标准库、构建类型与包选项；
3. 查 `find_package` 找到的 Config 来源与 imported target 属性；
4. 全新 build 目录、清理可疑包缓存后重现；
5. 若库暴露类型跨二进制边界，核对 ABI 和运行时依赖，而非盲目追加 `-L`。

# 最小实验

为一个只有 fmt 依赖的程序分别使用系统安装的 Config 包与一个包管理器配置；记录 `fmt::fmt` 的头文件来源、最终链接命令和依赖版本。切换来源时使用独立 build 目录，并通过程序运行验证，不仅看“配置成功”。

# 检查理解

1. `find_package`、FetchContent 和包管理器分别负责什么？
2. `version>=` 与精确解析结果有什么区别？
3. 为什么更换 toolchain 后应新建构建目录？
4. 同版本库链接失败时，除了版本号还应检查哪些 ABI 输入？

> [!summary] 本篇结论
> 依赖管理有三个不同层次：解析来源与版本、把依赖安装/构建好、让 CMake 消费其 target。明确每层责任和工具链 ABI，比“我本机能 find 到”更重要。

## 权威依据

- [CMake find_package](https://cmake.org/cmake/help/latest/command/find_package.html)
- [CMake FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
- [vcpkg versioning](https://learn.microsoft.com/en-us/vcpkg/users/versioning)
- [Conan 2 lockfiles](https://docs.conan.io/2/tutorial/versioning/lockfiles.html)

下一步：[03-Package Management and Reproducible Builds (包管理与可复现构建)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/03-Package%20Management%20and%20Reproducible%20Builds%20(包管理与可复现构建).md)
