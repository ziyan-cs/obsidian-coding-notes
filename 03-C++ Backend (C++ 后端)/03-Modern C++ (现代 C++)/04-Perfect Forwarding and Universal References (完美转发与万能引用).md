---
study_stage: backlog
---

> [!abstract] 学习目标
> 能区分转发引用与普通右值引用，按推导结果解释引用折叠，并在包装函数中保持调用方的值类别和返回类型。

# 为什么 `T&&` 有时能接收左值

只有 `T` 在该位置**由实参推导**时，`T&&` 才是转发引用（forwarding reference，也常叫 universal reference）。`std::vector<int>&&` 中类型已确定，是普通右值引用；类模板成员里固定的 `T&&` 也未必是转发引用。

| 调用实参 | `T` 推导为 | 折叠后参数类型 |
| --- | --- | --- |
| `int x; f(x)` | `int&` | `int& && → int&` |
| `f(42)` | `int` | `int&&` |
| `const int cx; f(cx)` | `const int&` | `const int&` |

折叠规则可记成：只要参与折叠的一方是 `&`，结果就是 `&`。`auto&&` 在推导上下文中也有类似行为，但 `auto&&` 与花括号初始化等场景还需单独判断，不应把所有 `&&` 都叫万能引用。

## `std::forward` 只恢复实参原来的值类别

参数即使声明为 `T&&`，在函数体内有名字的 `value` 表达式仍是左值。直接传 `value` 会丢掉右值信息；无条件 `std::move(value)` 又会把调用方的左值也转换成右值。

```cpp
#include <utility>

constexpr int category(const int&) { return 1; }
constexpr int category(int&&) { return 2; }

template<class T>
constexpr int relay(T&& value) {
    return category(std::forward<T>(value));
}

static_assert(relay(42) == 2);
constexpr int input = 7;
static_assert(relay(input) == 1);
```

`std::forward` 自身不移动资源，只按 `T` 做条件化转换。被调函数若选择移动构造，转发后的对象可能被取走资源；是否还能依赖其原值要看下游契约。

# 包装构造与调用

转发参数包常用于工厂。以下封装仅用来说明机制，实际代码应直接调用 `std::make_unique`：

```cpp
#include <memory>
#include <utility>

template<class T, class... Args>
std::unique_ptr<T> make_owned(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

int main() {
    auto value = make_owned<int>(42);
    return *value == 42 ? 0 : 1;
}
```

通用可调用对象建议用 `std::invoke`，它还支持成员函数指针。若包装器要保留“返回值还是引用”，可直接返回调用表达式：

```cpp
#include <functional>
#include <type_traits>
#include <utility>

template<class F, class... Args>
decltype(auto) forward_call(F&& f, Args&&... args) {
    return std::invoke(std::forward<F>(f),
                       std::forward<Args>(args)...);
}

int by_value() { return 7; }
int& by_ref() { static int value = 8; return value; }

static_assert(std::is_same_v<decltype(forward_call(by_value)), int>);
static_assert(std::is_same_v<decltype(forward_call(by_ref)), int&>);
```

`decltype(auto)` 的细节不能靠“局部变量是左值”一句话判断：`return result;` 中未加括号的名字按 **`decltype(result)`** 推导，而 `return (result);` 按表达式值类别推导，后者若引用了局部值对象就可能悬垂。对 `void` 返回、移动独占对象、引用返回与异常路径都要分别测试；因此计时包装器不应随手在返回前插入一个局部 `result`。

> [!note] 使用边界
> 只有下游 API 真正区分左值/右值时，才需要转发引用。普通只读借用优先用 `const T&` 或合适的视图；需要转移所有权时把参数契约写明确。转发不是性能咒语。

参考：[标准草案：`std::forward`](https://eel.is/c++draft/forward)、[`decltype` 规则](https://eel.is/c++draft/dcl.type.decltype)。
