---
tags:
  - career/interview
status: 🌱
---

# 01-C++ Backend Roadmap (C++ 后端路线)

> [!abstract] 阅读定位
>
> 核心思路
> 以 C++17 为主线，让核心基础在新特性学习中自然扎根。目标是：**给你一个工程，你能独立接手、读得懂、改得动、写得出。**

```
你现在的状态：
  基础语法 ✅  核心机制 ✅  面向对象 ✅
  └─ 从这里出发，用新特性把零散的知识点串起来

目标状态：
  拿到任意 C++17 项目 → 读得懂代码意图 → 改得动逻辑 → 写得符合工程规范
  C++20 特性 → 知道什么时候用，什么时候不用
```

---

## 阶段一：C++17 工程化实战（现在→2 个月）

> [!tip]- **核心策略**
> 不以语法分类，而是以"你要解决什么问题"来学。每个新特性背后都牵着一条核心基础的线。

### 1. 值语义与所有权——从智能指针打通内存模型

以 **unique_ptr / shared_ptr / move 语义**为入口，连点成线：

```
学习入口：unique_ptr（独占所有权） → 为什么不让拷贝？→ 移动语义是什么 → 右值引用
                                 → 引出：栈 vs 堆的生命周期 → RAII 思想
                                 → 对比：裸指针什么时候仍然必要
```

| 新特性抓手 | 自然牵引出的核心基础 |
|-----------|-------------------|
| `unique_ptr` + 自定义删除器 | RAII 本质、析构时机、异常安全 |
| `shared_ptr` 引用计数 | 内存布局、控制块、线程安全（引用计数本身的原子性） |
| `weak_ptr` + `lock()` | 循环引用、观察者模式 |
| `std::move` / 右值引用 | 左值右值区分、拷贝 vs 移动、对象生命周期 |
| 返回值优化（RVO/NRVO） | 编译器优化、何时依赖 move 何时不依赖 |

**验证标准：** 能徒手解释 `shared_ptr` 的线程安全性（引用计数安全但对象不保证），能在项目中合理选择 unique/share/weak。

### 2. 泛型编程——从 Concept 倒推模板基础

C++20 的 Concept 让模板变得可读，但要用好它需要理解背后的模板机制：

```
学习入口：Concept（约束模板参数）→ 为什么比 SFINAE 好 → 模板推导规则
                                 → 引出：模板特化 → 类型萃取（type_traits）
                                 → if constexpr 替代 SFINAE 的繁琐写法
```

| 新特性抓手 | 自然牵引出的核心基础 |
|-----------|-------------------|
| Concept + `requires` | 模板参数推导、SFINAE 原理（了解即可） |
| `if constexpr` | 编译期分支、替代标签分发 |
| `auto` / `decltype` | 类型推导规则、decltype(auto) |
| `std::optional` / `variant` | 类型安全、替代哨兵值/union |
| Structured Binding | 解包 pair/tuple，代码可读性 |

**验证标准：** 能在项目里用 Concept 写出清晰的泛型接口，而不是「模板报错看不懂就删掉」。

### 3. Lambda 与函数式风格——从闭包到泛型 Lambda

```
学习入口：lambda 捕获方式 → 捕获的生命周期 → 按值 vs 按引用
                                 → 泛型 lambda → 与 std::function 的取舍
                                 → 作为算法库的胶水代码（std::sort、std::find_if）
```

| 新特性抓手 | 自然牵引出的核心基础 |
|-----------|-------------------|
| lambda + 初始化捕获（C++14） | 移动仅移类型入 lambda、闭包对象模型 |
| 泛型 lambda（C++14） | 模板推导在 lambda 中的体现 |
| `string_view` | 零拷贝视图、生命周期管理、与 `std::string` 的转换关系 |

**验证标准：** 能写出既简洁又生命周期正确的 lambda，知道什么时候该用 lambda 什么时候该用普通函数。

### 4. 工程基础配套（同时进行）

与你学新特性同步，逐步建立工程习惯：

```
CMake 现代写法（target 导向，别用变量满天飞）
  → 至少：target_link_libraries + target_include_directories
  → 进阶：FetchContent、vcpkg / Conan 集成

编译期质量门禁：
  → -Wall -Wextra -Werror（不是折磨，是习惯）
  → AddressSanitizer / UBSan 日常开启
  → clang-tidy 静态分析

Git 协作基本功：
  → 能看懂别人的 PR，能写出清晰的 commit message
  → 冲突解决不是靠「重来」，而是 merge/rebase/rerere
```

**验证标准：** 新开一个 C++ 项目，从零搭 CMakeLists.txt、开 sanitizer、配 CI，不卡壳。

### 里程碑

```
拿到一个 C++17 中等规模项目（比如你笔记里的 Web Server 或 RPC 框架），
能独立做到：
  ✅ 读懂所有代码（包括模板、lambda、智能指针用法）
  ✅ 改一个功能点不出内存问题
  ✅ 用 CMake + Sanitizer 构建和调试
  ✅ 代码风格和项目保持一致
```

---

## 阶段二：并发与实战能力（2→4 个月）

### 1. 并发编程——从 std::thread 到 lock-free

