
`shared_ptr` 允许多个指针共享同一资源，内部维护**引用计数**，计数归零时自动释放。

```cpp
auto sp1 = std::make_shared<int>(42);  // 引用计数 = 1
auto sp2 = sp1;                         // 计数 = 2
auto sp3 = sp1;                         // 计数 = 3
sp2.reset();                            // 计数 = 2
// sp3 离开作用域 → 计数 = 1
// sp1 离开作用域 → 计数 = 0 → delete
```

### 控制块（Control Block）

```
shared_ptr 对象
┌─────────────┐         ┌──────────────────────┐
│  ptr ───────┼──────>  │    Managed Object    │
│  ctrl ──────┼──┐      └──────────────────────┘
└─────────────┘  │
                 └──> ┌──────────────────────┐
                      │  strong ref count: 2 │
                      │  weak   ref count: 1 │
                      │  deleter / allocator │
                      └──────────────────────┘
```

- **make_shared**：一次分配，对象和控制块在同一块内存（更高效，缓存友好）
- **shared_ptr(new T)**：两次分配（对象一次，控制块一次），不推荐

```cpp
// 推荐：一次分配
auto sp = std::make_shared<MyClass>(args...);

// 不推荐：两次分配
auto sp = std::shared_ptr<MyClass>(new MyClass(args...));
```

### shared_ptr 的线程安全

- **引用计数的增减**是原子操作，线程安全
- **指针指向的对象**本身不是线程安全的，并发读写需要额外同步