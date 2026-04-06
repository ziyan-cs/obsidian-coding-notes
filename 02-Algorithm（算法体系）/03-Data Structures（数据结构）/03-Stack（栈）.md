#data-structure #stack #lifo #linear-structure #cpp

## ⚡ TL;DR（快速决策）

- **后进先出（LIFO）的受限线性结构**
- 相关特征：
    - 最近加入的元素先处理
    - 括号匹配
    - 表达式求值
    - 撤销 / 回退逻辑
    - 单调栈基础模型

## 🧩 Core Idea（核心本质）

- 栈只能在顶部做操作：
    - `push`
    - `pop`
    - `top`
- 一句话理解：
    - **像一叠盘子，最后放上去的最先拿下来。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础使用

```cpp
#include <iostream>
#include <stack>
using namespace std;
int main() {
    stack<int> st;
    st.push(10); st.push(20); st.push(30);
    cout << st.top() << '\\n';
    st.pop();
    cout << st.top() << '\\n';
}
```

1. 判断是否为空

```cpp
if (!st.empty()) cout << st.top() << '\\n';
```

1. 括号匹配

```cpp
stack<char> st;
for (char c : s) {
    if (c == '(') st.push(c);
    else if (c == ')') {
        if (st.empty()) return false;
        st.pop();
    }
}
```

1. 逆序输出

```cpp
while (!st.empty()) {
    cout << st.top() << ' ';
    st.pop();
}
```

## ⚠️ Pitfalls（高频错误）

- 空栈上直接 `top()` / `pop()`
- `pop()` 不返回元素
- 和队列顺序搞混
- 最后忘记检查栈是否为空

## 🚀 Performance / Tips（性能优化）

- `push/pop/top` 常为 $O(1)$
- 单调栈、表达式处理都是重要扩展
- 高频经验：
    - 看到“最近元素优先”先想栈

## 🧪 Common Scenarios（常见使用场景）

- 括号匹配
- 撤销操作
- 表达式处理
- 逆序输出
- 单调栈基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <stack>
using namespace std;
int main() {
    stack<int> st;
    st.push(1); st.push(2); st.push(3);
    while (!st.empty()) {
        cout << st.top() << ' ';
        st.pop();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **栈就是：一个只允许在一端进出、遵循后进先出规则的线性结构。**