```
学习入口：std::thread + lambda（用你会的东西启动线程）
                                 → 遇到竞态 → mutex + lock_guard → lock 分类
                                 → 需要同步 → condition_variable → 为什么必须配 mutex
                                 → 性能敏感 → atomic + memory order → 从 relaxed 到 acquire/release
```

| 新特性抓手 | 自然牵引出的核心基础 |
|-----------|-------------------|
| `std::thread` + lambda | 线程生命周期、join/detach、资源管理 |
| `std::lock_guard` / `unique_lock` | RAII 在并发中的应用、锁的粒度控制 |
| `std::atomic` + memory order | CPU 内存模型、缓存一致性、指令重排 |
| `std::future` / `promise` / `async` | 异步结果传递、线程间通信 |
| `shared_mutex`（C++17） | 读写锁适用场景、与互斥锁的取舍 |

**验证标准：** 能徒手写出带线程池的正确代码，能解释 memory_order_acquire/release 保证的是什么。

### 2. 性能工程——让代码跑得快

```
学习入口：perf 火焰图（用工具说话，不靠猜）
                                 → 发现瓶颈 → 分析是 CPU 还是 IO
                                 → 伪共享（false sharing）→ cache line 对齐
                                 → 内存池 → 定位频繁 malloc 的热点
```

| 工具/技术 | 解决什么问题 |
|-----------|-----------|
| perf / flamegraph | 「我的程序慢在哪」 |
| AddressSanitizer | 「内存越界/泄漏在哪」 |
| Valgrind / heaptrack | 「内存都分配去哪了」 |
| cacheline padding / `alignas` | 「多线程怎么变慢了」（伪共享） |
| 内存池 / tcmalloc | 「malloc 太多了怎么优化」 |

**验证标准：** 能对一个慢程序做 profiling，定位瓶颈，提出优化方案并验证效果。

### 里程碑

```
能在多线程环境下写出正确的代码（不死了、不慢了），且知道为什么对。
  ✅ 能设计线程池，说清线程数选多少
  ✅ 会定位并发 bug（死锁、数据竞争、ABA）
  ✅ 会用工具（perf/sanitizer）而不是靠眼神调试
```

---

## 阶段三：C++20 视野拓展（4→5 个月，持续积累）

不追求全面铺开，挑**最有产出比**的：

### Concept（最高优先级）
```
你现在：模板报错 → 看不懂 → 删掉重写
学完后：用 Concept 约束模板参数 → 报错清晰 → 接口自文档化

核心场景：
  - 替代 SFINAE 的 enable_if 写法
  - 约束泛型算法参数类型
  - 结合 if constexpr 做编译期分支
```

### Coroutines（视项目需要）
```
适用场景：异步 IO、生成器、惰性求值
不适用场景：CPU 密集计算

关键理解：
  - co_await / co_return / co_yield 三件套
  - Promise 和 Awaitable 两个协议
  - 协程 vs 线程：不是替代关系
```

### Ranges（数据处理管道）
```
std::vector<int> v = ...;
auto r = v | views::filter([](int n) { return n % 2 == 0; })
           | views::transform([](int n) { return n * n; });
// 惰性求值，不产生中间容器
// 适合复杂数据处理管道，不适合性能热点路径
```

**验证标准：** 能在 code review 中识别「这里用 Concept 更好」「这里用 Coroutine 不划算」。

---

## 持续贯穿：日常工程习惯

这些不是「学完」的事，而是每次写代码都做的事：

```
每次提交前：
  □ 编译通过无警告（-Wall -Wextra -Werror）
  □ AddressSanitizer 跑过
  □ 考虑过异常安全（RAII 覆盖了吗）
  □ 考虑过线程安全（共享数据加锁了吗）
  □ 命名一致、不冗余

每个项目起步：
  □ CMakeLists.txt 用 target 命令而非变量
  □ 分离头文件与实现
  □ 配置 clang-format 统一风格
```

---

## 推荐实践项目（按阶段匹配）

| 阶段 | 项目 | 目的 |
|------|------|------|
| 一 | 用 C++17 重构你的笔记模板库 `code/` | 把学到的现代 C++ 落到实际代码里，用 Concept + lambda + 智能指针重写 |
| 一 | 小工具：文件搜索/日志解析器 | 练习 string_view、lambda、optional、CMake |
| 二 | 压力测试你的 Web Server | 用 perf 找瓶颈，用 sanitizer 找 bug，体验真实调优 |
| 二 | 给线程池加个工作窃取 | 进阶并发能力：无锁队列、任务窃取、NUMA 感知 |
| 三 | 选一个模块用 Concept 或 Coroutine 改造 | 不是为了用而用，而是对比改造前后的可读性和性能 |

---

> [!tip]- **核心建议**
> - **不要「先学完再用」**。每个阶段的知识点学到六七成就开始动手，遇到问题回来查笔记——笔记的作用是字典，不是小说。
> - **不要追求「所有特性都记住」**。C++17/20 特性很多，记住"有这个东西、解决什么问题、怎么查"就够了。真的写的时候再回来翻笔记。
> - **你的笔记库就是你的记忆外挂**。学到一个新特性，在这个文件里改 `status: 🌱` → `🌿`，日积月累你能清楚看到自己的成长。
