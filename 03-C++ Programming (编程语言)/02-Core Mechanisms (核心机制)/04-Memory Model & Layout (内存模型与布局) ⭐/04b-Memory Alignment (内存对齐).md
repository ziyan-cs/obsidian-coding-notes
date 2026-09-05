---
tags:
  - cpp/core
status: learning
review_due: 2026-09-12
confidence: 1
verified: stable
---

> [!important] **核心考点**：内存对齐规则、padding 与 sizeof、alignof/alignas 关键字

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

内存布局与内存池实现详见 → [Stack, Heap, BSS, Text Segments (四区详解)](/03-C++%20Programming%20(编程语言)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04a-Stack,%20Heap,%20BSS,%20Text%20Segments%20(四区详解).md) · [Memory Pool Implementation (内存池实现)](/03-C++%20Programming%20(编程语言)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04c-Memory%20Pool%20Implementation%20(内存池实现).md)
