#algorithm #enumeration #simulation #bruteforce

# 0. 核心

- 枚举 = **按规则把所有候选答案试出来**
- 模拟 = **按题目过程原样执行**

## 什么时候用

- 数据范围允许
- 过程规则复杂但不需要抽象成高级算法
- 暴力稍加剪枝就能过

## 高频模板

### 枚举子数组

```cpp
for (int l = 0; l < n; ++l) {
    for (int r = l; r < n; ++r) {
        // 处理区间 [l, r]
    }
}
```

### 按步骤模拟

```cpp
for (char op : ops) {
    if (op == 'L') --x;
    else if (op == 'R') ++x;
}
```

## 高频例子

### 模拟括号匹配

```cpp
bool isValid(string s) {
    stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') st.push(c);
        else {
            if (st.empty()) return false;
            char t = st.top(); st.pop();
            if ((c == ')' && t != '(') ||
                (c == ']' && t != '[') ||
                (c == '}' && t != '{')) return false;
        }
    }
    return st.empty();
}
```

### 枚举三元组

```cpp
for (int i = 0; i < n; ++i)
    for (int j = i + 1; j < n; ++j)
        for (int k = j + 1; k < n; ++k) {
            // 检查三元组
        }
```