---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

RAII（Resource Acquisition Is Initialization）的核心是：**资源的拥有期由对象生命周期表示**。构造函数成功后对象应处于可用状态；析构函数负责释放资源，因此无论正常返回、异常抛出还是提前 `return`，作用域退出时都能回收文件、锁、socket、内存或事务等资源。实际代码优先使用标准库封装，如 `std::vector`、智能指针和 `std::lock_guard`，而不是手写 `new/delete`。

# 先建立的模型

```text
acquire resource
    -> object owns resource
    -> scope exits
    -> destructor releases resource

failure while constructing
    -> no half-initialized object escapes
```

这里的“资源”不等于堆内存。凡是必须成对获取与释放的东西都适用：文件描述符、互斥锁、数据库事务、网络连接、临时状态和系统句柄。

# 设计判断

| 场景 | 首选 | 原因 |
| --- | --- | --- |
| 独占资源 | `std::unique_ptr` 或值对象 | 所有权唯一，移动成本清楚 |
| 共享对象生命周期 | `std::shared_ptr`，谨慎使用 | 只有确实存在共享所有权时才付出引用计数成本 |
| 临界区 | `std::lock_guard` / `std::unique_lock` | 锁的获取与释放不会因异常或分支漏掉 |
| 自定义句柄 | 小型 move-only RAII wrapper | 把关闭协议集中在一个类型中 |



# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- **RAII 不是“只要有析构函数就行”**：对象必须清楚表达谁拥有资源，拷贝、移动和析构的语义必须一致。
- **`shared_ptr` 不是默认选择**：它解决共享所有权，不解决所有对象的生命周期问题；循环引用仍会泄漏。
- **析构函数不应抛异常**：栈展开期间再次抛出通常会导致程序终止；析构中的失败应转换为可记录、可忽略或显式关闭前处理。
- **内存池不是 RAII 替代品**：它优化分配策略，不能替代所有权、析构和异常安全设计。

# 自测

1. 为什么“构造函数获取、析构函数释放”比手写 `open/close` 更能保证异常安全？
2. 一个 socket wrapper 需要禁止拷贝、允许移动吗？为什么？
3. 什么时候 `shared_ptr` 反而会让设计更难排查？

# RAII and Resource Management (RAII 与资源管理)

> [!note] 本节重点：核心考点：RAII 是 C++ 最核心的资源管理范式、资源获取即初始化、析构函数释放、异常安全的基础

# 什么是 RAII

RAII（Resource Acquisition Is Initialization）：**在构造函数中获取资源，在析构函数中释放资源**。

```cpp
// 传统 C 风格：手动管理
void bad() {
    int* p = (int*)malloc(sizeof(int) * 100);
    // ... 使用 p
    free(p);  // 容易忘记或提前 return 跳过
}

// RAII 风格：资源生命周期与对象绑定
class Buffer {
    int* data_;
public:
    Buffer(size_t n) : data_(new int[n]) {}
    ~Buffer() { delete[] data_; }  // 自动释放
    // ... 禁止拷贝或实现正确拷贝语义
};

void good() {
    Buffer buf(100);  // 获取资源
    // ... 使用（不需要手动释放）
}  // buf 析构 → 自动释放
```

**RAII 的三个核心要素**：
1. 资源在构造函数中获取
2. 资源在析构函数中释放
3. 资源不能离开对象独立存在（禁用拷贝或正确管理生命周期）

# RAII 管理的资源类型

```cpp
// RAII 不只管理内存——管理所有需要成对获取/释放的资源

// 1. 堆内存 → std::unique_ptr / std::shared_ptr / std::vector
auto ptr = std::make_unique<int[]>(100);

// 2. 文件句柄
class FileHandle {
    FILE* fp_;
public:
    FileHandle(const char* path) : fp_(fopen(path, "r")) {
        if (!fp_) throw std::runtime_error("open failed");
    }
    ~FileHandle() { if (fp_) fclose(fp_); }
    // 禁止拷贝...
};

// 3. 互斥锁 → std::lock_guard
std::lock_guard lock(mtx);  // 构造时加锁，析构时解锁

// 4. 数据库连接
class DBConnection {
    MYSQL* conn_;
public:
    DBConnection() { conn_ = mysql_init(nullptr); mysql_real_connect(...); }
    ~DBConnection() { mysql_close(conn_); }
};

// 5. POSIX 信号量/socket/其他系统资源
```

# 智能指针是 RAII 的典型应用

```cpp
// unique_ptr：独占所有权
std::unique_ptr<Foo> uptr(new Foo());    // 构造时获得所有权
// 析构时自动 delete
auto uptr2 = std::make_unique<Foo>();    // C++14 推荐（异常安全）

// shared_ptr：共享所有权（引用计数）
auto sptr = std::make_shared<Foo>();     // 引用计数 = 1
{
    auto sptr2 = sptr;                   // 引用计数 = 2
}                                        // 引用计数 = 1
// 引用计数 = 0 时自动 delete
```

