---
tags:
  - cpp/modern
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# unique ptr Ownership Model — 独占所有权

> [!abstract] 核心考点：> unique_ptr 的独占所有权语义、移动语义支持、自定义删除器、与原始指针的转换

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

### 自定义删除器

```cpp
// 管理 FILE*
auto fileDeleter = [](FILE* f) { if (f) fclose(f); };
std::unique_ptr<FILE, decltype(fileDeleter)> fp(fopen("a.txt","r"), fileDeleter);

// 管理 C 资源
std::unique_ptr<void, decltype(&free)> buf(malloc(1024), free);
```

## 30 秒回答 / 自测

- **30 秒回答**：`unique_ptr` 独占所有权、不可拷贝只可移动，通常不需要独立控制块；`shared_ptr` 以引用计数表达共享所有权；`weak_ptr` 不拥有、只观察。默认首选 `unique_ptr`，确需共享才用 `shared_ptr`。自定义删除器的类型和大小仍会影响 `unique_ptr` 对象布局。
- **常见误区**：把 `p.get()` 返回的裸指针交给另一个 `unique_ptr` 管理 → 双重释放；`p.release()` 后忘记手动 `delete`。
- **自测**：1) 为什么 `unique_ptr` 能放进 `std::vector` 却不能被拷贝？ 2) 函数参数何时传 `unique_ptr`（按值）、`const unique_ptr&`、还是裸指针？

---

shared_ptr 引用计数机制详见 → [shared ptr Reference Counting Internals (引用计数底层)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/04-Smart%20Pointers%20(智能指针)%20⭐/04b-shared%20ptr%20Reference%20Counting%20Internals%20(引用计数底层).md)
