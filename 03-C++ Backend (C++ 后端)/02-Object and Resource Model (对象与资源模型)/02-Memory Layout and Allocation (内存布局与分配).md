---
status: stable
confidence: high
content_verified: 2026-09-19
verified: 2026-09-28
review_stage: learn
review_due: 2026-09-28
previous_review_due: 2026-09-10
---

> [!abstract] 学习目标：分清 C++ 对象存储期、操作系统进程映射和 ABI 对齐布局，不把常见 Linux 图当成标准保证。

> [!summary] 核心摘要
>
> 内存布局回答对象和程序的字节在哪里；分配策略回答谁申请、谁释放以及分配成本是否值得优化。先用值类型、容器和 RAII 管理资源；对齐、placement new、内存池只在 profile 或明确约束证明必要时使用。

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

> [!question]- 自测：先回答再展开
> 1. 为什么成员顺序会改变 `sizeof(T)`？
> 2. 什么证据足以支持引入内存池？
> 3. placement new 后为什么仍需显式析构？

# Memory Layout (内存布局)

> [!note] 下面是典型 Linux ELF 进程的教学示意，不是 C++ 规定的固定“四区”。ASLR、共享库、线程栈和 mmap 会让实际地址空间更复杂，栈/堆也不要求按图中方向相向增长。

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
#include <memory>

int   g_init   = 42;          // 静态存储期；典型实现放在 data 段
int   g_uninit;               // 静态存储期、零初始化；典型实现放在 BSS
static int s_var = 10;        // 静态存储期

void foo() {
    int local = 1;            // 自动存储期；实现可放寄存器或优化掉
    static int s = 0;         // 静态存储期，首次经过声明时初始化
    auto p = std::make_unique<int>(2); // 动态存储，由 RAII 管理
}
// 典型 ELF 文件中的指令映射来自 text 段；段位置不由 C++ 规定
```

## 栈 vs 堆

| |栈|堆|
|---|---|---|
|分配方式|自动存储期，具体可由编译器优化|动态分配接口/分配器；通常交给 RAII 管理|
|成本|通常无需显式分配调用，但仍受对象构造成本影响|依实现、尺寸、缓存与竞争而变，需测量|
|容量|线程栈限制依平台和配置，不能记固定值|受地址空间、限制和可用资源约束|
|生命周期|离开作用域结束（例外见延长生命周期规则）|由所有者决定，常通过 RAII 结束|
|碎片|不是这种分配方式的主要问题|可能产生，取决于分配模式和分配器|

---

# Memory Alignment (内存对齐)

> [!note] 本节重点：内存对齐规则、padding 与 sizeof、alignof/alignas 关键字

## 对齐规则

每种对象类型都有**对齐要求（alignment requirement）**，但不能推断它通常等于 `sizeof(T)`。实现可为成员和对象增加 padding；下列偏移与大小是常见 ABI 下的观察值，不是跨平台公式：

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

1. 对象地址必须满足 `alignof(T)`；实现安排成员时还要遵守成员顺序和布局规则。
2. `sizeof(T)` 是 `alignof(T)` 的整数倍，以便数组元素连续放置；显式 `alignas` 也可能提高整个结构体的对齐要求。

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

// 在目标平台用 sizeof(Example)、alignof(Example) 和
// offsetof(Example, b) 等观察实际布局；不要把示例数值做跨平台断言。
```

---

# Memory Pool (内存池)

> [!note] 本节重点：先证明分配是瓶颈，再选择内存资源；池化必须守住对齐、生命周期和回收边界。

## 为什么需要内存池

频繁 `new`/`delete` 的问题：

- 分配器本身可能有开销，但一次 `new`/`malloc` 不等于一次系统调用；不同实现的锁策略也不同。
- 内存碎片（长时间运行后堆碎片化）
- 缓存不友好（分配的内存分散）

## 先使用标准内存资源

旧版固定大小池直接把 `char[]` 转成 `Chunk*` 使用，未证明 `T` 的对齐、块容量、节点对象生命周期和构造失败时的回收；照抄可能导致未定义行为。用 C++17 PMR 建立正确模型：

```cpp
#include <array>
#include <cstddef>
#include <iostream>
#include <memory_resource>
#include <vector>

int main() {
    std::array<std::byte, 4096> buffer{};
    std::pmr::monotonic_buffer_resource arena{buffer.data(), buffer.size()};
    std::pmr::vector<int> values{&arena};
    for (int i = 0; i < 100; ++i) values.push_back(i);
    std::cout << values.size() << '\n';
} // values 先析构，arena 后析构，buffer 最后析构
```

`monotonic_buffer_resource` 适合一批对象共享生命周期：单次 `deallocate` 不回收，资源析构或 `release()` 时整体释放；缓冲区耗尽可向上游资源申请。不能在仍有对象使用资源时调用 `release()`。需要频繁独立回收时再比较 pool resource，并按线程共享方式选择同步版本。

---

## Placement New

```cpp
// 在已分配的内存上构造对象（不分配内存）
alignas(T) std::byte storage[sizeof(T)];
T* p = new (storage) T(args);
p->~T();  // 不可对 p 调用 delete；T 必须在此作用域中是完整类型
```

---


> [!check]- 学完后检查
> ## 复述
>
> - 不看正文，说明 02-Memory Layout and Allocation (内存布局与分配) 的问题、核心机制与边界。
>
> ## 验证
>
> - 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。
>
> ## 自测
>
> 1. 这个主题解决什么问题？
> 2. 它在什么条件下会失效、变慢或需要替代方案？

> [!info]- 延伸阅读
> - 下一步：[03-Object Lifetime and Copy Control (对象生命周期与拷贝控制)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)