# RAII + 异常安全的黄金法则

```cpp
// 使用 RAII 包装所有资源 → 自动获得基本异常安全保证
// 再配合 copy-and-swap → 强异常安全保证

class SafeVector {
    int* data_;
    size_t size_;
public:
    SafeVector(size_t n) : data_(new int[n]), size_(n) {}
    
    // Copy-and-swap：强异常安全
    SafeVector(const SafeVector& other)
        : data_(new int[other.size_]), size_(other.size_)
    {
        std::copy(other.data_, other.data_ + size_, data_);
    }
    
    void swap(SafeVector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }
    
    SafeVector& operator=(SafeVector other) {  // 传值（拷贝或移动）
        swap(other);                            // noexcept swap
        return *this;                           // 旧资源被 other 析构
    }
    // 如果拷贝构造抛出异常 → 原对象不变（强保证）
    
    ~SafeVector() { delete[] data_; }
};
```

# RAII 的常见错误

```cpp
// ❌ 错误 1：RAII 类没有正确处理拷贝
class BadResource {
    int* data_;
public:
    BadResource() : data_(new int[100]) {}
    ~BadResource() { delete[] data_; }
    // ❌ 没有禁用或实现拷贝 → 浅拷贝 → double free!
};

BadResource a;
BadResource b = a;  // 两个对象指向同一块内存
// 析构时 double free!

// ✅ 解决方案：unique_ptr（不可拷贝）或 shared_ptr（引用计数）

// ❌ 错误 2：在构造函数中获取多个资源
class TwoResources {
    int* a_;
    int* b_;
public:
    TwoResources() : a_(new int[100]), b_(new int[100]) {}
    // 如果 b_ 的分配抛出异常，a_ 不会被释放！
    ~TwoResources() { delete[] a_; delete[] b_; }
};

// ✅ 解决方案：用 RAII 包装单个资源
class SafeTwoResources {
    std::unique_ptr<int[]> a_;
    std::unique_ptr<int[]> b_;
public:
    SafeTwoResources()
        : a_(std::make_unique<int[]>(100))
        , b_(std::make_unique<int[]>(100)) {}
    // 即使 b_ 构造失败，a_ 已构造成功会被正确析构
};
```

# 工程习惯

```cpp
// ✅ 每次看到"配对操作"（open/close, lock/unlock, new/delete, malloc/free）
//    → 思考：如何用 RAII 封装它？

// ✅ 类的成员变量优先使用 RAII 类型
class Server {
    std::vector<Connection> clients_;   // ✅ 自动管理
    std::mutex mtx_;                    // ✅ 自动管理
    std::unique_ptr<Config> config_;    // ✅ 自动管理
    // ❌ 避免：int* buffer_; 需要手动管理
};

// ✅ 理解"所有资源"包括：内存、文件、锁、socket、数据库连接、GPU 句柄
```

> **面试必问**："RAII 是什么？" — 用一句话回答：构造函数获取资源，析构函数释放资源。再补充"它让 C++ 不需要 finally 块，因为析构函数在栈展开时自动被调用"。如果要展示深度，可以提到 "RAII 是 C++ 异常安全的基础"。

**自测题：**
1. 为什么 `std::lock_guard` 不需要显式 `unlock()`？异常发生时锁会被释放吗？
2. 一个类持有两个裸指针成员且都在构造函数中 `new`，第二个 `new` 抛异常时第一个指针会怎样？如何修复？
3. 为什么 C++ 不需要 `finally` 块？

---

RAII 的核心是资源管理与指针生命周期，详见 → [Pointers & References In Depth (指针与引用深入)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/03-Pointers%20&%20References%20In%20Depth%20(指针与引用深入)%20⭐.md)

---

# Custom Allocators and Placement New (自定义分配器)

> [!note] 本节重点：核心考点：自定义分配器用于高性能场景（内存池、特定分配策略）、placement new 构造对象、operator new 重载

# Placement New

```cpp
#include <new>  // placement new 所需头文件

// placement new：在已分配的内存上构造对象
void* buffer = operator new(sizeof(Foo));  // 只分配内存，不构造
Foo* foo = new (buffer) Foo(42);          // 在 buffer 上构造 Foo
// 等价于：Foo* foo = ::new (buffer) Foo(42);

// 手动析构（不释放内存）
foo->~Foo();
operator delete(buffer);  // 只释放内存，不调用析构
```

**典型的应用场景**：内存池、自定义容器、嵌入式系统。

