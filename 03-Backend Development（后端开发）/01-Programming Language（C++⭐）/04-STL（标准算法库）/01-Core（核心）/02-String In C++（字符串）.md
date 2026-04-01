#stl #string #cpp #text #container

## ⚡ TL;DR（快速决策）

- `string` 本质是：**专门用于处理字符序列的动态字符串容器**
- 一看到这些需求，要优先想到 `string`：
    - 输入一整行文本
    - 拼接、截取、查找子串
    - 判断回文、模拟字符串变化
    - 做模式匹配、统计字符
- `string` 比 C 风格字符数组更安全、更好写
- 常见高频操作：
    - `size()` / `length()`
    - `substr()`
    - `find()`
    - `+=`
    - 排序、反转、遍历
- 如果题目核心对象是“文本 / 字符串”，默认先用 `string`

## 🧩 Core Idea（核心本质）

- `string` 可以理解成“字符版的 `vector`”
- 底层也是顺序存储，支持下标访问
- 和 `char[]` 的核心区别：
    - `char[]` 更底层
    - `string` 自动管理长度，更适合写题
- 一句话理解：
    - **`string` = 更现代、更易用的字符串。**

## 🔧 Usage Patterns（可复用代码模板）

1. 定义与输入

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string s = "hello";
    string t;
    cin >> t;
    cout << s << ' ' << t << '\\n';
    return 0;
}
```

1. 读一整行

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string line;
    getline(cin, line);
    cout << line << '\\n';
    return 0;
}
```

1. 拼接、截取、遍历

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string s = "abc";
    s += "def";
    cout << s.substr(1, 3) << '\\n';

    for (char c : s) cout << c << ' ';
    return 0;
}
```

1. 查找子串

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string s = "hello world";
    size_t pos = s.find("world");
    if (pos != string::npos) cout << pos << '\\n';
    return 0;
}
```

1. 回文判断模板

```cpp
#include <iostream>
#include <string>
using namespace std;

bool isPalindrome(const string& s) {
    int l = 0, r = (int)s.size() - 1;
    while (l < r) {
        if (s[l] != s[r]) return false;
        ++l;
        --r;
    }
    return true;
}
```

## ⚠️ Pitfalls（高频错误）

- `cin >> s` 读不到空格
- `find()` 找不到时返回 `string::npos`
- 下标访问要注意越界
- `substr(pos, len)` 第二个参数是长度，不是结束位置
- 字符和字符串别混：`'a'` 是字符，`"a"` 是字符串

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - 下标访问：$O(1)$
    - 末尾拼接：通常较快
    - 查找子串：和实现有关，别默认一定很快
- 实战建议：
    - 文本处理默认先用 `string`
    - 统计字符时常配合数组 / 哈希
    - 回文、双指针题常直接在 `string` 上做

## 🧪 Common Scenarios（常见使用场景）

- 模拟字符串变化
- 回文判断
- 子串查找
- 字符统计
- 表达式 / 文本处理

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string s = "abc";
    s += "xyz";
    cout << s << '\\n';
    cout << s.substr(1, 3) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`string` 就是：一个专门处理字符序列、支持动态长度和常用字符串操作的容器。**