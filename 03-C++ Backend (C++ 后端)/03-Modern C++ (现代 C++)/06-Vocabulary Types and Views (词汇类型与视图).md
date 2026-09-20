---
study_stage: backlog
---

> [!abstract] 学习目标
> 给函数接口选出能准确表达“缺席、多种结果、借用数据”的类型，并能画出每个视图所依赖对象的生命周期。

# 先决定结果是值、缺席还是错误

| 需求 | 候选类型 | 不适用的情况 |
| --- | --- | --- |
| 成功时有 `T`，无结果也是正常情况 | `std::optional<T>`（C++17） | 需要区分多种失败原因 |
| 结果可能是若干封闭形态之一 | `std::variant<A, B, ...>`（C++17） | 备选类型没有清晰领域意义 |
| 成功值或明确错误 | 领域 `variant`，或 `std::expected<T,E>`（C++23） | 不应把所有失败都压成同一个哨兵值 |

## `optional`：缺席是接口状态，不是错误说明

下面的解析器要求**整个输入**都是十进制整数；空串、溢出和尾随字符都返回空状态。调用者如需区分这三类原因，就不能只用 `optional`。

```cpp
#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>

std::optional<int> parse_int(std::string_view text) {
    if (text.empty()) return std::nullopt;
    int value{};
    const char* first = text.data();
    const char* last = first + text.size();
    auto [next, error] = std::from_chars(first, last, value);
    if (error != std::errc{} || next != last)
        return std::nullopt;
    return value;
}

int main() {
    auto result = parse_int("42");
    if (!result) return 1;
    return *result == 42 ? 0 : 2;
}
```

先判断 `has_value()` 或使用条件判断再解引用；`value()` 在空状态时抛 `std::bad_optional_access`，`value_or(default)` 在空状态时返回默认值，但别让默认值掩盖必须处理的失败。`optional<T>` 自身存放一个 `T`，不会**为所含对象单独**分配内存；`T` 的内部资源仍可能动态分配。C++23 的 `transform` / `and_then` 是后续可选语法，不应混进 C++17 基础示例。

## `variant`：封闭的结果集合

`variant` 任一时刻持有一个备选类型。读取不匹配的备选时，`std::get` 抛 `std::bad_variant_access`；`std::get_if` 返回空指针。对失败原因建模时，不要把所有打开失败误写成“文件不存在”：

```cpp
#include <fstream>
#include <string>
#include <type_traits>
#include <variant>

enum class ReadError { open_failed, read_failed };
using ReadResult = std::variant<std::string, ReadError>;

ReadResult read_file(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return ReadError::open_failed;

    std::string content;
    char ch;
    while (input.get(ch)) content.push_back(ch);
    if (input.bad()) return ReadError::read_failed;
    return content;
}

int main() {
    ReadResult result = read_file("config.txt");
    return std::visit([](const auto& value) -> int {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>)
            return 0; // 包括合法的空文件
        else
            return value == ReadError::open_failed ? 1 : 2;
    }, result);
}
```

这个领域枚举还不能区分权限、路径、设备错误；若确需精确信息，应在系统调用边界收集实际错误码。`std::visit` 的 visitor 应覆盖所有备选；新增备选后应让编译器帮助发现未处理分支。异常期间 `variant` 在特定操作后可能进入 `valueless_by_exception`，不要笼统声称它永远保存一个值。

# 视图只借用，不延长底层数据寿命

`std::string_view` 借用连续字符，`std::span<T>`（C++20）借用连续元素。它们适合短期**参数**和局部观察，却不自动拥有内存；销毁、扩容、重分配或错误的跨线程使用都可能让视图失效。

```cpp
#include <span>
#include <string>
#include <string_view>
#include <vector>

void log_message(std::string_view text); // 只在调用期间使用
void change_first(std::span<int> values) {
    if (!values.empty()) values.front() = 7;
}

int main() {
    std::string owner = "hello";
    std::string_view text = owner;
    log_message(text);

    std::vector<int> numbers{1, 2, 3};
    change_first(numbers);
    // 若 numbers 后续发生导致重分配的操作，旧 span 不能再使用。
}
```

直接把临时 `std::string` 传给只在调用期间使用的 `string_view` 参数，通常是安全的；把该视图存入回调或从函数返回则可能悬垂。`string_view` **不保证末尾有 `'\0'`**，传给 C API 前要确认边界或构造拥有的 `std::string`。`span` 同理，不应返回指向局部 `vector` 的视图。可变 `span<int>` 允许修改元素；`span<const int>` 只读。必要时让接口接收拥有值的容器或明确共享所有权。

# 结构化绑定：看清是否复制

`auto [a,b]` 默认按值生成绑定对象；`auto& [a,b]` 绑定原对象。遍历 map 时，若只读，常用 `const auto&` 避免复制 key/value：

```cpp
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores{{"Alice", 95}};
    for (const auto& [name, score] : scores) {
        (void)name;
        (void)score;
    }

    auto [it, inserted] = scores.insert({"Bob", 87});
    if (inserted) it->second += 1;
}
```

按引用绑定要继续遵守原对象的生命周期和迭代器失效规则。结构化绑定只是解包语法，不会自动把数据变成独立安全的副本。

> [!note] 接口选择顺序
> 先问“拥有还是借用”，再问“失败需不需要原因”，最后才选方便的语法。短生命周期的只读参数可用视图；跨作用域保存时明确所有权；可恢复错误要返回足够诊断信息。

参考：[C++ 标准库 `optional`](https://eel.is/c++draft/optional)、[`variant`](https://eel.is/c++draft/variant)、[`string_view`](https://eel.is/c++draft/string.view)、[`span`](https://eel.is/c++draft/views.span)。