```cpp
// placement new 的对称操作：
// new (ptr) T(args)  →  ptr->~T()
// operator new(size) →  operator delete(ptr)
// 不要混用 placement new 和普通 delete！
```

# 标准分配器接口（std::allocator）

```cpp
// std::allocator 的典型实现（简化版）
template<typename T>
struct Allocator {
    using value_type = T;
    
    T* allocate(size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    void deallocate(T* p, size_t) noexcept {
        ::operator delete(p);
    }
    
    // C++17 起，construct/destroy 已弃用
    // 直接用 placement new 和 ~T()
};

// 使用
std::vector<int, MyAllocator<int>> custom_vec;
// 但注意：标准容器默认使用 std::allocator，替换需谨慎
```

# 实现自定义分配器

```cpp
// 简单的池分配器（线性分配，不释放单个对象）
template<typename T>
class LinearAllocator {
    static constexpr size_t POOL_SIZE = 1024 * 1024;  // 1MB
    char pool_[POOL_SIZE];
    size_t offset_ = 0;
public:
    using value_type = T;
    
    T* allocate(size_t n) {
        size_t bytes = n * sizeof(T);
        // 对齐
        size_t aligned = (offset_ + alignof(T) - 1) & ~(alignof(T) - 1);
        if (aligned + bytes > POOL_SIZE)
            throw std::bad_alloc();
        offset_ = aligned + bytes;
        return reinterpret_cast<T*>(pool_ + aligned);
    }
    
    void deallocate(T*, size_t) noexcept {
        // 不做单个释放（batch 释放由池析构处理）
    }
};

// 使用
std::vector<int, LinearAllocator<int>> v;
// 适合：短期任务、需要避免内存碎片的场景
```

# 有状态的分配器（C++11+）

```cpp
// C++11 前，分配器必须是无状态的
// C++11 后，分配器可以持有状态

template<typename T>
class StatefulAllocator {
    std::string name_;  // 分配器可以有状态
public:
    using value_type = T;
    
    StatefulAllocator(const std::string& name) : name_(name) {}
    
    T* allocate(size_t n) {
        std::cout << "[" << name_ << "] allocate " << n << " objects\n";
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    void deallocate(T* p, size_t) noexcept {
        ::operator delete(p);
    }
    
    // 必须提供相等比较（决定两个分配器是否可以互相释放对方的内存）
    template<typename U>
    bool operator==(const StatefulAllocator<U>& other) const {
        return name_ == other.name_;
    }
    template<typename U>
    bool operator!=(const StatefulAllocator<U>& other) const {
        return !(*this == other);
    }
};

// 使用前需要注意：容器拷贝时需要决定使用哪个分配器
```

# 重载 operator new 和 operator delete

```cpp
// 全局重载（影响巨大，很少使用）
void* operator new(size_t size) {
    std::cout << "global new: " << size << " bytes\n";
    if (void* p = std::malloc(size)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept {
    std::free(p);
}

// ✅ 类级别的重载（常用，控制特定类的分配）
class Foo {
public:
    void* operator new(size_t size) {
        // 从对象池分配
        return pool.allocate(size);
    }
    void operator delete(void* p) {
        pool.deallocate(p);
    }
    static MemoryPool pool;
};
```

# 分配器适配器

```cpp
#include <scoped_allocator>  // C++11

// std::scoped_allocator_adaptor：将容器的分配器传播给容器内的元素
// 确保 vector<string> 整个使用同一分配器
using String = std::basic_string<char, std::char_traits<char>, 
                                  MyAllocator<char>>;
using Vector = std::vector<String, MyAllocator<String>>;

// 使用 scoped_allocator_adaptor 确保传播
using ScopedVec = std::vector<String, 
    std::scoped_allocator_adaptor<MyAllocator<String>>>;
```

# 何时需要自定义分配器？

| 场景 | 推荐方案 |
|------|---------|
| 大量小对象反复分配 | 固定大小对象池 |
| 避免内存碎片 | 线性分配器（bump allocator）|
| 共享内存/特定区域内存 | 区域分配器 |
| 性能 profiling 调试 | 统计分配器 |
| 特殊硬件/NUMA | 绑定分配器 |
| 99% 的日常场景 | **默认分配器就够了** |

> [!tip]- **工程要点**：不要过早自定义分配器。默认的 `::operator new` 使用 `malloc`，性能已经很好。只有当 profiling 证明分配器是瓶颈时（大量小对象、高并发分配），才考虑自定义。PMR（Polymorphic Memory Resource, C++17）提供了更现代的内存资源管理方式，是新项目的首选。

---

内存管理与指针操作紧密相关，详见 → [Pointers & References In Depth (指针与引用深入)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/03-Pointers%20&%20References%20In%20Depth%20(指针与引用深入)%20⭐.md)

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **06-RAII and Custom Allocation (RAII 与自定义分配)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
