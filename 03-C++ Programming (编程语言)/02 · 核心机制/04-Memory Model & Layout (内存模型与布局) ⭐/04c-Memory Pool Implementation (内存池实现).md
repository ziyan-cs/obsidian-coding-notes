
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
        char  data[sizeof(T)];
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