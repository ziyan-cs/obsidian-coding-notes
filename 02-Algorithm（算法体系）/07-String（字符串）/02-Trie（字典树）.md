#data-structure #trie #prefix-tree #string #cpp

## ⚡ TL;DR（快速决策）

- Trie 本质是：**按字符逐层存储字符串前缀的树结构**
- 一看到这些需求，要优先想到 Trie：
    - 前缀匹配
    - 单词插入与查找
    - 统计以某前缀开头的字符串数量
- 它特别适合大量字符串的前缀问题
- 如果题目重点是“前缀”，Trie 常常比普通哈希更顺手

## 🧩 Core Idea（核心本质）

- 根节点表示空前缀
- 从根到某节点的一条路径表示一个前缀
- 完整单词通常通过一个 `end` 标记结尾
- 一句话理解：
    - **Trie 就是把字符串按前缀组织成树。**
- 它强的地方在于：
    - 公共前缀能共享路径

## 🔧 Usage Patterns（可复用代码模板）

1. 基础节点定义

```cpp
struct Node {
    int nxt[26];
    bool end;
    Node() {
        memset(nxt, 0, sizeof(nxt));
        end = false;
    }
};
```

1. 插入字符串

```cpp
void insert(const string& s) {
    int p = 0;
    for (char c : s) {
        int u = c - 'a';
        if (!tr[p].nxt[u]) {
            tr[p].nxt[u] = tr.size();
            tr.push_back(Node());
        }
        p = tr[p].nxt[u];
    }
    tr[p].end = true;
}
```

1. 查找完整字符串

```cpp
bool find(const string& s) {
    int p = 0;
    for (char c : s) {
        int u = c - 'a';
        if (!tr[p].nxt[u]) return false;
        p = tr[p].nxt[u];
    }
    return tr[p].end;
}
```

1. 判断前缀是否存在

```cpp
bool startsWith(const string& s) {
    int p = 0;
    for (char c : s) {
        int u = c - 'a';
        if (!tr[p].nxt[u]) return false;
        p = tr[p].nxt[u];
    }
    return true;
}
```

## ⚠️ Pitfalls（高频错误）

- 字符集范围没处理好
- 忘记区分“前缀存在”和“完整单词存在”
- 节点数组初始化不干净
- 大字符集时直接开大数组会浪费空间

## 🚀 Performance / Tips（性能优化）

- 插入 / 查询复杂度通常和字符串长度成正比
- 字符集固定小时实现非常高效
- 高频经验：
    - 查前缀，优先想 Trie
    - 查完整字符串存在性，也可和哈希表对比选择

## 🧪 Common Scenarios（常见使用场景）

- 前缀查询
- 敏感词匹配
- 搜索建议系统
- 字典单词查找
- 最大异或 Trie（变形）

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;
int main() {
    cout << "Trie 常用于前缀匹配" << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **Trie 就是：按前缀逐层存储字符串的树形结构。**