#stl #deque #cpp #double-ended-queue #container

## ⚡ TL;DR（快速决策）

- `deque` 本质是：**两端都能高效插入和删除的顺序容器**
- 一看到这些需求，要优先想到 `deque`：
    - 既要头部操作，又要尾部操作
    - 实现单调队列
    - 滑动窗口
    - 同时保留队列和数组访问的一部分优点
- 常用操作：
    - `push_front()` / `push_back()`
    - `pop_front()` / `pop_back()`
    - `front()` / `back()`
- 如果你需要两端频繁操作，比 `vector` 更自然

## 🧩 Core Idea（核心本质）

- `deque` = double-ended queue
- 可以把它理解成“加强版队列”
- 和 `queue` 的区别：
    - `queue` 是受限接口
    - `deque` 是真正容器，两端都能直接操作
- 一句话理解：
    - **`deque` 就是两头都好用的顺序容器。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础使用

```cpp
#include <deque>
#include <iostream>
using namespace std;

int main() {
    deque<int> dq;
    dq.push_back(2);
    dq.push_front(1);
    dq.push_back(3);

    cout << dq.front() << ' ' << dq.back() << '\\n';
    return 0;
}
```

1. 两端删除

```cpp
#include <deque>
#include <iostream>
using namespace std;

int main() {
    deque<int> dq = {1, 2, 3, 4};
    dq.pop_front();
    dq.pop_back();

    for (int x : dq) cout << x << ' ';
    return 0;
}
```

1. 滑动窗口基础感觉

```cpp
#include <deque>
#include <iostream>
using namespace std;

int main() {
    deque<int> dq;
    dq.push_back(5);
    dq.push_back(3);
    dq.push_front(7);

    while (!dq.empty()) {
        cout << dq.front() << ' ';
        dq.pop_front();
    }
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- 空 `deque` 上不能直接 `front()` / `back()`
- 别把 `deque` 和 `queue` 混为一谈
- 虽然也能下标访问，但别误以为它和 `vector` 完全一样
- 两端操作写反很常见

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - 头尾插删：通常较快
    - 下标访问：支持
- 滑动窗口、单调队列里特别高频
- 只做尾插 + 随机访问时，通常 `vector` 更常用

## 🧪 Common Scenarios（常见使用场景）

- 双端进出
- 滑动窗口
- 单调队列
- BFS 某些变种结构基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <deque>
#include <iostream>
using namespace std;

int main() {
    deque<int> dq;
    dq.push_front(1);
    dq.push_back(2);
    dq.push_back(3);

    while (!dq.empty()) {
        cout << dq.front() << ' ';
        dq.pop_front();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`deque` 就是：一个支持两端高效插入和删除的双端顺序容器。**