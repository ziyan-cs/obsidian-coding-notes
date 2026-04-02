#cpp #stl #function #callable #wrapper

## ⚡ TL;DR（快速决策）

- `std::function` 的核心是：**统一封装不同类型的可调用对象**
- 它能装普通函数、Lambda、仿函数、成员函数绑定结果等
- 适合“接口层面需要统一可调用签名”的场景

## 🧩 Core Idea（核心本质）

- 模板能静态接收可调用对象，而 `std::function` 提供的是运行时统一封装
- 一句话理解：
    - **`std::function` 是可调用对象的类型擦除容器。**

## 🔧 典型写法

```cpp
function<int(int,int)> op = [](int a, int b) {
    return a + b;
};
cout << op(2, 3) << '\\n';
```

## 🧪 适用场景

- 回调函数封装
- 事件处理器列表
- 统一保存多种可调用对象

## ⚠️ Pitfalls（高频错误）

- 在极高性能场景滥用 `std::function`
- 把模板和 `std::function` 的职责混淆

## 🚀 Performance / Tips（理解深化）

- 若追求极致性能，模板参数往往更轻
- 若追求接口统一和灵活性，`std::function` 更方便

## 📌 One-liner Summary（一句话总结）

- **`std::function` 的核心，是把不同可调用对象统一封装成同一种函数签名接口。**