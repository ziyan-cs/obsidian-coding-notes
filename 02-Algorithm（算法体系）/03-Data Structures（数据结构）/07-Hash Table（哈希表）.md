#algorithm #hash #unordered_map #unordered_set


# 0. 核心

- **用空间换时间**
- 哈希函数将数据映射到数组下标，实现极速的存在性判断与映射查询
- 性能：
	- 平均 / 最好： $O(1)$（常数级，极快）
	- 最坏：$O(N)$（严重哈希冲突，极少出现）
- 高频应用场景：
    - 去重
    - 计数
    - 映射
    - 前缀和 + 哈希

# 1. 核心容器

- `unordered_map<Key, Value>`：
	- 无序键值对
	- 增删改查 $O(1)$
- `unordered_set<Key>`：
	- 无序单值集合
	- 去重 $O(1)$
- `map<Key, Value>`：
	- 有序键值对（红黑树）
	- $O(log N)$
- `set<Key>`：
	- 有序单值集合
	- $O(log N)$

# 3. 核心对比

| 特性    | 哈希表(unordered_) | 有序表(map/set)  |
| ----- | --------------- | ------------- |
| 顺序    | 无序              | 有序            |
| 时间    | 平均 $O(1)$       | 稳定 $O(log N)$ |
| Key要求 | 需哈希实现           | 需比较实现         |
| 适用场景  | 极速增删改查          | 排序/范围查找       |
| 内存占用  | 较高              | 较低            |

# 4. 自定义类存入哈希表
 
必须手动提供哈希逻辑：
```cpp
#include <unordered_set>
#include <functional> // 必须包含

// 1. 定义自定义类
struct Node {
    int value;
    Node(int v) : value(v) {}

    // 必须重载 == ，用于哈希表内部的去重比较
    bool operator==(const Node& other) const {
        return value == other.value;
    }
};

// 2. 为自定义类特化 std::hash
namespace std {
    template<> struct hash<Node> {
        size_t operator()(const Node& node) const {
            // 根据业务逻辑生成哈希值，常用 Objects.hash 或 位运算
            return hash<int>()(node.value); 
        }
    };
}

// 3. 直接使用
void test() {
    unordered_set<Node> mySet;
    mySet.insert(Node(1));
}
```
# 5. 经典案例

### 5.1 去重（Set）

```cpp
unordered_set<int> st;
for (int x : nums) st.insert(x);
```

### 5.2 计数（Map）

```cpp
unordered_map<int, int> cnt;
for (int x : nums) cnt[x]++;
```

### 5.3 两数之和

```cpp
// 
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> mp;
    for (int i = 0; i < (int)nums.size(); ++i) {
        int need = target - nums[i];
        // 检查是否已经遍历过所需的数
        if (mp.find(need) != mp.end()) { 
            return {mp[need], i};
        }
        // 没找到就把当前数存进去，供后续查找
        mp[nums[i]] = i;
    }
    return {};
}
```

### 5.4 最长连续序列（Hard）

```cpp
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> st(nums.begin(), nums.end());
    int maxLen = 0;
    
    for (int num : st) {
        // 关键：如果是序列的起点 (num-1 不存在)，才开始计算
        if (st.find(num - 1) == st.end()) { 
            int currentNum = num;
            int currentLen = 1;
            
            while (st.find(currentNum + 1) != st.end()) {
                currentNum++;
                currentLen++;
            }
            maxLen = max(maxLen, currentLen);
        }
    }
    return maxLen;
}
```