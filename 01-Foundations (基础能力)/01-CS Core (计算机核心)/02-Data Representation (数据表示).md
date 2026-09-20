---
study_stage: learn
review_due: 2026-10-03
tags: [cs/architecture, learning/foundation]
---

> [!abstract] 阅读方式
> 位模式本身没有业务含义；类型、编码和协议决定如何解释这些位。本文从整数、浮点、文本、字节序和位运算建立统一模型，并用 C++ 实验验证边界，而不是背若干孤立技巧。

> [!summary] 核心摘要
>
> 固定位宽整数按模运算，有符号数采用补码但溢出规则由语言规定；IEEE 754 浮点表达范围与精度的折中，运算包含舍入和特殊值；Unicode 区分码点、编码单元与用户看到的字符。跨进程、跨语言和跨机器传输时，必须显式约定位宽、符号、字节序、文本编码和版本。

# 位模式、类型与解释

同一组比特可以被解释为无符号整数、有符号整数、浮点数、指令、像素或字符编码。硬件保存和搬运位模式，ISA、语言类型与序列化协议赋予它含义。

例如 8 位模式 `11111111`：

| 解释方式 | 结果 |
| --- | --- |
| `uint8_t` | 255 |
| 8 位补码有符号整数 | -1 |
| 位掩码 | 8 个标志全部置位 |
| 单个 UTF-8 字节 | 不能独立构成合法字符 |

进制只是**书写方式**：二进制适合观察位，十六进制每一位对应 4 bit，十进制适合人类数量表达。改变写法不会改变内存中的值。

```cpp
unsigned value = 0b1101;
assert(value == 13);
assert(value == 015);   // 八进制；业务代码通常不建议这样写
assert(value == 0x0d);
```

# 整数表示与 C++ 运算规则

## 补码和范围

对 N 位整数：

| 类型 | 数值范围 | 运算模型 |
| --- | --- | --- |
| 无符号 | `0 ... 2^N - 1` | 算术按模 `2^N` 回绕 |
| 有符号 | `-2^(N-1) ... 2^(N-1)-1` | 表示采用补码；算术越界不能按回绕推理 |

补码让加法器统一处理正负数，并只有一个零。以 8 位为例，`-5` 的位模式为 `11111011`；把它解释为无符号数则是 251。

> [!important] 语言规则高于硬件直觉
> 当前 C++ 规定有符号整数采用补码表示，但**有符号算术溢出仍是 undefined behavior**。编译器可利用“不会溢出”的前提优化代码，不能因为机器最终会截断低位就依赖回绕。

## 转换不是简单“都变成 unsigned”

混合整数运算先进行 integer promotions，再应用 usual arithmetic conversions；结果取决于双方的位宽、rank 和可表示范围。

```cpp
std::uint32_t u = 1;
int i = -1;

// 常见 32 位 int 平台上，i 转为 uint32_t，因此条件为 false。
bool result = i < u;
```

工程上避免让符号承担两个含义：集合大小、下标和差值不要随意混用；转换前先检查范围，可使用 `std::in_range<T>`（C++20）。

## 溢出、扩展与截断

```cpp
std::uint32_t u = UINT32_MAX;
++u;                           // 明确定义：回到 0

int x = INT_MAX;
// ++x;                        // 未定义行为，不要执行

std::int16_t small = -16;
std::int32_t wide = small;     // 保持数值，典型实现做符号扩展

std::uint32_t bits = 0x12345678u;
std::uint16_t low = static_cast<std::uint16_t>(bits); // 低 16 位
```

对外部输入做加法、乘法和容量计算时，应在运算前验证边界，或使用编译器的 checked-overflow intrinsic。不要写 `a + b > MAX` 再检测，因为溢出可能已经发生。

# 浮点数是近似数系统

## IEEE 754 binary32 与 binary64

| 格式 | 符号 | 指数字段 | fraction 字段 | 常见 C++ 类型 |
| --- | ---: | ---: | ---: | --- |
| binary32 | 1 bit | 8 bit | 23 bit | 通常为 `float` |
| binary64 | 1 bit | 11 bit | 52 bit | 通常为 `double` |

对**正规数**，数值可概括为：

```text
(-1)^sign × (1.fraction) × 2^(exponent - bias)
```

这个公式不覆盖全部编码。指数全零用于零和 subnormal，指数全一用于 infinity 与 NaN。subnormal 让数值靠近零时逐步丢失精度，而不是突然从最小正规数跳到零。

## 精度、舍入和 ULP

`0.1` 不能被有限二进制小数精确表示，存储的是邻近可表示值。误差大小与数值尺度、运算顺序和舍入模式有关。

```cpp
double a = 0.1 + 0.2;
double b = 0.3;
std::cout << std::setprecision(17) << a << '\n' << b << '\n';
```

“浮点永远不能用 `==`”同样是错误规则：比较同一赋值结果、离散哨兵或预期精确的整数范围值时可以相等比较；对计算结果则应按问题选择绝对误差、相对误差或 ULP 策略。

```cpp
bool nearly_equal(double a, double b,
                  double rel = 1e-12,
                  double abs = 1e-15) {
    double diff = std::abs(a - b);
    return diff <= std::max(abs, rel * std::max(std::abs(a), std::abs(b)));
}
```

需要额外处理：

- `NaN` 与任何值（包括自身）比较都不相等，应使用 `std::isnan`。
- `+0.0 == -0.0` 为真，但某些运算和 `std::signbit` 能观察符号。
- 加法和乘法一般不满足结合律；并行归约可能改变末位结果。
- 金额和精确小数规则通常应使用定点整数或十进制类型，而非裸 `double`。

