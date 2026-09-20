---
study_stage: learn
review_due: 2026-10-19
---


> [!summary] 核心摘要
>
> 智能指针不是自动 delete 的裸指针，而是所有权语义的类型化表达：`unique_ptr` 表示唯一拥有者，`shared_ptr` 表示共享拥有者，`weak_ptr` 观察共享对象且不延长其生命周期。能用值类型就不用指针；能唯一拥有就不用共享拥有。

# 选择顺序

1. **值对象**：没有动态所有权需求时最简单。
2. **`unique_ptr`**：默认的动态所有权选择，明确 move-only 语义。
3. **`shared_ptr`**：多个独立生命周期确实共同拥有同一对象时才使用。
4. **`weak_ptr`**：不延长对象生命周期的观察者、缓存或反向引用。

> [!warning]- 易错点
> - 从同一个裸指针构造两个 `shared_ptr`，会形成两个控制块并 double delete。
> - 把 `shared_ptr` 当对象图的默认指针，会让所有权与循环引用难以排查。
> - `weak_ptr::lock()` 后不检查结果；对象可能已销毁。
> - 用智能指针管理不是 `new` 得到的地址；deleter 必须与资源获取方式匹配。

> [!question]- 自测：先回答再展开
> 1. 为什么工厂函数通常返回 `unique_ptr` 而不是裸指针？
> 2. 双向关联怎样用 `weak_ptr` 避免循环引用？
> 3. `make_shared` 的合并分配有什么优势？长期 `weak_ptr` 与自定义删除器会怎样影响选择？

# 独占所有权：`unique_ptr`

> [!note] 本节重点： unique_ptr 的独占所有权语义、移动语义支持、自定义删除器、与原始指针的转换

`unique_ptr` 独占资源，不可拷贝，只能移动。超出作用域时自动释放。

```cpp
#include <memory>
#include <utility>

void take(std::unique_ptr<int> owned); // 接管所有权
void use(const int& borrowed);          // 只借用一个非空对象

void example() {
    auto p = std::make_unique<int>(42);
    use(*p);                  // 借用，不转移所有权
    int* observer = p.get();  // 只观察；p 释放后 observer 失效
    (void)observer;

    auto next = std::move(p); // 所有权转移；p 现在为空
    next.reset();             // 释放所拥有的 int
}
```

`get()` 不交出所有权；`release()` 则交出原始指针，必须由调用方用**匹配的删除策略**接管。普通 `new int` 得到的对象可 `delete`，自定义 deleter 或 C 资源不能机械套用 `delete`。

## 自定义删除器

```cpp
// 管理 FILE*
auto fileDeleter = [](FILE* f) { if (f) fclose(f); };
std::unique_ptr<FILE, decltype(fileDeleter)> fp(fopen("a.txt","r"), fileDeleter);

// 管理 C 资源
std::unique_ptr<void, decltype(&free)> buf(malloc(1024), free);
```


# 共享所有权：`shared_ptr`

> [!note] 本节重点：shared_ptr 引用计数原理、控制块结构、make_shared 的优势与限制

`shared_ptr` 允许多个指针共享同一资源，内部维护**引用计数**，计数归零时自动释放。

```cpp
auto sp1 = std::make_shared<int>(42);  // 引用计数 = 1
auto sp2 = sp1;                         // 计数 = 2
auto sp3 = sp1;                         // 计数 = 3
sp2.reset();                            // 计数 = 2
// sp3 离开作用域 → 计数 = 1
// sp1 离开作用域 → 计数 = 0 → delete
```

## 控制块与两种生命周期

`shared_ptr` 通过共享的所有权状态协调多个拥有者；多数实现将强/弱计数、删除器等信息放在控制块中，但指针个数、字段布局与具体计数表示不是标准规定的 ABI。还要区分 `shared_ptr` **存储的指针**与它**拥有的对象**：别名构造可让二者不同。

```text
shared_ptr A ----+
                 +---- shared ownership state ---- managed object
shared_ptr B ----+                ^
                                  | weak observation
weak_ptr W -----------------------+
```

最后一个强拥有者消失时，被管理对象被销毁；只要弱观察者仍需区分“对象已销毁”，相关所有权状态还需要留存。若对象和控制块同块分配，弱引用存续期间可能延迟**整块存储**的归还，虽然对象已析构。不要把图中的字段、地址或计数相加公式当标准保证。


- **make_shared**：实现通常将对象与所有权状态合并分配；标准建议不超过一次分配，但不应把具体布局写死。弱引用长期存续可能让合并块延迟归还。
- **shared_ptr(new T)**：通常分别分配对象与控制块；在需要自定义删除器或其他特殊所有权边界时仍可能有用，不是一律禁用。

```cpp
// 一般优先：实现通常合并分配对象和控制块
auto coallocated = std::make_shared<int>(42);

// 需要独立分配或特定删除器等场景可选此形式
auto separate = std::shared_ptr<int>(new int(42));
```

## shared_ptr 的线程安全

- 不同 `shared_ptr` 对象即使共享所有权，也可以由不同线程各自复制或销毁；控制块会协调所有权计数。
- **同一个 `shared_ptr` 实例**若被多线程同时读写，仍需要外部同步或 C++20 的 `std::atomic<std::shared_ptr<T>>`。
- **所指对象**的并发读写也须按自身规则同步；引用计数安全不等于对象安全。

---

# 弱观察与循环引用：`weak_ptr`

> [!note] 本节重点：weak_ptr 打破循环引用、expired/lock 使用模式、weak count 与 shared count 关系

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
// a、b 的强引用计数离开作用域后仍为 1；若不主动打破环，析构不会发生并造成泄漏
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

wp.expired();   // 仅观察瞬时状态；随后可能变化，真正使用应调用 lock()
wp.use_count(); // 强引用计数
```

## 典型应用：Observer 模式 / 缓存

```cpp
// 单线程示意：weak_ptr 不延长对象寿命；并发访问 cache_ 还需要锁
struct Object { explicit Object(int value) : id(value) {} int id; };
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
|所有权状态|无共享计数|共享强所有权状态|共享观察状态，不增加强拥有者|
|拷贝|❌|✅|✅|
|典型开销|接近裸指针；自定义 deleter 可能影响对象大小|控制块 + 引用计数操作|共享控制块，不增加强计数|
|使用场景|默认首选|共享所有权|打破循环引用、缓存、观察者|
