---
tags:
  - cpp/core
status: 🌱
---

> **核心考点**：自定义分配器用于高性能场景（内存池、特定分配策略）、placement new 构造对象、operator new 重载

## Placement New

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

## 标准分配器接口（std::allocator）

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

## 实现自定义分配器

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

## 有状态的分配器（C++11+）

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

## 重载 operator new 和 operator delete

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

## 分配器适配器

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

## 何时需要自定义分配器？

| 场景 | 推荐方案 |
|------|---------|
| 大量小对象反复分配 | 固定大小对象池 |
| 避免内存碎片 | 线性分配器（bump allocator）|
| 共享内存/特定区域内存 | 区域分配器 |
| 性能 profiling 调试 | 统计分配器 |
| 特殊硬件/NUMA | 绑定分配器 |
| 99% 的日常场景 | **默认分配器就够了** |

> **工程要点**：不要过早自定义分配器。默认的 `::operator new` 使用 `malloc`，性能已经很好。只有当 profiling 证明分配器是瓶颈时（大量小对象、高并发分配），才考虑自定义。PMR（Polymorphic Memory Resource, C++17）提供了更现代的内存资源管理方式，是新项目的首选。

---

内存管理与指针操作紧密相关，详见 → [Pointers & References In Depth (指针与引用深入)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/03-Pointers%20&%20References%20In%20Depth%20(指针与引用深入)%20⭐.md)
