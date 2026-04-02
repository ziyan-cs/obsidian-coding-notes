#cpp #basics #input-output #cin #cout

## ⚡ TL;DR（快速决策）

- C++ 输入输出的核心是：**用 `cin` 读入，用 `cout` 输出**
- 一看到这些需求，要优先想到这些工具：
    - 读整数 / 浮点数 / 字符串：`cin`
    - 输出结果：`cout`
    - 读整行：`getline`
    - 调试输出：`cerr`
- 算法题里最常用：
    - `cin >> x`
    - `cout << x << '\\n'`
- 数据量大时，常配合：
    - `ios::sync_with_stdio(false);`
    - `cin.tie(nullptr);`

## 🧩 Core Idea（核心本质）

- 输入输出本质是程序和外部数据交换信息的方式
- C++ 标准输入输出流：
    - `cin`：标准输入
    - `cout`：标准输出
    - `cerr`：标准错误输出
- 一句话理解：
    - **`cin` 负责读，`cout` 负责写。**

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础输入输出

```cpp
#include <iostream>
using namespace std;

int main() {
    int x;
    cin >> x;
    cout << x << '\n';
    return 0;
}
```

2. 多变量输入

```cpp
int a, b;
cin >> a >> b;
cout << a + b << '\n';
```

3. 字符串输入

```cpp
string s;
cin >> s;
cout << s << '\n';
```

4. 读一整行

```cpp
string line;
getline(cin, line);
cout << line << '\n';
```

5. 快速 I/O

```cpp
ios::sync_with_stdio(false);
cin.tie(nullptr);
```

## ⚠️ Pitfalls（高频错误）

- `cin >> s` 读不到空格
- `getline` 和 `cin` 混用时容易读到空行
- 输出格式别漏空格和换行
- 大数据时忘记开快速 I/O 可能超时

## 🚀 Performance / Tips（性能优化）

- 常规题直接 `cin/cout` 就够用
- 数据量大时加快速 I/O
- 调试时可用 `cerr`
- 输出换行常用 `\\n`，比 `endl` 更轻量

## 🧪 Common Scenarios（常见使用场景）

- 读一组数据
- 多组测试输入
- 字符串与整行处理
- 调试信息输出

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int a, b;
    cin >> a >> b;
    cout << a + b << '\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **C++ 输入输出就是：用 `cin` 读数据，用 `cout` 输出结果。**