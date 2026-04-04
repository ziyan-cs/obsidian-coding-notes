#algorithm #two-pointers #array #string #optimization

## ⚡ TL;DR（快速决策）

- 双指针本质是：**用两个位置指针协同移动，减少重复枚举**
- 一看到这些特征，要优先想到双指针：
    - 有序数组中的两数之和
    - 区间收缩 / 扩张
    - 原地去重、分组、划分
    - 回文判断、字符串匹配
- 它的核心不是“两个指针”本身，而是：**通过移动规则降低复杂度**
- 许多 $O(n^2)$ 枚举能被优化成 $O(n)$ 或 $O(n \log n)$

## 🧩 Core Idea（核心本质）

- 双指针常见形态：
    - 对撞指针：一左一右向中间移动
    - 同向指针：快慢指针 / 滑动推进
- 一句话理解：
    - **让两个指针各司其职，共同缩小搜索范围。**
- 双指针成立的关键通常是：
    - 序列有序
    - 某种单调性成立
    - 可以在移动中维护区间性质

## 🔧 Usage Patterns（可复用代码模板）

1. 有序数组两数之和

```cpp
#include <iostream>
#include <vector>
using namespace std;

bool twoSumSorted(const vector<int>& a, int target) {
    int l = 0, r = (int)a.size() - 1;
    while (l < r) {
        int s = a[l] + a[r];
        if (s == target) return true;
        if (s < target) ++l;
        else --r;
    }
    return false;
}
```

2. 快慢指针去重

```cpp
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 1;
    for (int fast = 1; fast < (int)nums.size(); ++fast) {
        if (nums[fast] != nums[fast - 1]) {
            nums[slow++] = nums[fast];
        }
    }
    return slow;
}
```

3. 回文判断

```cpp
bool isPalindrome(const string& s) {
    int l = 0, r = (int)s.size() - 1;
    while (l < r) {
        if (s[l] != s[r]) return false;
        ++l; --r;
    }
    return true;
}
```

4. 分离奇偶 / 划分数组

```cpp
while (l < r) {
    while (l < r && a[l] % 2 == 1) ++l;
    while (l < r && a[r] % 2 == 0) --r;
    if (l < r) swap(a[l], a[r]);
}
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：没有单调性却硬套双指针
- 最常见错误 2：指针移动条件写错导致死循环
- 最常见错误 3：快慢指针覆盖数据时顺序错误
- 最常见错误 4：对撞指针常忘记先排序
- 最常见错误 5：边界条件 `l < r` / `l <= r` 混乱

## 🚀 Performance / Tips（性能优化）

- 经典对撞 / 同向双指针常见为 $O(n)$
- 若前置要排序，则整体常是 $O(n \log n)$
- 高频经验：
    - 题目提“有序数组”时，对撞指针敏感度要高
    - 原地去重、压缩、分组时，快慢指针很常见

## 🧪 Common Scenarios（常见使用场景）

- 两数之和（有序版）
- 删除重复元素
- 回文判断
- 分区问题
- 合并有序序列的线性扫描

## 🆚 Two Pointers vs Sliding Window

- 双指针更广，是总思维
- 滑动窗口是双指针中专门处理“连续区间”的一种模型

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {1, 2, 4, 7, 11};
    int l = 0, r = (int)a.size() - 1;
    while (l < r) {
        int s = a[l] + a[r];
        if (s == 9) break;
        if (s < 9) ++l;
        else --r;
    }
    cout << l << ' ' << r << '\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **双指针就是：通过两个指针的协同移动，利用单调性或结构性质高效缩小搜索范围。**