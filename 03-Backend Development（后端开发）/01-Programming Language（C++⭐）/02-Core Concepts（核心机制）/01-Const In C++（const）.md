#cpp #const #immutability #pointer #reference

## ⚡ TL;DR（快速决策）

- `const` 本质是：**表达“这个东西不应该被修改”**
- 一看到这些场景，要优先想到 `const`：
    - 常量值
    - 只读参数
    - 不应修改对象状态的成员函数
    - 指针与引用的只读约束
- `const` 既是语法规则，也是代码意图表达
- 学会 `const`，能明显提升代码安全性和可读性

## 🧩 Core Idea（核心本质）

- `const` 可以修饰：
    - 变量
    - 指针
    - 引用
    - 成员函数
- 一句话理解：
    - **`const` 就是在类型系统里写下“只读承诺”。**
- 经典区别：
    - `const int* p`：不能通过 `p` 改值
    - `int* const p`：`p` 自己不能改指向
    - `const int* const p`：都不能改

## 🔧 Usage Patterns（可复用代码模板）

1. 常量变量

```cpp
const int MOD = 1000000007;
```

2. 常量引用

```cpp
void print(const string& s) {
    cout << s << '\n';
}
```

3. 指针中的 `const`

```cpp
int x = 10;
const int* p1 = &x;
int* const p2 = &x;
```

4. `const` 成员函数

```cpp
class A {
public:
    int get() const {
        return x;
    }
private:
    int x = 0;
};
```

## ⚠️ Pitfalls（高频错误）

- `const int*` 和 `int* const` 混淆
- 忘记给只读参数加 `const &`
- 成员函数该写 `const` 却没写
- 误以为 `const` 就一定是编译期常量

## 🚀 Performance / Tips（性能优化）

- `const &` 可避免大对象复制
- `const` 更重要的是语义与安全性，而非直接性能
- 习惯性给“不修改”的参数和成员函数加 `const`，代码质量会明显提升

## 🧪 Common Scenarios（常见使用场景）

- 常量定义
- 函数只读参数
- 类的只读接口
- 指针 / 引用的只读约束

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

void show(const int& x) {
    cout << x << '\n';
}

int main() {
    const int a = 5;
    show(a);
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`const` 就是：在类型层面明确表达“这里不能改”。**