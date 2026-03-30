#algorithm #dp #digit-dp #number-theory #dfs

## ⚡ TL;DR（快速决策）

- 数位 DP 本质是：**按数字的每一位做限制搜索与记忆化**
- 一看到这些特征，要优先想到数位 DP：
    - 统计某个区间 `[0, n]` 或 `[l, r]` 内满足条件的数有多少
    - 条件和数字每一位有关
    - 不能暴力枚举所有数字
    - 常见条件：不含某些数字、各位和限制、相邻位关系
- 常见套路：
    - `solve(r) - solve(l - 1)`
    - `dfs(pos, state, tight, started)`
- 如果题目是“统计区间中满足数位性质的个数”，高概率就是数位 DP

## 🧩 Core Idea（核心本质）

- 把一个上界 `n` 拆成十进制数位数组
- 从高位到低位依次决定当前位填什么
- `tight` 表示当前是否仍受上界约束
- `started` 表示前面是否已经开始构造有效数字
- 一句话理解：
    - **逐位构造数字，并在构造过程中记忆状态。**
- 通常答案写成：
    - `count(0..r) - count(0..l-1)`

## 🔧 Usage Patterns（可复用代码模板）

1. 数位 DP 经典框架

```cpp
#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

long long memo[20][2];
vector<int> digits;

long long dfs(int pos, bool tight) {
    if (pos == (int)digits.size()) return 1;
    if (!tight && memo[pos][0] != -1) return memo[pos][0];

    int up = tight ? digits[pos] : 9;
    long long res = 0;
    for (int d = 0; d <= up; ++d) {
        res += dfs(pos + 1, tight && (d == up));
    }

    if (!tight) memo[pos][0] = res;
    return res;
}
```

1. `solve(n)` 思路

```cpp
long long solve(long long n) {
    if (n < 0) return 0;
    digits.clear();
    string s = to_string(n);
    for (char c : s) digits.push_back(c - '0');
    memset(memo, -1, sizeof(memo));
    return dfs(0, true);
}
```

1. 区间答案

```cpp
answer = solve(r) - solve(l - 1);
```

## ⚠️ Pitfalls（高频错误）

- 忘记写成 `solve(r) - solve(l - 1)`
- `tight` 理解不清，导致上界控制错
- 前导零 `started` 处理混乱
- 记忆化通常只在 `tight = false` 时复用
- 状态里若还有“前一位”“数位和”等信息，要一起记进 memo

## 🚀 Performance / Tips（性能优化）

- 数位 DP 状态数通常不大，关键是状态设计
- 做题顺序建议：
    - 先想 `dfs` 参数要带哪些状态
    - 再想什么时候能记忆化
    - 最后再补区间答案
- 对初学者来说，数位 DP 难点几乎都在状态设计，不在代码量

## 🧪 Common Scenarios（常见使用场景）

- 统计不含 4 / 62 的数
- 统计各位和满足条件的数
- 统计相邻位不相等的数
- 统计区间中特定数位性质的数量

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

long long memo[20];
vector<int> digits;

long long dfs(int pos, bool tight) {
    if (pos == (int)digits.size()) return 1;
    if (!tight && memo[pos] != -1) return memo[pos];

    int up = tight ? digits[pos] : 9;
    long long res = 0;
    for (int d = 0; d <= up; ++d) {
        res += dfs(pos + 1, tight && (d == up));
    }

    if (!tight) memo[pos] = res;
    return res;
}

int main() {
    string s = "123";
    for (char c : s) digits.push_back(c - '0');
    memset(memo, -1, sizeof(memo));
    cout << dfs(0, true) << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **数位 DP 就是：在数字的每一位上做受限搜索，并用记忆化统计满足条件的数。**