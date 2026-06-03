
> **核心考点**：PRIVATE / PUBLIC / INTERFACE 的区别、现代 CMake 的 target-based 思想

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