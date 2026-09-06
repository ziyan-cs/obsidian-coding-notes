---
tags:
  - cpp/core
status: 🌱
---

# Type Conversion & Casting — 类型转换

> [!important] **核心考点**：四种命名的 C++ 类型转换（static/dynamic/const/reinterpret）、隐式转换规则

## 四种命名转换（Named Casts）

### static_cast

编译期类型转换，用于相关类型之间的安全转换：

```cpp
double d = 3.14;
int i = static_cast<int>(d);         // 截断小数

// 父子类指针转换（下行转换不安全，没有运行时检查）
Base* b = new Derived();
Derived* d = static_cast<Derived*>(b);  // 需确保 b 确实指向 Derived

// 枚举 ↔ 整型
int n = static_cast<int>(Direction::NORTH);
```

### dynamic_cast

运行时类型检查（RTTI），用于多态类的安全下行转换：

```cpp
Base* b = new Derived();
Derived* d = dynamic_cast<Derived*>(b);   // 成功返回指针，失败返回 nullptr
if (d) { d->derivedMethod(); }

// 引用版本：失败抛 std::bad_cast
try {
    Derived& dr = dynamic_cast<Derived&>(*b);
} catch (const std::bad_cast&) { ... }

// 要求：基类必须有至少一个虚函数（才有 RTTI 信息）
```

### const_cast

添加或移除 `const`，是唯一能移除 const 的转换：

```cpp
const char* cs = "hello";
char* s = const_cast<char*>(cs);   // 危险！修改字符串字面量是未定义行为

// 合法场景：函数参数是 const，但确知底层对象非 const
void legacyFunc(char* p);
void wrapper(const char* p) {
    legacyFunc(const_cast<char*>(p));  // 若 p 指向非 const 对象则安全
}
```

### reinterpret_cast

重新解释内存，最危险，几乎不带任何转换：

```cpp
int x = 42;
int* p = &x;
char* cp = reinterpret_cast<char*>(p);  // 按字节访问 int 的内存

// 序列化/反序列化、与硬件寄存器交互时使用
uint64_t addr = reinterpret_cast<uint64_t>(p);

// 函数指针转换（某些插件/JIT 场景）
```

## 转换选择原则

```
需要运行时安全检查（多态向下转型）  → dynamic_cast
数值类型转换 / 亲缘类型            → static_cast
只去掉 const                       → const_cast
纯内存重新解释                     → reinterpret_cast
永远不用 C 风格 (int)x             → 代码难维护、无类型安全
```

---

指针与引用的转换操作详见 → [Pointers & References In Depth (指针与引用深入)](/03-C++%20Programming%20(编程语言)/02-Core%20Mechanisms%20(核心机制)/03-Pointers%20&%20References%20In%20Depth%20(指针与引用深入)%20⭐.md)
