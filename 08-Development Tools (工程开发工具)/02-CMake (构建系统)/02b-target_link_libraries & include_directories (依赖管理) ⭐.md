---
tags:
  - devtools/cmake
status: 🌱
---

# target_link_libraries & include_directories — 依赖管理

> [!important] **核心考点**：PRIVATE / PUBLIC / INTERFACE 的区别、现代 CMake 的 target-based 思想

## 现代 CMake 的核心思想

**以 target 为中心，而非以目录为中心。** 每个 target（可执行文件或库）管理自己的属性，依赖关系通过 target 之间传递。

```cmake
# 旧式写法（不推荐）
include_directories(include/)
link_libraries(mylib)

# 现代写法（推荐）
target_include_directories(myapp PRIVATE include/)
target_link_libraries(myapp PRIVATE mylib)
```

---

## PRIVATE / PUBLIC / INTERFACE

这是现代 CMake 中最重要的概念，控制属性的**传播范围**：

|关键字|对当前 target 生效|传播给依赖当前 target 的 target|
|---|---|---|
|PRIVATE|✅|❌|
|PUBLIC|✅|✅|
|INTERFACE|❌|✅|

### 示例场景

```cmake
# mylib 库
target_include_directories(mylib
    PUBLIC  include/        # mylib 自己用，链接 mylib 的 target 也自动获得
    PRIVATE src/internal/   # 只有 mylib 自己的编译单元能看到
)

target_link_libraries(mylib
    PUBLIC  fmt::fmt         # mylib 和所有链接 mylib 的 target 都链接 fmt
    PRIVATE spdlog::spdlog  # 只有 mylib 自己链接 spdlog
)

# myapp 链接 mylib 后，自动获得 mylib 的 PUBLIC include 路径和 PUBLIC 链接库
target_link_libraries(myapp PRIVATE mylib)
```

**判断用哪个的经验法则：**

- 头文件在 `include/`（对外暴露）→ `PUBLIC`
- 头文件在 `src/`（内部实现）→ `PRIVATE`
- 纯头文件库（header-only）→ `INTERFACE`

---

## target_compile_options & target_compile_definitions


```cmake
target_compile_options(myapp PRIVATE
    -Wall -Wextra -Wpedantic   # 开启警告
    $<$<CONFIG:Debug>:-g -O0>  # Debug 模式额外选项（生成器表达式）
)

target_compile_definitions(myapp PRIVATE
    VERSION_MAJOR=1
    $<$<CONFIG:Debug>:DEBUG_MODE>   # Debug 模式下定义 DEBUG_MODE 宏
)
```

---

## 关联笔记

- [CMakeLists․txt Structure (项目结构模板)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)
- [Build Types：Debug, Release, RelWithDebInfo (构建类型)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02c-Build%20Types：Debug,%20Release,%20RelWithDebInfo%20(构建类型)%20⭐.md)
- [find_package & External Dependencies (第三方库引入)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02d-find_package%20&%20External%20Dependencies%20(第三方库引入).md)
- [CMake with vcpkg & Conan (包管理器集成)](/08-Development%20Tools%20(工程开发工具)/02-CMake%20(构建系统)/02e-CMake%20with%20vcpkg%20&%20Conan%20(包管理器集成).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