# 文本：码点不等于用户看到的字符

## Unicode 与 UTF-8

Unicode 为字符分配**码点（code point）**，UTF-8/UTF-16/UTF-32 是把码点编码成字节或编码单元的方式。UTF-8 使用 1～4 字节，ASCII 的 0～127 与 UTF-8 完全兼容。

| 码点范围 | UTF-8 字节模式 |
| --- | --- |
| U+0000–U+007F | `0xxxxxxx` |
| U+0080–U+07FF | `110xxxxx 10xxxxxx` |
| U+0800–U+FFFF | `1110xxxx 10xxxxxx 10xxxxxx` |
| U+10000–U+10FFFF | `11110xxx 10xxxxxx 10xxxxxx 10xxxxxx` |

但“一个字符”至少有三种含义：

- byte：存储单位；
- code point：Unicode 抽象字符编号；
- grapheme cluster：用户感知的一个书写单元，可能由多个码点组成。

因此 UTF-8 字符串的字节数、码点数和界面光标移动次数可能都不同。组合字符还会产生视觉相同但字节不同的字符串，搜索、用户名和安全比较要考虑 normalization。

## 边界处理原则

- 文件和协议明确声明 UTF-8，不依赖进程默认编码。
- 解码时拒绝或显式替换非法序列，不能悄悄逐字节处理。
- 截断文本要按 grapheme cluster 或至少按码点边界，不能截断到 UTF-8 连续字节中间。
- 对标识符、路径和数据库键，先定义 normalization 与大小写策略。

# 字节序、对齐与序列化

## Endianness（字节序）

字节序描述多字节标量的字节在内存中的排列，不影响单字节。以 `0x12345678` 为例：

```text
低地址 → 高地址
little-endian: 78 56 34 12
big-endian:    12 34 56 78
```

x86-64 使用 little-endian；Arm 和 RISC-V 的具体执行环境通常也采用 little-endian，但协议不能依赖“大家都一样”。传统 IP 网络字段使用 network byte order（big-endian）。

C++20 可用 `std::endian` 检查本机端序；C++23 提供 `std::byteswap`。不要通过违反 strict aliasing 的指针强转读取对象表示，优先使用 `std::bit_cast`、`std::memcpy` 或明确的逐字节编码。

## 对齐不是序列化格式

对象布局可能包含 padding，且受 ABI、编译器选项和成员类型影响。把结构体内存直接写入网络或文件会泄漏 padding、绑定本机端序，并造成版本兼容问题。

```cpp
struct Header {
    std::uint16_t version;
    std::uint32_t length;
};

// sizeof(Header) 可能大于 6；不可直接 send(&header, sizeof header, ...)
```

正确做法是逐字段定义：位宽、端序、合法范围、缺省值和版本演进。对齐优化服务于内存访问；wire format 服务于跨边界兼容，两者目标不同。

# 位运算：表达位集合而非炫技

```cpp
std::uint32_t mask = 0;
mask |=  (1u << 5);          // set bit 5
mask &= ~(1u << 5);          // clear bit 5
mask ^=  (1u << 5);          // toggle bit 5
bool set = (mask & (1u << 5)) != 0;
```

安全边界：

- 移位数必须非负且小于左操作数提升后的位宽。
- 对位模式优先使用无符号类型，避免符号位和溢出规则干扰。
- C++20 规定有符号负数右移执行算术右移，但跨旧标准或跨语言代码仍应明确假设。
- `x & -x` 若使用有符号最小值可能触发溢出；位技巧应在无符号类型上实现。
- 不要手工把乘除改写成移位来“加速”；现代优化器会在语义允许时处理，手写转换容易破坏负数、溢出和可读性。

C++20 `<bit>` 已提供 `std::popcount`、`std::has_single_bit`、`std::rotl` 等意图清晰的接口，应优先于难读的技巧。

# 实验与掌握标准

## 可运行观察

```cpp
#include <bit>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>

int main() {
    float f = 0.1f;
    auto raw = std::bit_cast<std::uint32_t>(f);
    std::cout << std::bitset<32>(raw) << '\n';
    std::cout << std::hex << raw << '\n';
    std::cout << (std::endian::native == std::endian::little) << '\n';
}
```

再用 sanitizer 验证有符号溢出：

```bash
g++ -std=c++20 -O2 -Wall -Wextra -fsanitize=undefined demo.cpp
./a.out
```

完成后应能回答：

1. 为什么相同位模式能表示完全不同的值？
2. 为什么补码表示不等于“有符号溢出可以回绕”？
3. 什么时候浮点 `==` 合理，什么时候需要相对误差？
4. 为什么 UTF-8 的 `size()` 不能代表用户看到的字符数？
5. 为什么内存中的 C++ 结构体不是稳定的网络协议？

> [!warning] 常见误区
> - 把类型宽度、端序和 `char` 是否有符号当成所有平台固定事实。
> - 把 epsilon 写死为一个绝对常数，用于所有数量级的浮点比较。
> - 以为 Unicode 码点、UTF-8 字节和界面字符是一一对应。
> - 为了“位运算更快”牺牲语义和可读性，却没有任何 benchmark。

# 资料与后续

- [C++ working draft: fundamental types](https://eel.is/c++draft/basic.fundamental)
- [C++ working draft: shift operators](https://eel.is/c++draft/expr.shift)
- [The Unicode Standard — latest version](https://www.unicode.org/versions/latest/)
- 下一步：[Program Build and Execution (程序构建与执行)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)
