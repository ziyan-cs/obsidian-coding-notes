---
tags:
  - cpp/core
status: 🌱
---

# Memory Pool Implementation — 内存池实现

> [!important] **核心考点**：内存池解决 malloc 开销与碎片问题、固定大小分配器实现

### 为什么需要内存池

频繁 `new`/`delete` 的问题：

- 系统调用开销（`malloc` 内部有锁）
- 内存碎片（长时间运行后堆碎片化）
- 缓存不友好（分配的内存分散）

### 固定大小内存池

```cpp
template<typename T, size_t BlockSize = 4096>
class MemoryPool {
    union Chunk {
        char data[sizeof(T)];
        Chunk* next;
    };
    Chunk* freeList_ = nullptr;
    std::vector<std::unique_ptr<char[]>> blocks_;

    void allocBlock() {
        auto block = std::make_unique<char[]>(BlockSize);
        size_t count = BlockSize / sizeof(Chunk);
        auto chunks = reinterpret_cast<Chunk*>(block.get());
        for (size_t i = 0; i < count - 1; i++)
            chunks[i].next = &chunks[i + 1];
        chunks[count-1].next = freeList_;
        freeList_ = chunks;
        blocks_.push_back(std::move(block));
    }

public:
    T* allocate() {
        if (!freeList_) allocBlock();
        Chunk* c = freeList_;
        freeList_ = c->next;
        return reinterpret_cast<T*>(c->data);
    }

    void deallocate(T* p) {
        auto c = reinterpret_cast<Chunk*>(p);
        c->next = freeList_;
        freeList_ = c;
    }

    template<typename... Args>
    T* construct(Args&&... args) {
        T* p = allocate();
        new (p) T(std::forward<Args>(args)...);  // placement new
        return p;
    }

    void destroy(T* p) {
        p->~T();          // 显式调用析构
        deallocate(p);
    }
};

// 使用
MemoryPool<Node> pool;
Node* n = pool.construct(42);
pool.destroy(n);
```

### Placement New

```cpp
// 在已分配的内存上构造对象（不分配内存）
char buf[sizeof(MyClass)];
MyClass* p = new (buf) MyClass(args);   // placement new
p->~MyClass();                           // 必须显式调用析构（不能 delete p！）
```

---

内存对齐对内存池实现至关重要，详见 → [Memory Alignment (内存对齐)](/03-C++%20Programming%20(编程语言)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04b-Memory%20Alignment%20(内存对齐).md)
