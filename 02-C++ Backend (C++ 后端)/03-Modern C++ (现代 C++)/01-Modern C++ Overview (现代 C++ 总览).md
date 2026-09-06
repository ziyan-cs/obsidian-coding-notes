---
tags:
  - cpp/modern
status: 🌱
---

# 01-Modern C++ Overview (现代 C++ 总览)

> [!abstract] 核心考点：> C++11/14/17/20 关键特性一览、现代 C++ 的核心设计理念

|特性|标准|核心价值|
|---|---|---|
|auto / decltype|C++11|减少冗余类型声明|
|Lambda|C++11/14|就地定义可调用对象|
|右值引用 / 移动语义|C++11|消除不必要的深拷贝|
|unique_ptr / shared_ptr|C++11|自动内存管理，消除裸 new/delete|
|完美转发|C++11|泛型代码中保持值类别|
|constexpr|C++11/14/17|将计算移到编译期|
|if constexpr|C++17|编译期条件分支，替代 SFINAE|
|std::optional|C++17|表达"可能无值"，替代哨兵值|
|std::variant|C++17|类型安全联合体，替代 union|
|string_view|C++17|零拷贝字符串视图|
|Structured Bindings|C++17|解包 pair/tuple/struct|
|Concepts|C++20|约束模板参数类型，替代 SFINAE|
|Ranges|C++20|可组合的惰性序列操作|
|Coroutines|C++20|协程支持，异步编程|

---

类型推导详见 → [Type Deduction (类型推导)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Type%20Deduction%20(类型推导).md)
