#cpp #modern-cpp #smart-pointer #memory #raii

## ⚡ TL;DR（快速决策）

- 智能指针本质是：**用对象自动管理动态内存和资源生命周期**
- 三个高频成员：
    - `unique_ptr`：独占所有权
    - `shared_ptr`：共享所有权
    - `weak_ptr`：弱引用，不参与计数
- 一看到这些场景，要优先想到智能指针：
    - 动态分配对象
    - 想避免手写 `delete`
    - 需要 RAII 资源管理
- 现代 C++ 里，能不用裸 `new/delete` 就尽量不用

## 🧩 Core Idea（核心本质）

- 智能指针不是“会魔法的指针”，而是封装了资源释放规则的类
- 它们的核心价值是：
    - 自动释放
    - 异常安全
    - 所有权表达清晰
- 一句话理解：
    - **智能指针 = 自动回收资源的所有权工具。**

## 🔧 Usage Patterns（可复用代码模板）

1. `unique_ptr`

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main() {
    unique_ptr<int> p = make_unique<int>(42);
    cout << *p << '\\n';
}
```

1. `shared_ptr`

```cpp
auto p = make_shared<int>(10);
auto q = p;
cout << p.use_count() << '\\n';
```

1. `weak_ptr`

```cpp
shared_ptr<int> p = make_shared<int>(5);
weak_ptr<int> w = p;
if (auto sp = w.lock()) {
    cout << *sp << '\\n';
}
```

## ⚠️ Pitfalls（高频错误）

- `unique_ptr` 不能随便拷贝，只能移动
- `shared_ptr` 滥用可能导致共享关系混乱
- 循环引用会让 `shared_ptr` 无法释放，需要 `weak_ptr` 打破环
- 同一裸指针别交给多个独立 `shared_ptr` 管理

## 🚀 Performance / Tips（性能优化）

- 默认先想 `unique_ptr`
- 确实需要共享所有权时再用 `shared_ptr`
- 优先 `make_unique` / `make_shared`
- 智能指针提升的是资源安全性，也会带来一定抽象开销

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main() {
    auto p = make_unique<int>(100);
    cout << *p << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **智能指针就是：用 RAII 方式自动管理动态资源生命周期的现代 C++ 工具。**