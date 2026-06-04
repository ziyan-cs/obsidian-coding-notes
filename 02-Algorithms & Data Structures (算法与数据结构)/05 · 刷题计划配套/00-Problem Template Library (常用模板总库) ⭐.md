---
tags:
  - algorithm
  - practice
---

> **核心考点**：常用算法模板汇总、各数据结构的操作接口速查、面试速记

## 二分查找

```cpp
// 精确查找
int binarySearch(vector<int>& nums, int target) {
    int l = 0, r = nums.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        if (nums[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}

// 左边界（第一个 >= target）
int lowerBound(vector<int>& nums, int target) {
    int l = 0, r = nums.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] < target) l = mid + 1;
        else r = mid;
    }
    return l;
}

// 右边界（最后一个 <= target）
int upperBound(vector<int>& nums, int target) {
    int l = 0, r = nums.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] <= target) l = mid + 1;
        else r = mid;
    }
    return l - 1;
}
```

## 排序

```cpp
// 快速排序
void quickSort(vector<int>& nums, int l, int r) {
    if (l >= r) return;
    int pivot = partition(nums, l, r);
    quickSort(nums, l, pivot - 1);
    quickSort(nums, pivot + 1, r);
}

int partition(vector<int>& nums, int l, int r) {
    int p = nums[r], i = l;
    for (int j = l; j < r; j++)
        if (nums[j] <= p) swap(nums[i++], nums[j]);
    swap(nums[i], nums[r]);
    return i;
}

// 归并排序
void mergeSort(vector<int>& nums, int l, int r, vector<int>& tmp) {
    if (l >= r) return;
    int mid = l + (r - l) / 2;
    mergeSort(nums, l, mid, tmp);
    mergeSort(nums, mid + 1, r, tmp);
    int i = l, j = mid + 1, k = l;
    while (i <= mid && j <= r)
        tmp[k++] = nums[i] <= nums[j] ? nums[i++] : nums[j++];
    while (i <= mid) tmp[k++] = nums[i++];
    while (j <= r) tmp[k++] = nums[j++];
    copy(tmp.begin() + l, tmp.begin() + r + 1, nums.begin() + l);
}
```

## 链表

```cpp
// 反转链表（迭代）
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    while (head) {
        ListNode* next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

// 找中点（快慢指针）
ListNode* middleNode(ListNode* head) {
    auto slow = head, fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// 合并有序链表
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0), *p = &dummy;
    while (l1 && l2) {
        if (l1->val < l2->val) { p->next = l1; l1 = l1->next; }
        else { p->next = l2; l2 = l2->next; }
        p = p->next;
    }
    p->next = l1 ? l1 : l2;
    return dummy.next;
}
```

## 二叉树遍历

```cpp
// 前序（迭代）
vector<int> preorder(TreeNode* root) {
    if (!root) return {};
    vector<int> res;
    stack<TreeNode*> st{{root}};
    while (!st.empty()) {
        auto n = st.top(); st.pop();
        res.push_back(n->val);
        if (n->right) st.push(n->right);
        if (n->left) st.push(n->left);
    }
    return res;
}

// 中序（迭代）
vector<int> inorder(TreeNode* root) {
    vector<int> res;
    stack<TreeNode*> st;
    auto cur = root;
    while (cur || !st.empty()) {
        while (cur) { st.push(cur); cur = cur->left; }
        cur = st.top(); st.pop();
        res.push_back(cur->val);
        cur = cur->right;
    }
    return res;
}

// 层序（BFS）
vector<vector<int>> levelOrder(TreeNode* root) {
    if (!root) return {};
    vector<vector<int>> res;
    queue<TreeNode*> q{{root}};
    while (!q.empty()) {
        int sz = q.size();
        vector<int> level;
        while (sz--) {
            auto n = q.front(); q.pop();
            level.push_back(n->val);
            if (n->left) q.push(n->left);
            if (n->right) q.push(n->right);
        }
        res.push_back(level);
    }
    return res;
}
```

## 图

```cpp
// 并查集
struct UnionFind {
    vector<int> parent, rank;
    UnionFind(int n) : parent(n), rank(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        return parent[x] == x ? x : (parent[x] = find(parent[x]));
    }
    void unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return;
        if (rank[x] < rank[y]) parent[x] = y;
        else if (rank[x] > rank[y]) parent[y] = x;
        else { parent[y] = x; rank[x]++; }
    }
};

// Dijkstra
vector<int> dijkstra(vector<vector<pair<int,int>>>& g, int start) {
    int n = g.size();
    vector<int> dist(n, INT_MAX);
    dist[start] = 0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    pq.push({0, start});
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto [v, w] : g[u])
            if (dist[u] + w < dist[v])
                dist[v] = dist[u] + w, pq.push({dist[v], v});
    }
    return dist;
}
```

## 动态规划

```cpp
// 0-1 背包
int knapsack01(vector<int>& w, vector<int>& v, int cap) {
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < w.size(); i++)
        for (int j = cap; j >= w[i]; j--)     // 逆序
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
    return dp[cap];
}

// 完全背包
int knapsackComplete(vector<int>& w, vector<int>& v, int cap) {
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < w.size(); i++)
        for (int j = w[i]; j <= cap; j++)     // 正序
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
    return dp[cap];
}

// LIS O(n log n)
int lengthOfLIS(vector<int>& nums) {
    vector<int> tails;
    for (int x : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;
    }
    return tails.size();
}
```

## 字符串

```cpp
// KMP next 数组
vector<int> buildNext(string& p) {
    int m = p.size();
    vector<int> next(m, 0);
    for (int i = 1, j = 0; i < m; i++) {
        while (j > 0 && p[i] != p[j]) j = next[j-1];
        if (p[i] == p[j]) j++;
        next[i] = j;
    }
    return next;
}
```

---

## 关联笔记

- [Mistake Journal (错题与易错点记录)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/05%20·%20刷题计划配套/00-Mistake%20Journal%20(错题与易错点记录)%20⭐.md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
