---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

智能指针不是自动 delete 的裸指针，而是所有权语义的类型化表达：`unique_ptr` 表示唯一拥有者，`shared_ptr` 表示共享拥有者，`weak_ptr` 观察共享对象且不延长其生命周期。能用值类型就不用指针；能唯一拥有就不用共享拥有。

# 选择顺序

1. **值对象**：没有动态所有权需求时最简单。
2. **`unique_ptr`**：默认的动态所有权选择，明确 move-only 语义。
3. **`shared_ptr`**：多个独立生命周期确实共同拥有同一对象时才使用。
4. **`weak_ptr`**：不延长对象生命周期的观察者、缓存或反向引用。

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 从同一个裸指针构造两个 `shared_ptr`，会形成两个控制块并 double delete。
- 把 `shared_ptr` 当对象图的默认指针，会让所有权与循环引用难以排查。
- `weak_ptr::lock()` 后不检查结果；对象可能已销毁。
- 用智能指针管理不是 `new` 得到的地址；deleter 必须与资源获取方式匹配。

# 自测

1. 为什么工厂函数通常返回 `unique_ptr` 而不是裸指针？
2. 双向关联怎样用 `weak_ptr` 避免循环引用？
3. `make_shared` 与 `shared_ptr(new T)` 在分配和异常安全上有什么差别？

# unique ptr Ownership (unique ptr 独占所有权)

> [!note] 本节重点：核心考点：> unique_ptr 的独占所有权语义、移动语义支持、自定义删除器、与原始指针的转换

`unique_ptr` 独占资源，不可拷贝，只能移动。超出作用域时自动释放。

```cpp
// 创建（C++14 推荐用 make_unique，避免裸 new）
auto p = std::make_unique<int>(42);
auto arr = std::make_unique<int[]>(10);

// 访问
*p;          // 解引用
p.get();     // 获取裸指针（不转移所有权）
p.reset();   // 释放并置空
p.release(); // 释放所有权，返回裸指针（需手动 delete！）

// 转移所有权（只能 move，不能拷贝）
auto p2 = std::move(p);   // p 变为 nullptr，p2 接管

// 作为函数参数
void take(std::unique_ptr<int> p);     // 转移所有权
void use(const std::unique_ptr<int>& p); // 不转移，只使用
void use(int* p);                       // 更通用：直接传裸指针 p.get()
```

## 自定义删除器

```cpp
// 管理 FILE*
auto fileDeleter = [](FILE* f) { if (f) fclose(f); };
std::unique_ptr<FILE, decltype(fileDeleter)> fp(fopen("a.txt","r"), fileDeleter);

// 管理 C 资源
std::unique_ptr<void, decltype(&free)> buf(malloc(1024), free);
```

# 30 秒回答 / 自测

- **30 秒回答**：`unique_ptr` 独占所有权、不可拷贝只可移动，通常不需要独立控制块；`shared_ptr` 以引用计数表达共享所有权；`weak_ptr` 不拥有、只观察。默认首选 `unique_ptr`，确需共享才用 `shared_ptr`。自定义删除器的类型和大小仍会影响 `unique_ptr` 对象布局。
- **常见误区**：把 `p.get()` 返回的裸指针交给另一个 `unique_ptr` 管理 → 双重释放；`p.release()` 后忘记手动 `delete`。
- **自测**：1) 为什么 `unique_ptr` 能放进 `std::vector` 却不能被拷贝？ 2) 函数参数何时传 `unique_ptr`（按值）、`const unique_ptr&`、还是裸指针？

---

shared_ptr 引用计数机制详见 → [shared ptr Reference Counting Internals (引用计数底层)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/04-Smart%20Pointers%20(智能指针)%20⭐/04b-shared%20ptr%20Reference%20Counting%20Internals%20(引用计数底层).md)

---

# shared ptr Reference Counting (shared ptr 引用计数)

> [!note] 本节重点：核心考点：shared_ptr 引用计数原理、控制块结构、make_shared 的优势与限制

`shared_ptr` 允许多个指针共享同一资源，内部维护**引用计数**，计数归零时自动释放。

```cpp
auto sp1 = std::make_shared<int>(42);  // 引用计数 = 1
auto sp2 = sp1;                         // 计数 = 2
auto sp3 = sp1;                         // 计数 = 3
sp2.reset();                            // 计数 = 2
// sp3 离开作用域 → 计数 = 1
// sp1 离开作用域 → 计数 = 0 → delete
```

## 控制块（Control Block）

