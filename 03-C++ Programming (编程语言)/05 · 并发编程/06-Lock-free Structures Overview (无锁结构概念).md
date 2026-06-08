---
tags:
  - cpp/concurrency
status: 🌱
---

> [!important] **核心考点**：无锁编程的基本思想、ABA 问题、CAS 实现、适用与不适用场景

## 什么是无锁（Lock-Free）

```cpp
// 有锁版本
std::mutex mtx;
void push(int val) {
    std::lock_guard lock(mtx);
    // 操作共享数据
}

// 无锁版本：用 CAS 原子操作，不依赖锁
std::atomic<Node*> head{nullptr};
void push(int val) {
    Node* new_node = new Node(val);
    Node* old_head = head.load();
    do {
        new_node->next = old_head;
    } while (!head.compare_exchange_weak(old_head, new_node));
}
```

**Lock-Free 的定义**：
- 任意线程挂起不会阻塞其他线程的进度
- 系统中至少有一个线程能在有限步内完成操作

## 无锁栈（Lock-Free Stack）

```cpp
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
    };
    std::atomic<Node*> head_{nullptr};

public:
    void push(const T& val) {
        Node* node = new Node{val, nullptr};
        node->next = head_.load();
        while (!head_.compare_exchange_weak(node->next, node))
            ;  // CAS 循环
    }

    bool pop(T& result) {
        Node* old_head = head_.load();
        while (old_head && 
               !head_.compare_exchange_weak(old_head, old_head->next))
            ;
        if (!old_head) return false;
        result = old_head->data;
        delete old_head;  // 注意：这里不安全（后面讲 ABA）
        return true;
    }
};
```

## ABA 问题

```cpp
// ABA 问题场景：
// 线程 1: 读取 head → Node A
// 线程 2: pop A → push B → push A（内存地址相同，但内容不同）
// 线程 1: CAS 比较 head == A → 成功！但此时 head 指向的是新的 A

// 解决方案：带上版本号（Double-width CAS / tagged pointer）
struct TaggedPointer {
    Node* ptr;
    uintptr_t tag;  // 递增版本号
};

std::atomic<TaggedPointer> head_;

// 实际代码中常用：
// - 在 x86_64 上利用指针的高 16 位存 tag（指针只有 48 位有效）
// - 或用 std::atomic<std::shared_ptr<T>> (C++20)
```

## 内存管理难题

```cpp
// 无锁结构的最大问题：何时释放内存？

// 线程 A 准备删除 Node
// 线程 B 正持有指向同一个 Node 的指针
// 线程 A delete → 线程 B 访问已释放内存 → 未定义行为

// 解决方案：
// 1. 风险指针（Hazard Pointer）：线程声明正在使用的指针
// 2. RCU（Read-Copy-Update）：延迟回收
// 3. 引用计数 std::shared_ptr 的原子版本
// 4. Epoch-Based Reclamation (EBR)
```

## 何时用无锁？

| 适合无锁 | 不适合无锁 |
|---------|-----------|
| 极高并发，锁成为瓶颈 | 实现复杂度低时 |
| 细粒度操作（push/pop） | 复合操作（需要同时改多个变量）|
| 实时系统（不能容忍等待） | T 的拷贝/移动开销大 |
| 设计简单清晰 | 需要严格的内存序保证 |

```cpp
// 实际工程中：优先用锁
// 基准测试证实锁是瓶颈后，再考虑无锁
// "Lock-free programming is like a sharp knife — useful but easy to cut yourself"
```

## C++ 中的无锁设施

| 设施 | 说明 |
|------|------|
| `std::atomic<T>` | 原子类型基础 |
| `atomic<T*>::compare_exchange_*` | CAS 操作 |
| `atomic_signal_fence` / `atomic_thread_fence` | 内存栅栏 |
| `std::atomic<shared_ptr<T>>` (C++20) | 无锁引用计数（可能）|
| `std::atomic_ref<T>` (C++20) | 非原子对象的原子操作 |

> **面试重点**：ABA 问题是必考题。说出 ABA 的含义 + 版本号方案 = 加分。不用深入 hazard pointer 细节，但要能说出"无锁编程最大的挑战是内存回收"。

---

原子操作与内存序是无锁编程的基础，详见 → [Atomic & Memory Order (原子操作与内存序)](/03-C++%20Programming%20(编程语言)/05%20·%20并发编程/04-Atomic%20&%20Memory%20Order%20(原子操作与内存序)%20⭐.md)
