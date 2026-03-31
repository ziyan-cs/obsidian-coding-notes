#cpp #memory-management #stack #heap #raii

## ⚡ TL;DR（快速决策）

- 内存管理本质是：**明确对象在哪里创建、活多久、由谁释放**
- 一看到这些问题，要优先想到内存管理：
    - 对象生命周期
    - `new/delete`
    - 栈区 / 堆区
    - 资源泄漏、悬空引用
- 现代 C++ 的核心理念不是“手写 delete”，而是：**RAII + 智能指针**

## 🧩 Core Idea（核心本质）

- 常见存储区域理解：
    - 栈：局部对象，自动释放
    - 堆：动态分配，需要管理释放
    - 全局 / 静态区：程序全过程存在
- 一句话理解：
    - **内存管理就是管理对象的生命周期和所有权。**
- 根本问题：
    - 什么时候创建
    - 什么时候销毁
    - 谁负责销毁

## 🔧 Usage Patterns（可复用代码模板）

1. 栈对象

```cpp
int x = 10;
string s = "hello";
```

1. 堆对象

```cpp
int* p = new int(42);
cout << *p << '\\n';
delete p;
```

1. 动态数组

```cpp
int* a = new int[5];
delete[] a;
```

1. RAII 思想

```cpp
vector<int> v = {1, 2, 3};
// 离开作用域自动析构
```

## ⚠️ Pitfalls（高频错误）

- `new` 后忘记 `delete`
- `delete` 和 `delete[]` 混用
- 释放后继续使用指针
- 返回局部对象地址
- 误把“能运行”当“生命周期正确”

## 🚀 Performance / Tips（性能优化）

- 能用自动对象就优先不用裸 `new`
- 资源管理优先 RAII
- 标准容器通常比手写动态数组更稳
- 智能指针优先于裸资源管理

## 🧪 Common Scenarios（常见使用场景）

- 动态创建对象
- 资源生命周期管理
- 容器与对象所有权
- 对象析构时自动释放资源

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int* p = new int(100);
    cout << *p << '\\n';
    delete p;
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **内存管理就是：管理对象的创建位置、生命周期以及资源释放责任。**