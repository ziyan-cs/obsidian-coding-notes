---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Memory Layout and Allocation (内存布局与分配)

> [!abstract] 阅读定位
>
> 本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## 04-Memory Layout (内存布局)

> [!abstract] 核心考点：> 进程内存四区的划分与作用、堆与栈的区别、BSS/data/text 各自存放什么

```text
┌──────────────────────────────────────────────┐
│  HIGH ADDRESS                                │
├──────────────────────────────────────────────┤
│  Stack                                       │
│    (local variables, function parameters,    │
│     return addresses)                        │
│    ↓ grows downward                          │
├──────────────────────────────────────────────┤
│  Heap                                        │
│    (dynamic allocation via new/malloc)       │
│    ↑ grows upward                            │
├──────────────────────────────────────────────┤
│  BSS Segment                                 │
│    (uninitialized global/static variables,   │
│     zero-filled at program startup)          │
├──────────────────────────────────────────────┤
│  Data Segment                                │
│    (initialized global/static variables)     │
├──────────────────────────────────────────────┤
│  Text Segment (Code Segment)                 │
│    (read-only, stores machine instructions)  │
├──────────────────────────────────────────────┤
│  LOW ADDRESS                                 │
└──────────────────────────────────────────────┘
```

```cpp
int   g_init   = 42;          // Data 段（已初始化全局变量）
int   g_uninit;               // BSS 段（未初始化全局变量，自动清零）
static int s_var = 10;        // Data 段

void foo() {
    int local = 1;            // 栈（函数返回时自动释放）
    static int s = 0;         // Data/BSS 段（static 局部变量，只初始化一次）
    int* p = new int(2);      // 堆（需要手动 delete，或用智能指针）
}
// 指令本身在 Text 段
```

### 栈 vs 堆

| |栈|堆|
|---|---|---|
|分配方式|自动（移动 SP）|手动（new/malloc）|
|速度|极快（O(1)）|较慢（需找空闲块）|
|大小|有限（默认 8MB）|受虚拟内存限制|
|生命周期|作用域结束自动释放|手动管理（或智能指针）|
|碎片|无|有（长时间运行后）|

---

内存布局相关概念详见 → [Memory Alignment (内存对齐)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04b-Memory%20Alignment%20(内存对齐).md) · [Memory Pool Implementation (内存池实现)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04c-Memory%20Pool%20Implementation%20(内存池实现).md)

---

## 05-Memory Alignment (内存对齐)

> [!abstract] 核心考点：内存对齐规则、padding 与 sizeof、alignof/alignas 关键字

### 对齐规则

每种类型有**对齐要求（alignment requirement）**，通常等于其大小，结构体成员必须放在对齐地址上，编译器自动插入填充字节（padding）：

```cpp
struct Bad {
    char  a;    // 1 字节，offset 0
    // 3 字节 padding
    int   b;    // 4 字节，offset 4（4 的倍数）
    char  c;    // 1 字节，offset 8
    // 3 字节 padding（使结构体总大小为 4 的倍数）
};
// sizeof(Bad) = 12，而非 6

struct Good {
    int   b;    // 4 字节，offset 0
    char  a;    // 1 字节，offset 4
    char  c;    // 1 字节，offset 5
    // 2 字节 padding
};
// sizeof(Good) = 8（大字段放前面，减少 padding）
```

```cpp
// 查询
alignof(int);              // 4
alignof(double);           // 8
sizeof(Bad);               // 12

// 手动指定对齐（C++11）
struct alignas(16) SIMD { float data[4]; };
alignas(64) char cacheline_buf[64];  // 对齐到 cache line
```

### 结构体大小计算规则

1. 每个成员放在自身对齐大小的整数倍偏移处
2. 结构体总大小是**最大成员对齐大小**的整数倍

```cpp
struct Example {
    char   a;    // 1,  offset=0
    // pad 1
    short  b;    // 2,  offset=2
    int    c;    // 4,  offset=4
    double d;    // 8,  offset=8
    char   e;    // 1,  offset=16
    // pad 7     使总大小为 8 的倍数
};
// sizeof = 24

// 验证
static_assert(offsetof(Example, b) == 2);
static_assert(offsetof(Example, d) == 8);
static_assert(sizeof(Example) == 24);
```

---

内存布局与内存池实现详见 → [Stack, Heap, BSS, Text Segments (四区详解)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04a-Stack,%20Heap,%20BSS,%20Text%20Segments%20(四区详解).md) · [Memory Pool Implementation (内存池实现)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04c-Memory%20Pool%20Implementation%20(内存池实现).md)

---

## 06-Memory Pool (内存池)

> [!abstract] 核心考点：内存池解决 malloc 开销与碎片问题、固定大小分配器实现

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

内存对齐对内存池实现至关重要，详见 → [Memory Alignment (内存对齐)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04b-Memory%20Alignment%20(内存对齐).md)