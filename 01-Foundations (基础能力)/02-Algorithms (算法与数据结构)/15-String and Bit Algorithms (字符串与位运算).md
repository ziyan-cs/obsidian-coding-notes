---
study_stage: backlog
tags: [algorithm/string, algorithm/bit-manipulation, learning/foundation]
---

> [!abstract] 学习目标
> 理解 KMP 前缀函数如何复用已匹配信息，掌握字符串算法的编码边界，并安全使用掩码、移位、异或和子集枚举。

# KMP：失败后不回退文本指针

朴素匹配在失配后可能重复比较。KMP 为模式串预处理前缀函数 `pi[i]`：子串 `pattern[0..i]` 的最长真前缀长度，且该前缀也是后缀。

## 构建前缀函数

```cpp
std::vector<int> prefix_function(std::string_view pattern) {
    std::vector<int> pi(pattern.size());
    for (std::size_t i = 1; i < pattern.size(); ++i) {
        int j = pi[i - 1];
        while (j > 0 && pattern[i] != pattern[j]) j = pi[j - 1];
        if (pattern[i] == pattern[j]) ++j;
        pi[i] = j;
    }
    return pi;
}
```

失配时令 `j = pi[j-1]`，表示尝试当前已匹配前缀的最长候选 border。`j` 严格减小，因此预处理总复杂度为 `O(m)`。

## 搜索

```cpp
std::size_t kmp_find(std::string_view text, std::string_view pattern) {
    if (pattern.empty()) return 0;
    auto pi = prefix_function(pattern);
    int j = 0;
    for (std::size_t i = 0; i < text.size(); ++i) {
        while (j > 0 && text[i] != pattern[j]) j = pi[j - 1];
        if (text[i] == pattern[j]) ++j;
        if (j == static_cast<int>(pattern.size()))
            return i + 1 - pattern.size();
    }
    return std::string_view::npos;
}
```

时间 `O(n+m)`、额外空间 `O(m)`。找全部匹配时，记录结果后令 `j = pi[j-1]` 以允许重叠匹配。

# 周期与其他字符串结构

长度为 `n` 的字符串若 `period = n - pi[n-1]` 且 `n % period == 0`，则可由长度为 `period` 的片段重复构成。这个结论依赖 border 与周期关系，不是对任意 `pi` 值都直接成立。

其他常用结构：Trie 适合前缀检索；rolling hash 可快速比较子串但存在碰撞，安全敏感场景需双哈希或最终字符比较；Z-function 记录从每个位置开始与整个字符串前缀的匹配长度。应按查询类型选择，不必把所有问题都改写成 KMP。

字符串代码还要声明编码语义。`std::string` 是字节序列；按下标遍历 UTF-8 得到代码单元，而不是必然得到 Unicode code point 或用户感知字符。

# 位运算的类型边界

| 表达式 | 含义 |
|---|---|
| `x & mask` | 保留 mask 中为 1 的位 |
| `x \| mask` | 设置位 |
| `x & ~mask` | 清除位 |
| `x ^ mask` | 翻转位 |
| `x & (x - 1)` | 清除最低位的 1（`x != 0`） |
| `x & -x` | 提取最低位的 1，建议在无符号类型上使用 |

移位必须使用合适的无符号类型：位数不能为负，也必须小于类型宽度；对负有符号数移位和溢出相关表达式不要依赖平台偶然行为。

```cpp
std::uint64_t bit = std::uint64_t{1} << index; // 先把 1 提升到目标宽度
bool set = (value & bit) != 0;
```

C++20 `<bit>` 提供 `std::popcount`、`std::countl_zero`、`std::countr_zero`、`std::bit_width` 等函数，通常比手写技巧更清楚。其参数主要是无符号整数类型。

# 异或的能力与限制

异或满足交换律、结合律，且 `x ^ x = 0`。因此在“除一个元素出现一次，其余恰好出现两次”的严格前提下，可异或得到单独元素。前提改变后结论不成立；异或不能替代通用计数。

两个元素各出现一次时，先异或得到 `diff = a ^ b`，取其中一个置位把数组分组，再分别异或。`diff` 必须非零，取最低位时优先使用无符号类型。

# 状态压缩与子集枚举

当元素数 `n` 较小，可用一个整数的第 `i` 位表示元素是否在集合中。所有子集数量是 `2^n`，这仍是指数复杂度。

枚举某个掩码的所有非空子集：

```cpp
for (std::uint64_t sub = mask; sub != 0; sub = (sub - 1) & mask) {
    // sub 是 mask 的一个非空子集
}
// 空集需要按题意单独处理
```

所有 mask 的全部子集枚举总复杂度为 `O(3^n)`，不是 `O(2^n)`。位压缩 DP 适合 `n` 较小且状态由集合决定的问题；先根据内存 `2^n × state_size` 估算上限。

# 检查理解

1. KMP 失配时为什么跳到 `pi[j-1]`，而不是把 `j` 清零？
2. 找重叠匹配后为什么还要继续回退前缀函数？
3. `1 << index` 在大下标时有什么类型风险？
4. 子集枚举总复杂度为何可能是 `3^n`？

> [!summary] 本篇结论
> KMP 用模式串自身的 border 复用匹配历史；位运算用固定宽度整数压缩集合和状态。两者都依赖精确语义：字符串要声明编码，位操作要声明宽度、符号与输入前提。

回到：[00-Foundations Standard (基础能力标准与自测)](/01-Foundations%20(基础能力)/00-Foundations%20Standard%20(基础能力标准与自测).md)
