#cpp #pointer #reference #memory #core-concepts

## ⚡ TL;DR（快速决策）

- 指针本质是：**存地址的变量**
- 引用本质是：**已有对象的别名**
- 一看到这些需求，要优先想到它们：
    - 想修改外部变量
    - 想避免大对象拷贝
    - 想表达“可为空”或“必须绑定对象”
- 选择建议：
    - 优先用引用表达“必须有对象”
    - 用指针表达“可能为空 / 需要重新指向别处”
- 很多 C++ 难点，根子都在“值、地址、生命周期”没分清

## 🧩 Core Idea（核心本质）

- 指针保存的是地址：`int* p`
- 引用不是新对象，而是原对象另一个名字：`int& r = x`
- 一句话理解：
    - **指针像写着地址的纸条，引用像给对象起了另一个名字。**
- 关键区别：
    - 指针可为空、可改指向
    - 引用通常必须初始化，且绑定后不再改绑

## 🔧 Usage Patterns（可复用代码模板）

1. 指针基础

```cpp
int x = 10;
int* p = &x;
cout << *p << '\\n';
```

1. 引用基础

```cpp
int x = 10;
int& r = x;
r = 20;
cout << x << '\\n';
```

1. 函数传引用

```cpp
void addOne(int& x) {
    ++x;
}
```

1. 指针判空

```cpp
if (p != nullptr) {
    cout << *p << '\\n';
}
```

## ⚠️ Pitfalls（高频错误）

- 忘记区分 `p` 和 `*p`
- 野指针、悬空指针
- 空指针解引用
- 以为引用能像指针一样改绑
- `const int*` 和 `int* const` 混淆

## 🚀 Performance / Tips（性能优化）

- 大对象传 `const &` 很常见
- 裸指针更偏底层，现代 C++ 常与智能指针配合讨论
- 学习顺序建议：
    - 先分清值和地址
    - 再分清指针和引用
    - 最后理解 `const`、生命周期、所有权

## 🧪 Common Scenarios（常见使用场景）

- 交换变量
- 链表 / 树结构
- 函数参数高效传递
- 动态内存访问
- 面向对象中的 `this` 指针

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 5;
    int* p = &x;
    int& r = x;
    *p = 7;
    r = 9;
    cout << x << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **指针与引用的核心就是：前者通过地址间接访问对象，后者直接作为对象的别名使用。**