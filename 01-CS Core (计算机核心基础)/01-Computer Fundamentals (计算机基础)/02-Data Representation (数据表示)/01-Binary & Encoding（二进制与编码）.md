---
tags:
  - cs/fundamentals
status: 🌱
---

> [!important] **核心考点**：进制转换、原码/反码/补码、ASCII 与 Unicode、UTF-8 编码规则

## 进制转换

```cpp
// 任意进制转十进制：按权展开
// 例如：0b1101 = 1*8 + 1*4 + 0*2 + 1*1 = 13
int toDecimal(string_view num, int base) {
    int val = 0;
    for (char c : num) val = val * base + (isdigit(c) ? c - '0' : c - 'a' + 10);
    return val;
}

// 十进制转 n 进制：除基取余法
string fromDecimal(int val, int base) {
    string digits;
    while (val > 0) {
        int r = val % base;
        digits.push_back(r < 10 ? '0' + r : 'a' + r - 10);
        val /= base;
    }
    reverse(digits.begin(), digits.end());
    return digits.empty() ? "0" : digits;
}
```

| 进制 | 前缀 | 例子 |
|------|------|------|
| 二进制 | `0b` | `0b1101 = 13` |
| 八进制 | `0` | `015 = 13` |
| 十进制 | 无 | `13` |
| 十六进制 | `0x` | `0xD = 13` |

## 原码、反码、补码

| 编码 | 正数 | 负数 |
|------|------|------|
| 原码 | 符号位 0 + 数值位 | 符号位 1 + 数值位 |
| 反码 | 同原码 | 原码符号位不变，数值位取反 |
| 补码 | 同原码 | 反码 + 1 |

**补码设计的精妙之处：** 将减法转为加法，`x - y = x + (~y + 1)`，CPU 只需加法器。

## ASCII 与 Unicode

- **ASCII**：7 位编码（0-127），表示英文字母、数字、控制字符
- **Unicode**：统一字符集，为世界上每种语言的每个字符分配唯一码点（U+xxxx）
- **UTF-8**：Unicode 的可变长度编码（1-4 字节），向后兼容 ASCII

### UTF-8 编码规则

| 码点范围 | 字节数 | 编码格式 |
|----------|--------|----------|
| U+0000 ~ U+007F | 1 | `0xxxxxxx` |
| U+0080 ~ U+07FF | 2 | `110xxxxx 10xxxxxx` |
| U+0800 ~ U+FFFF | 3 | `1110xxxx 10xxxxxx 10xxxxxx` |
| U+10000 ~ U+10FFFF | 4 | `11110xxx 10xxxxxx 10xxxxxx 10xxxxxx` |

**UTF-8 的优势：**
- ASCII 文本也是合法的 UTF-8（兼容性）
- 无字节序问题（BOM 可选）
- 自同步：丢失一个字节不会影响后续字符

---


整数与浮点数表示详见 → [Integer Representation（整数表示）](/01-CS%20Core%20(计算机核心基础)/01%20·%20Computer%20Fundamentals（计算机基础）/02-Data%20Representation（数据表示）/02-Integer%20Representation（整数表示）.md) · [Floating Point（浮点数）](/01-CS%20Core%20(计算机核心基础)/01%20·%20Computer%20Fundamentals（计算机基础）/02-Data%20Representation（数据表示）/03-Floating%20Point（浮点数）.md)
