#cpp #modern-cpp #string-view #string #non-owning

## ⚡ TL;DR（快速决策）

- `std::string_view` 本质是：**对一段字符串的只读、非拥有视图**
- 一看到这些场景，要优先想到 `string_view`：
    - 只想读字符串，不想拷贝
    - 想统一接收 `string`、字符串字面量、子串视图
    - 高频参数传递，想降低拷贝开销
- 它不拥有数据，所以**生命周期问题特别关键**

## 🧩 Core Idea（核心本质）

- `string_view` 只是“看着一段字符”，不负责管理内存
- 可以理解成：
    - 指针 + 长度
- 一句话理解：
    - **`string_view` = 不拷贝字符串，只借来看。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础使用

```cpp
#include <iostream>
#include <string_view>
using namespace std;

int main() {
    string_view sv = "hello";
    cout << sv << '\\n';
}
```

1. 作为参数

```cpp
void print(string_view sv) {
    cout << sv << '\\n';
}
```

1. 子串视图

```cpp
string_view sv = "abcdef";
cout << sv.substr(1, 3) << '\\n';
```

## ⚠️ Pitfalls（高频错误）

- `string_view` 不拥有数据，引用的原字符串销毁后它就悬空了
- 不适合保存长期所有权
- 它主要是只读视图，不是可随意修改的字符串容器
- 很多人把它当轻量版 `string`，这是不对的

## 🚀 Performance / Tips（性能优化）

- 参数只读传递时非常有价值
- 可以避免许多不必要的字符串拷贝
- 但生命周期安全永远比省那点拷贝更重要

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <string_view>
using namespace std;

int main() {
    string_view sv = "modern cpp";
    cout << sv.substr(0, 6) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`std::string_view` 就是：一个不拥有字符串数据、只提供只读视图的轻量类型。**