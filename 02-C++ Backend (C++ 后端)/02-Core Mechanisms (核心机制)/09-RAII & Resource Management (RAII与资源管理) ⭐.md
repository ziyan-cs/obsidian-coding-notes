---
tags:
  - cpp/core
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# RAII & Resource Management — RAII与资源管理

> [!important] **核心考点**：RAII 是 C++ 最核心的资源管理范式、资源获取即初始化、析构函数释放、异常安全的基础

## 什么是 RAII

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

## RAII 管理的资源类型

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

## 智能指针是 RAII 的典型应用

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

## RAII + 异常安全的黄金法则

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

## RAII 的常见错误

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

## 工程习惯

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
