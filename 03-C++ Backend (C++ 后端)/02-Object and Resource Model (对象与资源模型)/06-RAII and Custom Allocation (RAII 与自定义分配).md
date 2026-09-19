---
status: stable
confidence: high
content_verified: 2026-09-19
verified: 2026-10-15
review_stage: learn
review_due: 2026-10-15
previous_review_due: 2026-09-16
---

> [!abstract] 阅读方式：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

> [!summary] 核心摘要
>
> RAII（Resource Acquisition Is Initialization）的核心是：**资源的拥有期由对象生命周期表示**。构造函数成功后对象应处于可用状态；析构函数负责释放资源，因此无论正常返回、异常抛出还是提前 `return`，作用域退出时都能回收文件、锁、socket、内存或事务等资源。实际代码优先使用标准库封装，如 `std::vector`、智能指针和 `std::lock_guard`，而不是手写 `new/delete`。

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

## 设计判断

| 场景 | 首选 | 原因 |
| --- | --- | --- |
| 独占资源 | `std::unique_ptr` 或值对象 | 所有权唯一，移动成本清楚 |
| 共享对象生命周期 | `std::shared_ptr`，谨慎使用 | 只有确实存在共享所有权时才付出引用计数成本 |
| 临界区 | `std::lock_guard` / `std::unique_lock` | 锁的获取与释放不会因异常或分支漏掉 |
| 自定义句柄 | 小型 move-only RAII wrapper | 把关闭协议集中在一个类型中 |

> [!warning]- 易错点
> - **RAII 不是“只要有析构函数就行”**：对象必须清楚表达谁拥有资源，拷贝、移动和析构的语义必须一致。
> - **`shared_ptr` 不是默认选择**：它解决共享所有权，不解决所有对象的生命周期问题；循环引用仍会泄漏。
> - **析构函数不应抛异常**：栈展开期间再次抛出通常会导致程序终止；析构中的失败应转换为可记录、可忽略或显式关闭前处理。
> - **内存池不是 RAII 替代品**：它优化分配策略，不能替代所有权、析构和异常安全设计。

> [!question]- 自测：先回答再展开
> 1. 为什么“构造函数获取、析构函数释放”比手写 `open/close` 更能保证异常安全？
> 2. 一个 socket wrapper 需要禁止拷贝、允许移动吗？为什么？
> 3. 什么时候 `shared_ptr` 反而会让设计更难排查？

# RAII and Resource Management (RAII 与资源管理)

> [!note] 本节重点：RAII 是 C++ 最核心的资源管理范式、资源获取即初始化、析构函数释放、异常安全的基础

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

# Allocation, Construction and PMR (分配、构造与内存资源)

**分配存储**和**构造对象**是两步：`operator new`/allocator 取得满足大小与对齐要求的存储，构造函数才开始对象生命周期。相应地，析构对象与释放存储也不同。`delete` 同时处理匹配的析构及释放，不可拿它释放 placement new 放在外部缓冲区的对象。

```cpp
#include <cstddef>
#include <new>
#include <string>

void placement_example() {
    alignas(std::string) std::byte storage[sizeof(std::string)];
    auto* p = new (storage) std::string{"hello"};
    p->~basic_string(); // 只析构对象；storage 是自动存储
}
```

这只是解释生命周期的最小示意，不是鼓励项目手写裸存储。对齐不足是未定义行为；构造失败时不能析构未构造完成的对象；析构函数抛异常还会破坏清理路径。实际代码优先使用标准容器和 RAII。

## 标准 allocator 与 PMR 的边界

容器通过 `std::allocator_traits` 与分配器交互。旧版几行代码的 `MyAllocator`、栈内 1 MB `LinearAllocator` 和“按名字比较相等”的有状态 allocator 都不满足可复用教学实现的完整契约：还涉及重绑定、实例间资源共享、对齐、溢出、传播和相等语义。不能把 `operator==` 随意定义为“名字相同”。

C++17 的 polymorphic memory resource（PMR）把分配策略放在运行时资源对象中，适合先学习和测量：

```cpp
#include <memory_resource>
#include <vector>

void pool_example() {
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::vector<int> values{&pool};
    for (int i = 0; i < 1000; ++i) values.push_back(i);
} // values 必须先于 pool 析构
```

`unsynchronized_pool_resource` 不能由多个线程并发共享；需要共享时考虑 `synchronized_pool_resource`。资源对象必须活得比使用它的容器和元素长。`pmr::vector<std::string>` 只保证 vector 的节点/元素存储使用资源，不会自动让普通 `std::string` 的内部字符缓冲改用 PMR；要研究嵌套资源传播时使用 `std::pmr::string` 并验证构造路径。

## 何时需要专门的分配策略

| 观察到的问题 | 下一步 |
|---|---|
| 小对象反复分配成为热点 | 比较 pool resource 与原分配器的吞吐、尾延迟和内存占用 |
| 大量对象同生共死 | 比较 monotonic resource；注意 `release()` 会让先前分配的存储失效 |
| 跨线程共享池 | 先验证同步策略，不把 unsynchronized resource 直接共享 |
| 特定对齐或设备内存 | 明确平台 API、错误处理、释放配对与对象生命周期 |

重载全局或类级 `operator new/delete` 会影响匹配的分配/释放形式及异常安全；除非有明确需求与测试，不应把示意代码当作生产分配器。先看 profile，再引入复杂性。

规范参考：[对齐规则](https://eel.is/c++draft/basic.align)、[PMR 资源](https://eel.is/c++draft/mem.res)。

---
