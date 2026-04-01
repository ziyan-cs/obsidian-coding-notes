#stl #stack #cpp #lifo #container-adapter

## ⚡ TL;DR（快速决策）

- `stack` 本质是：**后进先出（LIFO）**
- 一看到这些特征，要优先想到栈：
    - 最近加入的元素要先处理
    - 括号匹配
    - 表达式求值
    - 单调栈前的基础容器理解
    - 需要“撤回上一步”式处理
- 常用操作就几个：
    - `push()`
    - `pop()`
    - `top()`
    - `empty()`
- `stack` 不支持随机访问，也不支持遍历接口
- 如果题目是“最后进去的先出来”，默认先想栈

## 🧩 Core Idea（核心本质）

- 栈是一种**操作受限的线性结构**
- 你只能在“栈顶”进行插入和删除
- 所以栈最像：
    - 盘子一层层叠起来
    - 只能先拿最上面那个
- 一句话理解：
    - **谁最后进去，谁最先出来。**
- C++ STL 里的 `stack` 是容器适配器，不是普通顺序容器

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础使用

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    st.push(10);
    st.push(20);
    st.push(30);

    cout << st.top() << '\\n';
    st.pop();
    cout << st.top() << '\\n';
    return 0;
}
```

1. 判断是否为空

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    cout << st.empty() << '\\n';
    st.push(1);
    cout << st.empty() << '\\n';
    return 0;
}
```

1. 括号匹配模板

```cpp
#include <iostream>
#include <stack>
#include <string>
using namespace std;

bool isValid(string s) {
    stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else {
            if (st.empty()) return false;
            char t = st.top();
            st.pop();
            if (c == ')' && t != '(') return false;
            if (c == ']' && t != '[') return false;
            if (c == '}' && t != '{') return false;
        }
    }
    return st.empty();
}
```

1. 逆序输出

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    for (int x : {1, 2, 3, 4}) st.push(x);

    while (!st.empty()) {
        cout << st.top() << ' ';
        st.pop();
    }
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- 空栈上不能直接 `top()` 或 `pop()`
- `pop()` 不返回被删元素
- `stack` 不能像 `vector` 那样遍历
- 别把队列和栈的顺序搞混
- 括号题里常见错误是：忘记最后检查栈是否为空

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - `push()`：$O(1)$
    - `pop()`：$O(1)$
    - `top()`：$O(1)$
- 实战建议：
    - 题目只要是“最近元素优先”，先想栈
    - 需要遍历全部元素时，通常要边弹边处理
    - 后续学单调栈时，普通栈就是基础

## 🧪 Common Scenarios（常见使用场景）

- 括号匹配
- 逆序处理
- 表达式求值
- 撤销 / 回退逻辑
- 单调栈基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);

    while (!st.empty()) {
        cout << st.top() << ' ';
        st.pop();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`stack` 就是：一个只允许在栈顶进出元素、遵循后进先出规则的容器。**