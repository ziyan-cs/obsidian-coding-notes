---
tags:
  - cpp/core
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

> [!important] **核心考点**：Rule of Five（析构/拷贝构造/拷贝赋值/移动构造/移动赋值）、浅拷贝 vs 深拷贝

```cpp
    int*   data_;
    size_t size_;

public:
    // 构造
    Resource(size_t n) : data_(new int[n]()), size_(n) {}

    // 析构
    ~Resource() { delete[] data_; }

    // 拷贝构造（深拷贝）
    Resource(const Resource& other) : size_(other.size_) {
        data_ = new int[size_];
        std::copy(other.data_, other.data_ + size_, data_);
    }

    // 拷贝赋值（copy-and-swap 惯用法，异常安全）
    Resource& operator=(Resource other) {   // 按值传入：触发拷贝或移动
        swap(*this, other);                  // 交换资源
        return *this;
    }                                        // other 析构，释放旧资源

    // 移动构造
    Resource(Resource&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // 移动赋值
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;   size_ = other.size_;
            other.data_ = nullptr; other.size_ = 0;
        }
        return *this;
    }

    friend void swap(Resource& a, Resource& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
    }
};
```

### 编译器自动生成的规则

```cpp
// = default：显式要求编译器生成默认实现
// = delete ：禁止该操作

class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&)            = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&)                 = default;
    NonCopyable& operator=(NonCopyable&&)      = default;
};

// 若声明了析构/拷贝构造/拷贝赋值，编译器不自动生成移动操作
// → 五法则：要么全定义，要么全 default/delete
```

---

构造析构顺序详见 → [Constructor & Destructor Order (构造析构顺序)](/03-C++%20Programming%20(编程语言)/02-Core%20Mechanisms%20(核心机制)/06-Class%20Internals%20(类的底层)%20⭐/06a-Constructor%20&%20Destructor%20Order%20(构造析构顺序).md)
