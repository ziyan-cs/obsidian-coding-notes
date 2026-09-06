---
status: stable
confidence: high
verified: 2026-09-06
review_due: 2026-09-10
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

内存布局回答对象和程序的字节在哪里；分配策略回答谁申请、谁释放以及分配成本是否值得优化。先用值类型、容器和 RAII 管理资源；对齐、placement new、内存池只在 profile 或明确约束证明必要时使用。

# 心智模型

| 层次 | 关注问题 |
| --- | --- |
| 对象布局 | 成员顺序、padding、`alignof`、ABI 边界 |
| 栈与堆 | 生命周期、分配成本、局部性 |
| 进程地址空间 | text、data、BSS、heap、stack、mmap |
| 自定义分配 | 固定对象、碎片、析构、线程安全、诊断 |

# 工程边界

- `placement new` 只负责构造；调用方仍要在正确时机析构并归还存储。
- 内存池必须定义对象构造、析构、归还与泄漏诊断；只复用字节块不是完整设计。
- 不要为了几个字节随意改变公开结构体布局；协议、持久化格式和 ABI 需要版本策略。

# 自测

1. 为什么成员顺序会改变 `sizeof(T)`？
2. 什么证据足以支持引入内存池？
3. placement new 后为什么仍需显式析构？

# Memory Layout (内存布局)

> [!note] 本节重点：核心考点：> 进程内存四区的划分与作用、堆与栈的区别、BSS/data/text 各自存放什么

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

## 栈 vs 堆

| |栈|堆|
|---|---|---|
|分配方式|自动（移动 SP）|手动（new/malloc）|
|速度|极快（O(1)）|较慢（需找空闲块）|
|大小|有限（默认 8MB）|受虚拟内存限制|
|生命周期|作用域结束自动释放|手动管理（或智能指针）|
|碎片|无|有（长时间运行后）|

---

内存布局相关概念详见 → Memory Alignment (内存对齐) · Memory Pool Implementation (内存池实现)

---

# Memory Alignment (内存对齐)

> [!note] 本节重点：核心考点：内存对齐规则、padding 与 sizeof、alignof/alignas 关键字

## 对齐规则

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

## 结构体大小计算规则

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

内存布局与内存池实现详见 → Stack, Heap, BSS, Text Segments (四区详解) · Memory Pool Implementation (内存池实现)

---

# Memory Pool (内存池)

> [!note] 本节重点：核心考点：内存池解决 malloc 开销与碎片问题、固定大小分配器实现

## 为什么需要内存池

频繁 `new`/`delete` 的问题：

- 系统调用开销（`malloc` 内部有锁）
- 内存碎片（长时间运行后堆碎片化）
- 缓存不友好（分配的内存分散）

## 固定大小内存池

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

## Placement New

```cpp
// 在已分配的内存上构造对象（不分配内存）
char buf[sizeof(MyClass)];
MyClass* p = new (buf) MyClass(args);   // placement new
p->~MyClass();                           // 必须显式调用析构（不能 delete p！）
```

---

内存对齐对内存池实现至关重要，详见 → Memory Alignment (内存对齐)

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 复述

- 不看正文，说明 02-Memory Layout and Allocation (内存布局与分配) 的问题、核心机制与边界。

## 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

## 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

# 关联学习

- 导航：[00-Object and Resource Map (对象与资源导航)](/02-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/00-Object%20and%20Resource%20Map%20(对象与资源导航).md)
- 下一步：[03-Object Lifetime and Copy Control (对象生命周期与拷贝控制)](/02-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)
