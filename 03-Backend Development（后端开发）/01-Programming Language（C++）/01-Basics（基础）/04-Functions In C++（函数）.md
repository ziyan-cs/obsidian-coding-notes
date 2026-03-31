#cpp #functions #basics #parameter #return-value

## ⚡ TL;DR（快速决策）

- 函数本质是：**把一段可复用逻辑封装起来**
- 一看到这些需求，要优先想到函数：
    - 某段逻辑会重复出现
    - 想把主程序拆得更清楚
    - 想传入参数并返回结果
- 函数三要素：
    - 返回值类型
    - 函数名
    - 参数列表
- 代码一复杂，就该主动拆函数

## 🧩 Core Idea（核心本质）

- 函数就是“输入一些东西，处理后返回结果”
- 有些函数只做事，不返回值：`void`
- 一句话理解：
    - **函数是程序里的可复用模块。**
- 好函数能让主程序更短、更清晰、更易调试

## 🔧 Usage Patterns（可复用代码模板）

1. 基础函数

```cpp
#include <iostream>
using namespace std;

int add(int a, int b) {
    return a + b;
}

int main() {
    cout << add(2, 3) << '\\n';
}
```

1. `void` 函数

```cpp
void hello() {
    cout << "hello\\n";
}
```

1. 引用参数

```cpp
void addOne(int& x) {
    ++x;
}
```

1. 默认参数

```cpp
int power(int a, int b = 2) {
    int ans = 1;
    while (b--) ans *= a;
    return ans;
}
```

## ⚠️ Pitfalls（高频错误）

- 忘记写返回值
- 返回值类型和实际返回内容不匹配
- 形参和实参概念混乱
- 不想改原变量时别乱用引用
- 函数声明和定义签名要一致

## 🚀 Performance / Tips（性能优化）

- 逻辑重复两次以上时，优先考虑抽函数
- 大对象传参时常用 `const &`
- 函数命名要尽量表达含义
- 调试时，函数拆分越清晰越好定位问题

## 🧪 Common Scenarios（常见使用场景）

- 数值计算
- 判定函数 `check()`
- 搜索 / DP / 图论中的子逻辑拆分
- 输入输出辅助函数

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int square(int x) {
    return x * x;
}

int main() {
    cout << square(5) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **函数就是：把一段逻辑封装起来，通过参数输入，并在需要时返回结果。**