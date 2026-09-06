---
tags:
  - cs/fundamentals
status: 🌱
---

# Integer Representation — 整数表示

> [!important] **核心考点**：有符号 vs 无符号、补码表示范围、整数溢出、符号扩展与截断

## 有符号与无符号

| 类型 | N 位范围 | 说明 |
|------|---------|------|
| 无符号 | 0 ~ 2^N - 1 | 直接二进制表示 |
| 有符号（补码） | -2^(N-1) ~ 2^(N-1) - 1 | 最高位为符号位 |

```cpp
// C++ 中的陷阱
unsigned int u = 0;
u - 1;  // 4294967295（无符号溢出回绕）

int a = -1;
unsigned int b = 1;
a < b;  // false！a 被隐式转为 unsigned → 4294967295 > 1
```

**规则：** 有符号与无符号混合运算时，有符号隐式转为无符号。

## 整数溢出

| 溢出类型 | 现象 | 例子 |
|---------|------|------|
| 无符号回绕 | UINT_MAX + 1 = 0 | `unsigned u = UINT_MAX; u++ → 0` |
| 有符号溢出 | **未定义行为** | `INT_MAX + 1` 可能崩溃 |
| 截断溢出 | 高位丢失 | `(short)0x10001 = 1` |

**防护：**
- 用 `checked_add` / `__builtin_add_overflow` 检测溢出
- 计算前做范围检查：`if (a > INT_MAX - b) { /* 溢出 */ }`

## 符号扩展与截断

```cpp
// 符号扩展：短类型→长类型，高位补符号位
int16_t s = -16;           // 0xFFF0
int32_t i = s;             // 0xFFFFFFF0（补 1）

// 零扩展：无符号高位补 0
uint16_t us = 0xFFF0;
uint32_t ui = us;          // 0x0000FFF0（补 0）

// 截断：长→短，直接截取低位
int32_t x = 0x12345678;
int16_t y = x;             // 0x5678（高位丢失）
```

---


二进制编码与浮点数表示详见 → [Binary & Encoding（二进制与编码）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals%20(计算机基础)/02-Data%20Representation%20(数据表示)/01-Binary%20&%20Encoding%20(二进制与编码).md) · [Floating Point（浮点数）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals%20(计算机基础)/02-Data%20Representation%20(数据表示)/03-Floating%20Point%20(浮点数).md)
