#stl #queue #cpp #fifo #container-adapter

## ⚡ TL;DR（快速决策）

- `queue` 本质是：**先进先出（FIFO）**
- 一看到这些特征，要优先想到队列：
    - 先来的先处理
    - 按顺序排队处理任务
    - 层序遍历 / BFS
    - 缓冲区、事件流、等待队列
- 常用操作：`push()`、`pop()`、`front()`、`back()`、`empty()`
- `queue` 不支持随机访问，也不支持直接遍历
- 如果题目是“先进入的元素先出来”，默认先想队列

## 🧩 Core Idea（核心本质）

- 队列是一种**操作受限的线性结构**
- 插入通常在队尾，删除通常在队头
- 最像现实中的排队
- 一句话理解：
    - **谁先进去，谁先出来。**
- STL 的 `queue` 是容器适配器，底层常基于 `deque`

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础使用

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);

    cout << q.front() << '\\n';
    q.pop();
    cout << q.front() << '\\n';
    return 0;
}
```

1. 查看队头和队尾

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);

    cout << q.front() << '\\n';
    cout << q.back() << '\\n';
    return 0;
}
```

1. BFS 基础模板

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(1);

    while (!q.empty()) {
        int x = q.front();
        q.pop();
        cout << x << ' ';
        if (x < 3) q.push(x + 1);
    }
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- 空队列上不能直接 `front()` / `back()` / `pop()`
- `pop()` 不返回被删元素
- 别把队列和栈顺序搞反
- `queue` 不能像 `vector` 一样下标访问

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - `push()`：$O(1)$
    - `pop()`：$O(1)$
    - `front()` / `back()`：$O(1)$
- BFS、按顺序处理任务时特别常用
- 如果需要双端操作，更适合 `deque`

## 🧪 Common Scenarios（常见使用场景）

- BFS / 层序遍历
- 任务排队处理
- 事件流处理
- 缓冲区模型

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);

    while (!q.empty()) {
        cout << q.front() << ' ';
        q.pop();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`queue` 就是：一个遵循先进先出规则、只允许队头出队和队尾入队的容器。**