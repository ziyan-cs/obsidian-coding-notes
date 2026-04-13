

# 0. 核心

- 问题：主串 S 、模式串 P ，在 S 中找到 P 第一次出现的位置

# 1. BF 算法（暴力匹配）

- 逐位对齐比对，匹配失败则主串指针回溯、模式串指针归零，重新比对
- 时间复杂度：$O(nm)$（最坏情况：主串全是 aaaaa，模式串是 aaab）

```cpp
int bfMatch(string s, string p) {
    int n = s.size(), m = p.size();
    int i = 0, j = 0;
    while (i < n && j < m) {
        if (s[i] == p[j]) {
            i++, j++;
        } else {
            i = i - j + 1; // 主串回溯到下一个起始位置
            j = 0;          // 模式串归零
        }
    }
    return j == m ? i - j : -1;
}
```

# 2. KMP 算法（优化解法）

- 主串指针不回溯，利用模式串的「前缀后缀最长公共部分」（next 数组），跳过无效比对
- 时间复杂度：$O(n+m)$（预处理 next 数组 O (m)，匹配 O (n)）
- 核心：next 数组（部分匹配表）的定义与求解

```cpp
// 1. 求 next 数组
void getNext(string p, int next[]) {
    int m = p.size();
    next[0] = -1;
    int i = 0, j = -1;

    while (i < m - 1) {
        if (j == -1 || p[i] == p[j]) {
            i++, j++;
            next[i] = j;
        } else {
            j = next[j];
        }
    }
}

// 2. KMP 匹配主函数
int kmp(string s, string p, int next[]) {
    int n = s.size();
    int m = p.size();
    int i = 0, j = 0;

    while (i < n && j < m) {
        if (j == -1 || s[i] == p[j]) {
            i++, j++;
        } else {
            j = next[j];
        }
    }
    
    if (j == m) return i - j; // 匹配成功，返回起始下标
    else return -1;           // 匹配失败
}
```

# 3. BF 与 KMP 核心对比（可以加在笔记里）

| 特性    | BF 算法       | KMP 算法             |
| ----- | ----------- | ------------------ |
| 思路    | 暴力逐位比对，失败回溯 | 利用前缀信息，主串不回溯       |
| 时间复杂度 | $O(n·m)$    | $O(n+m)$           |
| 空间复杂度 | $O(1)$      | $O(m)$（存储 next 数组） |
| 适用    | 短文本、小规模匹配   | 长文本、大规模匹配          |
| 缺点    | 重复比对多，效率低   | 逻辑稍复杂，需预处理 next 数组 |
