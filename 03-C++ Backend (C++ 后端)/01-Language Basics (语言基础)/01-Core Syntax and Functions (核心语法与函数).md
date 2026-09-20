---
study_stage: backlog
---

> [!abstract] 学习目标
> 用对象、类型、初始化、表达式、控制流与函数组成可运行的小程序，并解释每个边界。示例以 C++17 为基线。

# 对象、类型与初始化

声明引入名称；对象有类型、存储和生命周期。类型决定可表示的值及合法操作。不要把常见机器上的大小当成语言保证：`sizeof(char) == 1`，但一个 byte 有多少位由 `CHAR_BIT` 决定；`int`、`long`、`long double` 的宽度依实现而定。精确宽度需求可查看 `<cstdint>` 的 `std::int32_t` 等类型；容器大小与下标通常用 `std::size_t`。

```cpp
#include <iostream>
#include <limits>

int main() {
    int count{3};              // 列表初始化拒绝缩窄
    double ratio{0.5};
    const int limit{10};       // 不能通过此名称修改
    std::cout << count << ' ' << ratio << ' ' << limit << '\n';
    std::cout << sizeof(int) << ' ' << std::numeric_limits<int>::max() << '\n';
}
```

局部 `int n;` 可能未初始化；`int n{};` 值初始化为零。`int n{3.5};` 因缩窄而无法编译。字符串字面量 `"hello"` 是 `const char[6]`，只在很多表达式中转换为首元素指针，不是天生的 `const char*`。普通 `char` 的有符号性由实现决定，不能用 `char c = 200;` 推断统一结果。

## 运算、转换与溢出

无符号整数运算按模数回绕；有符号溢出是未定义行为。混用有符号与无符号数时先明确转换规则，`-1 < 1u` 不能靠直觉判断。“char → short → int → long → float”不是通用转换链：整型提升和 usual arithmetic conversions 要分开理解。

```cpp
unsigned int quota{10};
int delta{-1};
if (delta >= 0) quota += static_cast<unsigned int>(delta);

int flags{0x100};
if ((flags & 0xFF) == 0) { /* 低八位全为零 */ }
```

给混合位运算和比较加括号。`&&`、`||` 短路，`&`、`|` 不短路；不要在一个表达式中反复修改同一变量。

# 控制流与循环不变量

`if` 选择分支，`switch` 对整型或枚举多路选择，`for` 适合遍历，`while` 适合状态驱动。每个循环都要说明不变量和终止条件。

```cpp
#include <vector>

int sum_positive(const std::vector<int>& values) {
    int total{};
    for (int value : values) {
        if (value <= 0) continue;
        // 真实业务还需检查 total + value 是否溢出。
        total += value;
    }
    return total;
}
```

范围 `for` 遍历大对象时可用 `const auto& item` 避免拷贝；修改元素用 `auto& item`。`break` 退出当前循环或 `switch`，`continue` 进入下一轮循环。刻意穿透 `switch` 分支时可用 C++17 的 `[[fallthrough]]`。

# 函数把约束放在接口上

函数不能只按返回类型重载。小标量通常按值传；只读大对象通常用 `const T&`；要修改调用者对象用 `T&`；可缺席且非拥有的参数可用指针。仍需结合生命周期、所有权判断，不能机械套字节阈值。

```cpp
#include <limits>
#include <optional>

std::optional<int> divide_exact(int numerator, int denominator) {
    if (denominator == 0) return std::nullopt;
    if (numerator == std::numeric_limits<int>::min() && denominator == -1)
        return std::nullopt;  // 避免有符号溢出
    if (numerator % denominator != 0) return std::nullopt;
    return numerator / denominator;
}
```

`std::optional` 明确表示可能没有整数结果。`inline` 允许符合 ODR 约束的定义出现在多个翻译单元，并不保证机器码内联展开。默认实参通常写在头文件声明处，调用点决定采用哪个默认值。递归要有基例并评估栈深；C++ 不保证尾调用优化。

## 动手验证

1. 用 `-std=c++17 -Wall -Wextra -Wconversion` 编译首段示例，改成 `int n{3.5};` 观察诊断。
2. 测试 `sum_positive` 的空数组、全负数和大正数，指出溢出边界。
3. 测试 `divide_exact` 的除零、不整除、精确整除和 `INT_MIN / -1`。

> [!info]- 延伸阅读
> - [02-Arrays Strings and IO (数组字符串与输入输出)](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/02-Arrays%20Strings%20and%20IO%20(数组字符串与输入输出).md)
> - 规范依据：[基础类型](https://eel.is/c++draft/basic.fundamental)、[列表初始化](https://eel.is/c++draft/dcl.init.list)
