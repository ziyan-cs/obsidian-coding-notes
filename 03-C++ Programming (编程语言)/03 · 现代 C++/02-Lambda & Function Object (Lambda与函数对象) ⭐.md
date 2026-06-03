> **核心考点**：捕获方式、泛型 lambda、std::function 的开销

## Lambda 基本语法

```cpp
[捕获列表](参数列表) mutable -> 返回类型 { 函数体 }

auto add = [](int a, int b) -> int { return a + b; };
auto greet = [] { std::cout << "hello\n"; };  // 无参数可省略括号（C++23前需要括号）
```

## 捕获方式

```cpp
int x = 10, y = 20;

// 值捕获（拷贝，lambda 内的 x 与外部无关）
auto f1 = [x]() { return x; };

// 引用捕获（lambda 持有引用，需注意悬空）
auto f2 = [&x]() { x++; };

// 混合捕获
auto f3 = [x, &y]() { return x + y; };

// 默认值捕获（捕获所有用到的局部变量的副本）
auto f4 = [=]() { return x + y; };

// 默认引用捕获
auto f5 = [&]() { x++; y++; };

// 混合默认：引用捕获 x，值捕获其余
auto f6 = [&, y]() { x++; return y; };

// 捕获 this（在成员函数中使用）
struct Foo {
    int val = 42;
    auto getF() {
        return [this]() { return val; };     // 捕获 this 指针
        // C++17 推荐：[*this]() { return val; };  // 捕获 this 的副本，更安全
    }
};
```

## mutable：修改值捕获的副本

```cpp
int x = 0;
auto f = [x]() mutable { x++; return x; };  // 不加 mutable，值捕获不可修改
f();  // 返回 1，但外部 x 仍为 0
```


## 泛型 Lambda（C++14）

```cpp
// auto 参数，等价于模板函数
auto print = [](auto x) { std::cout << x << '\n'; };
print(42);
print("hello");
print(3.14);

// 多参数
auto max_val = [](auto a, auto b) { return a > b ? a : b; };
```

## Lambda 作为排序谓词

```cpp
std::vector<std::pair<int,int>> v = {{3,1},{1,2},{2,3}};

// 按第二个元素降序
std::sort(v.begin(), v.end(), [](const auto& a, const auto& b){
    return a.second > b.second;
});

// 捕获外部变量作为比较依据
int pivot = 5;
auto less_than = [pivot](int x) { return x < pivot; };
```

---

## std::function

类型擦除的通用函数包装器，可存储任何可调用对象（函数、lambda、函数对象）：

```cpp
#include <functional>

std::function<int(int, int)> f;

f = [](int a, int b) { return a + b; };   // 存 lambda
f = std::plus<int>{};                      // 存函数对象
f = add;                                   // 存函数指针

// 回调/策略模式
void process(std::vector<int>& v, std::function<bool(int)> pred) {
    v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
}
process(nums, [](int x){ return x % 2 == 0; });  // 删除偶数
```

### std::function 的代价

`std::function` 使用**类型擦除**（虚函数或函数指针间接调用），有以下开销：

- 间接调用（无法内联）
- 可能堆分配（捕获较大时）
- 性能敏感场景用模板参数代替

```cpp
// 性能敏感时：模板参数（编译期确定类型，可内联）
template<typename F>
void process(std::vector<int>& v, F pred) {
    v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
}
```