
`weak_ptr` 是对 `shared_ptr` 管理对象的**非拥有观察者**，不增加强引用计数，只增加弱引用计数。

### 循环引用问题

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

### weak_ptr 的使用

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

### 典型应用：Observer 模式 / 缓存

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

### 三种智能指针对比

| |unique_ptr|shared_ptr|weak_ptr|
|---|---|---|---|
|所有权|独占|共享|无|
|引用计数|无|有（原子）|只增弱计数|
|拷贝|❌|✅|✅|
|开销|零开销|控制块 + 原子操作|同 shared_ptr|
|使用场景|默认首选|共享所有权|打破循环引用、缓存、观察者|