```text
shared_ptr Internal Structure:

  shared_ptr A                shared_ptr B
  ┌──────────────────┐       ┌──────────────────┐
  │ raw ptr: &T ─────│───────│── raw ptr: &T    │
  │                   │       │                  │
  │ ctrl blk ptr ────│───────│── ctrl blk ptr   │
  └──────────────────┘       └──────────────────┘
           │                        │
           │  ┌─────────────────────┘
           │  │
           ↓  ↓       ┌──────────────────────────┐
           └──┼──────→│  T (managed object)      │
              │       │  (heap allocated)        │
              │       └──────────────────────────┘
              │
              │       ┌──────────────────────────┐
              └──────→│  Control Block (heap)    │
                      │  ref_count    = 2        │
                      │  weak_count   = 0        │
                      │  deleter                 │
                      │  allocator               │
                      └──────────────────────────┘

Lifecycle:
  ref_count == 0               → T is destroyed
  ref_count + weak_count == 0  → Control Block is destroyed
```

- **make_shared**：一次分配，对象和控制块在同一块内存（更高效，缓存友好）
- **shared_ptr(new T)**：两次分配（对象一次，控制块一次），不推荐

```cpp
// 推荐：一次分配
auto sp = std::make_shared<MyClass>(args...);

// 不推荐：两次分配
auto sp = std::shared_ptr<MyClass>(new MyClass(args...));
```

## shared_ptr 的线程安全

- **引用计数的增减**是原子操作，线程安全
- **指针指向的对象**本身不是线程安全的，并发读写需要额外同步

---

weak_ptr 与循环引用详见 → [weak_ptr & Circular Reference (弱引用与循环引用)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/04-Smart%20Pointers%20(智能指针)%20⭐/04c-weak_ptr%20&%20Circular%20Reference%20(弱引用与循环引用).md)

---

# weak ptr and Circular References (weak ptr 与循环引用)

> [!note] 本节重点：核心考点：weak_ptr 打破循环引用、expired/lock 使用模式、weak count 与 shared count 关系

`weak_ptr` 是对 `shared_ptr` 管理对象的**非拥有观察者**，不增加强引用计数，只增加弱引用计数。

## 循环引用问题

```cpp
struct Node {
    std::shared_ptr<Node> next;   // 强引用 → 循环引用！
};

auto a = std::make_shared<Node>();
auto b = std::make_shared<Node>();
a->next = b;
b->next = a;
// a、b 各自的强引用计数 = 2，离开作用域后计数 = 1，永远不会释放 → 内存泄漏！
```

**解决：将其中一个方向改为 `weak_ptr`：**

```cpp
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node>   prev;   // 弱引用，不影响引用计数
};
```

## weak_ptr 的使用

```cpp
auto sp = std::make_shared<int>(42);
std::weak_ptr<int> wp = sp;          // 不增加强引用计数

// 使用前必须检查对象是否还存在
if (auto locked = wp.lock()) {       // lock() 返回 shared_ptr，若对象已销毁返回空
    std::cout << *locked;
} else {
    std::cout << "object destroyed";
}

wp.expired();   // 快速检查：强引用计数是否为 0（对象已销毁）
wp.use_count(); // 强引用计数
```

## 典型应用：Observer 模式 / 缓存

```cpp
// 缓存：用 weak_ptr 存缓存，对象被外部释放后自动失效
class Cache {
    std::unordered_map<int, std::weak_ptr<Object>> cache_;
public:
    std::shared_ptr<Object> get(int id) {
        auto& wp = cache_[id];
        if (auto sp = wp.lock()) return sp;   // 缓存命中
        auto sp = std::make_shared<Object>(id);
        wp = sp;                               // 更新缓存
        return sp;
    }
};
```

## 三种智能指针对比

| |unique_ptr|shared_ptr|weak_ptr|
|---|---|---|---|
|所有权|独占|共享|无|
|引用计数|无|有（原子）|只增弱计数|
|拷贝|❌|✅|✅|
|开销|零开销|控制块 + 原子操作|同 shared_ptr|
|使用场景|默认首选|共享所有权|打破循环引用、缓存、观察者|

# 30 秒回答 / 自测 · 延伸要点 2
- **30 秒回答**：`weak_ptr` 是 `shared_ptr` 的"非拥有观察者"，不增加强引用计数。用 `lock()` 临时提升为 `shared_ptr` 来安全访问，避免悬垂。
- **常见误区**：直接用 `weak_ptr` 解引用（不可行，必须先 `lock()`）；把缓存/观察者设计成 `shared_ptr` 导致对象永远不释放。
- **自测**：1) `expired()` 返回 true 与 `lock()` 返回空，两者何时结果不一致（竞态）？ 2) 为什么 `weak_ptr` 需要在控制块里额外记录 weak count？

---

shared_ptr 引用计数机制详见 → [shared ptr Reference Counting Internals (引用计数底层)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/04-Smart%20Pointers%20(智能指针)%20⭐/04b-shared%20ptr%20Reference%20Counting%20Internals%20(引用计数底层).md)

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Smart Pointers (智能指针)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
