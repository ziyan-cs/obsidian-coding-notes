#cpp #string #basics #text #character

## ⚡ TL;DR（快速决策）

- 字符串本质是：**由字符组成的一段文本序列**
- 在 C++ 里，算法题通常默认优先用 `string`
- 一看到这些需求，要优先想到字符串：
    - 输入单词或一整行文本
    - 遍历字符
    - 拼接、截取、比较
    - 回文、匹配、统计字符
- 核心高频操作：
    - `size()`
    - `substr()`
    - `find()`
    - `+=`

## 🧩 Core Idea（核心本质）

- `string` 是 C++ 标准库里的字符串类型
- 可以把它理解成“字符版动态数组”
- 支持下标访问、遍历、拼接、查找等常见操作
- 一句话理解：
    - **`string` 是处理文本最常用的基础类型。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础输入输出

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string s;
    cin >> s;
    cout << s << '\\n';
}
```

1. 读整行

```cpp
string line;
getline(cin, line);
```

1. 遍历字符

```cpp
for (char c : s) {
    cout << c << ' ';
}
```

1. 拼接与截取

```cpp
string s = "abc";
s += "def";
cout << s.substr(1, 3) << '\\n';
```

1. 查找

```cpp
if (s.find("ab") != string::npos) {
    cout << "found\\n";
}
```

## ⚠️ Pitfalls（高频错误）

- `cin >> s` 读不到空格
- `'a'` 和 `"a"` 不一样
- `substr(pos, len)` 第二个参数是长度
- `find()` 找不到时返回 `string::npos`
- 下标访问仍要注意越界

## 🚀 Performance / Tips（性能优化）

- 字符串题很多是遍历 + 判断
- 字符统计常配合数组 / 哈希
- 回文、双指针、模拟题里 `string` 非常常见
- 先保证逻辑清楚，再考虑优化

## 🧪 Common Scenarios（常见使用场景）

- 文本输入
- 回文判断
- 子串查找
- 字符统计
- 字符串模拟

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string s = "hello";
    cout << s << '\\n';
    cout << s.size() << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **字符串基础就是：学会用 `string` 存文本、遍历字符、做常见文本操作。**