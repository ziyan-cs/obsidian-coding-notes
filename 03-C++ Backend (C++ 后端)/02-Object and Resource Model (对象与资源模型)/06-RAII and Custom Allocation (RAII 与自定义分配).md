---
study_stage: learn
review_due: 2026-10-15
---


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

# RAII：先表达所有权，再谈异常安全

RAII 把资源的**拥有期**绑定到对象生命周期。构造成功后对象应维持可用不变量；析构只做不会把异常传播出去的释放工作。它适用于内存、文件、锁、套接字和事务，但具体“释放”语义不同：析构能保证调用清理逻辑，不保证刷盘成功、网络发送完成或数据库事务提交成功。

## 文件与锁的最小实例

```cpp
#include <cstdio>
#include <memory>
#include <mutex>
#include <stdexcept>

using File = std::unique_ptr<std::FILE, int (*)(std::FILE*)>;

File open_input(const char* path) {
    std::FILE* raw = std::fopen(path, "rb");
    if (!raw) throw std::runtime_error("open_input failed");
    return File{raw, &std::fclose};
}

void guarded_use(std::mutex& mutex, const char* path) {
    auto file = open_input(path);      // 独占文件；异常时也会调用 fclose
    std::lock_guard lock{mutex};       // 进入作用域加锁，退出作用域解锁
    // 处理文件；不要在持锁状态做无关、可能长期阻塞的 I/O。
}
```

`File` 不可拷贝而可移动，避免两个对象各自关闭同一 `FILE*`。若关闭错误本身影响业务正确性，不能只靠析构静默关闭：要设计可检查结果的显式 `close()`，之后将句柄置为无效，析构只做兜底。`std::lock_guard` 负责解锁，但**不会**帮你保护锁外访问，也不会消除死锁。

## 构造失败时谁负责清理

如果某个对象构造函数中第二次 `new` 抛异常，它自身的析构函数不会运行，因为对象从未完整构造；已经构造完成的成员和基类仍会逆序析构。把每项资源各自放入 RAII 成员，才能守住这一边界：

```cpp
#include <memory>

class TwoBuffers {
    std::unique_ptr<int[]> first_;
    std::unique_ptr<int[]> second_;

public:
    TwoBuffers()
        : first_(std::make_unique<int[]>(100)),
          second_(std::make_unique<int[]>(100)) {}
    // 若 second_ 分配失败，first_ 已完成构造，会自动释放。
};
```

如果类直接保存裸资源并声明析构，却沿用编译器生成的复制操作，两个副本可能重复释放。优先使用值成员、容器和独占智能指针遵循 **Rule of Zero**；确需直接拥有句柄时，删除拷贝并明确移动语义，或正确实现深拷贝。

## 异常安全不能只靠“自动释放”

- **无泄漏**只说明资源会归还，不说明对象状态仍满足不变量。
- **基本保证**：异常后仍满足不变量、没有资源泄漏；内容可能改变。
- **强保证**：失败时可观察状态不变；常见手法是先在临时对象上完成可能失败的工作，再用不抛异常的提交步骤交换。
- **不抛保证**：操作保证不让异常逃逸。析构及真正标注 `noexcept` 的移动操作尤其值得审查。

这些保证属于**具体操作与类型**，不是“用了 RAII 的项目自动拥有强保证”。例如向 `std::vector` 插入元素的保证还受元素类型和所调用重载影响；不要把 copy-and-swap 当成任意操作的万能药。

| 要保护的资源 | 首选封装 | 边界 |
| --- | --- | --- |
| 动态数组 | `std::vector` / `std::unique_ptr<T[]>` | 由容器/指针持有对象寿命 |
| 文件 | `std::ifstream` 或专用句柄 | 重要写入需显式检查刷新/关闭错误 |
| 互斥量 | `std::lock_guard` / `std::unique_lock` | 锁顺序、持锁时长仍需设计 |
| 数据库事务 | 事务 guard | 析构通常回滚；提交必须显式确认结果 |

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
