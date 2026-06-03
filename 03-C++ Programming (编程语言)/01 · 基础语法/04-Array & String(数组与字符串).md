> **核心考点**：C 风格数组与指针的关系、std::string 的操作与性能、C 字符串函数

## 数组

```cpp
int arr[5] = {1, 2, 3, 4, 5};
int arr2[]  = {1, 2, 3};       // 大小自动推导 = 3
int mat[3][4] = {};             // 二维数组，零初始化

// 数组退化为指针（传参时丢失大小信息！）
void foo(int* arr, int n);      // 必须额外传大小

// 推荐：用 std::array（固定大小，不退化，有 size()）
#include <array>
std::array<int, 5> a = {1,2,3,4,5};
a.size();    // 5
a.at(2);     // 带越界检查
a.data();    // 获取裸指针

// std::vector（动态大小）
std::vector<int> v = {1,2,3};
v.push_back(4);
v.size();  v.capacity();
v.reserve(100);   // 预分配，避免多次扩容
v.shrink_to_fit();
```

## C 字符串 vs std::string

```cpp
// C 风格字符串（以 '\0' 结尾的 char 数组，避免使用）
char s[] = "hello";
strlen(s);  strcpy(dst, src);  strcat(dst, src);  strcmp(a, b);

// std::string（推荐）
#include <string>
std::string s = "hello";
s.size();   s.length();
s.empty();
s += " world";            // 拼接
s.substr(0, 5);           // 子串
s.find("world");          // 查找，返回下标或 std::string::npos
s.replace(6, 5, "C++");   // 替换
s.c_str();                // 转 const char*（生命周期与 s 绑定）

// 数值转换
std::to_string(42);       // int → string
std::stoi("42");          // string → int
std::stod("3.14");        // string → double

// string_view（C++17，零拷贝只读视图）
std::string_view sv = s;
```