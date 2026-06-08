---
tags:
  - cpp/modern
status: 🌱
---

> [!important] **核心考点**：string_view 非拥有视图与生命周期注意事项、结构化绑定的使用场景

## std::string_view（C++17）

对字符串的**非拥有只读视图**，避免不必要的字符串拷贝：

```cpp
#include <string_view>

// 可以从字面量、string、char* 构造，零拷贝
void print(std::string_view sv) {
    std::cout << sv << " len=" << sv.size() << '\n';
}

print("hello");                        // char 字面量，无拷贝
print(std::string("world"));           // std::string，无拷贝（只存指针和长度）

std::string_view sv = "hello world";
sv.substr(0, 5);    // 返回新的 string_view，不分配内存
sv.starts_with("hello");   // C++20
sv.find("world");
```

### string_view 的生命周期陷阱

```cpp
// 危险！string_view 持有临时 string 的引用，函数返回后悬空
std::string_view dangerous() {
    std::string s = "hello";
    return s;   // s 析构后 string_view 悬空！
}

// 安全：被观察的对象必须比 string_view 活得更长
std::string s = "hello world";
std::string_view sv = s;   // OK，sv 的生命周期在 s 内
```

**函数参数用 `string_view` 代替 `const string&`：**

```cpp
// 旧写法：传字面量时会构造临时 string
void old_func(const std::string& s);

// 新写法：零开销，同时接受 string、字面量、char* 等
void new_func(std::string_view sv);
```

---

## Structured Bindings（结构化绑定，C++17）

解包 pair、tuple、struct、数组到多个命名变量：

```cpp
// pair
std::pair<int, std::string> p{1, "Alice"};
auto [id, name] = p;

// tuple
auto [x, y, z] = std::make_tuple(1, 2.0, "three");

// map 遍历（最常用）
std::map<std::string, int> scores{{"Alice",95},{"Bob",87}};
for (auto& [name, score] : scores) {
    std::cout << name << ": " << score << '\n';
}

// struct（聚合类型）
struct Point { double x, y; };
Point pt{3.0, 4.0};
auto [px, py] = pt;

// 数组
int arr[] = {1, 2, 3};
auto [a, b, c] = arr;

// 绑定为引用（可修改）
auto& [rx, ry] = pt;
rx = 10.0;   // 修改 pt.x
```

### 与 if/switch 结合（C++17 init-statement）

```cpp
// 在 if 的初始化语句中使用结构化绑定
if (auto [it, ok] = myMap.insert({key, val}); ok) {
    std::cout << "inserted\n";
} else {
    std::cout << "key already exists\n";
}

// lock_guard + 结构化绑定（C++17 scoped init）
if (auto [lock, data] = acquireData(); data.valid()) {
    process(data);
}
```

---

string_view 与结构化绑定是现代 C++ 的轻量视图特性，详见 → [Modern C++ Overview (现代 C++ 特性总览)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)
