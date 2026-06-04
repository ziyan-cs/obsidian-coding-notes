---
tags:
  - devtools
  - cmake
---

> **核心考点**：四种构建类型的使用场景、优化级别、常用配置


## 四种标准构建类型

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
```

|类型|编译器标志（GCC/Clang）|用途|
|---|---|---|
|Debug|`-g -O0`|开发调试，包含符号表，不优化|
|Release|`-O3 -DNDEBUG`|生产发布，最大优化，禁用 assert|
|RelWithDebInfo|`-O2 -g -DNDEBUG`|生产环境调试，有符号表但也优化|
|MinSizeRel|`-Os -DNDEBUG`|嵌入式/资源受限，最小体积|

> `NDEBUG` 宏会禁用 `assert()`，Release 模式下断言失效，需注意。

## 在 CMake 中按构建类型设置行为

```cmake
# 生成器表达式（Generator Expression）
target_compile_options(myapp PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address>       # Debug 下开 ASan
    $<$<CONFIG:Release>:-march=native>          # Release 下针对本机 CPU 优化
)

# if 判断（配置时执行，不如生成器表达式灵活）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(myapp PRIVATE ENABLE_LOGGING)
endif()
```

## 多配置生成器（Visual Studio / Xcode / Ninja Multi-Config）

```bash
# 配置时不指定类型
cmake -G "Ninja Multi-Config" ..
# 构建时指定
cmake --build . --config Release
cmake --build . --config Debug
```

---

## 关联笔记

- [CMakeLists․txt Structure (项目结构模板)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)
- [target_link_libraries & include_directories (依赖管理)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02b-target_link_libraries%20&%20include_directories%20(依赖管理)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [CMake with vcpkg & Conan (包管理器集成)](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
