#stl #list #cpp #linked-list #container

## ⚡ TL;DR（快速决策）

- `list` 本质是：**STL 里的双向链表容器**
- 一看到这些需求，要优先想到 `list`：
    - 已知位置后要频繁插入 / 删除
    - 需要链表式结构修改
    - 需要 `splice` 这类节点搬运操作
- `list` 不适合随机访问
- 如果你需要 `v[i]` 这种下标访问，就别用 `list`
- 在大多数算法题里，默认还是先想 `vector`，不是 `list`

## 🧩 Core Idea（核心本质）

- `list` 底层通常是双向链表
- 每个节点知道前驱和后继
- 优势：
    - 已知迭代器位置后插入 / 删除方便
- 劣势：
    - 不连续存储
    - 不支持随机访问
    - 缓存友好性差
- 一句话理解：
    - **`list` = 标准库里的链表容器。**

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础使用

```cpp
#include <iostream>
#include <list>
using namespace std;

int main() {
    list<int> lst = {1, 2, 3};
    lst.push_front(0);
    lst.push_back(4);

    for (int x : lst) cout << x << ' ';
    return 0;
}
```

1. 插入和删除

```cpp
auto it = lst.begin();
++it;
lst.insert(it, 100);
lst.erase(it);
```

1. 排序与反转

```cpp
lst.sort();
lst.reverse();
```

1. `splice` 基础理解

```cpp
list<int> a = {1, 2};
list<int> b = {3, 4};
a.splice(a.end(), b);
```

## ⚠️ Pitfalls（高频错误）

- `list` 不能 `lst[0]`
- `sort()` 用成员函数，不是总靠 `std::sort`
- 节点分散，遍历局部性不如 `vector`
- 算法题里很多人滥用 `list`，其实并不划算

## 🚀 Performance / Tips（性能优化）

- 已知位置插删通常较灵活
- 但查找某个位置本身可能还是 $O(n)$
- 所以“查找 + 删除”未必就比 `vector` 快
- 实战建议：
    - 只有当链表结构优势非常明显时，再考虑 `list`
    - 普通顺序存储默认还是优先 `vector`

## 🧪 Common Scenarios（常见使用场景）

- 双向链表场景
- 需要频繁中间插删且已知位置
- 容器搬运、节点重接
- 学习 STL 链表容器特性

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <list>
using namespace std;

int main() {
    list<int> lst = {2, 1, 3};
    lst.push_front(0);
    lst.sort();

    for (int x : lst) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`list` 就是：标准库中的双向链表容器，擅长结构修改，不擅长随机访问。**