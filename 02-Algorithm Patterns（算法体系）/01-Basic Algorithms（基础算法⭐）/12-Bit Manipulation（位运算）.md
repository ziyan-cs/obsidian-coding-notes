#algorithm #bit-manipulation #binary #bitmask #xor

## ⚡ TL;DR（快速决策）

- 本质上是在**二进制位层面直接操作数字**
- 常见操作：
    - `&`：按位与
    - `|`：按位或
    - `^`：按位异或
    - `~`：按位取反
    - `<<`：左移
    - `>>`：右移
- 看到这些特征，优先想到位运算：
    - 题目和 **奇偶、2 的幂、二进制状态** 有关
    - 要你表示“选 / 不选”“开 / 关”“有 / 无”这类状态
    - 要你做**子集枚举、状态压缩、快速判断某一位**
    - 出现“只出现一次 / 出现两次 / 异或和”这类关键词
- 位运算的核心价值：
    - 快
    - 写法短
    - 很适合表达二元状态

## 🧩 Core Idea（核心本质）

- 计算机里整数本来就是二进制存储
- 位运算就是直接对这些二进制位做操作
- 最常见的理解模型：
    - 一个整数的每一位代表一个状态
    - 第 0 位表示第一个开关
    - 第 1 位表示第二个开关
    - 某一位是 `1` 表示开，`0` 表示关
- 所以位运算特别适合：
    - **状态压缩**
    - **集合表示**
    - **快速判断 / 修改某一位**
    - **利用异或性质做消除 / 配对**
- 最重要的几个基本性质：
    - `x & 1`：判断奇偶
    - `x & (x - 1)`：去掉最低位的一个 `1`
    - `x & -x`：取出最低位的一个 `1`
    - `a ^ a = 0`
    - `a ^ 0 = a`
    - 异或满足交换律和结合律
- 一句话理解：
    - **位运算就是把数字拆成二进制位来思考。**

## 🔧 Usage Patterns（可复用代码模板）

1. 判断奇偶

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 13;
    if (x & 1) cout << "odd\n";
    else cout << "even\n";
    return 0;
}
```

- 最低位是 `1` 表示奇数
- 最低位是 `0` 表示偶数

2. 判断第 k 位是否为 1

```cpp
#include <iostream>
using namespace std;

bool getBit(int x, int k) {
    return (x >> k) & 1;
}

int main() {
    int x = 10;  // 二进制 1010
    cout << getBit(x, 1) << '\n';
    cout << getBit(x, 2) << '\n';
    return 0;
}
```

- 先右移 `k` 位，把目标位移到最低位
- 再与 `1` 做按位与

3. 把第 k 位设为 1 / 设为 0 / 翻转

```cpp
#include <iostream>
using namespace std;

int setBit(int x, int k) {
    return x | (1 << k);
}

int clearBit(int x, int k) {
    return x & ~(1 << k);
}

int toggleBit(int x, int k) {
    return x ^ (1 << k);
}

int main() {
    int x = 10;  // 1010
    cout << setBit(x, 0) << '\n';
    cout << clearBit(x, 1) << '\n';
    cout << toggleBit(x, 2) << '\n';
    return 0;
}
```

- 设为 1：`|`
- 清为 0：`& ~`
- 翻转：`^`

4. 判断是不是 2 的幂

```cpp
#include <iostream>
using namespace std;

bool isPowerOfTwo(int x) {
    return x > 0 && (x & (x - 1)) == 0;
}

int main() {
    cout << isPowerOfTwo(8) << '\n';
    cout << isPowerOfTwo(10) << '\n';
    return 0;
}
```

- 2 的幂在二进制里只有一个 `1`
- `x & (x - 1)` 会把最低位的一个 `1` 去掉

5. 统计二进制中 1 的个数

```cpp
#include <iostream>
using namespace std;

int countBits(int x) {
    int cnt = 0;
    while (x) {
        x &= (x - 1);
        ++cnt;
    }
    return cnt;
}

int main() {
    cout << countBits(13) << '\n';
    return 0;
}
```

- 每次循环去掉一个 `1`
- 比一位一位检查更常见、更经典

6. 只出现一次的数字（异或经典题）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int singleNumber(const vector<int>& nums) {
    int eor = 0;
    for (int v : nums) eor ^= v;
    return eor;
}

int main() {
    vector<int> nums = {2, 1, 4, 2, 1};
    cout << singleNumber(nums) << '\n';
    return 0;
}
```

- 成对出现的数会两两抵消
- 最后剩下只出现一次的数

7. 子集枚举（bitmask 高级高频）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums = {10, 20, 30};
    int n = nums.size();

    for (int mask = 0; mask < (1 << n); ++mask) {
        cout << "{ ";
        for (int i = 0; i < n; ++i) {
            if ((mask >> i) & 1) {
                cout << nums[i] << ' ';
            }
        }
        cout << "}\n";
    }

    return 0;
}
```

- `mask` 的每一位代表一个元素选或不选
- 总共有 `2^n` 个子集
- 状态压缩 DP、搜索、组合枚举里特别常见

8. 枚举一个集合的所有非空子集

```cpp
#include <iostream>
using namespace std;

int main() {
    int mask = 13;  // 1101

    for (int sub = mask; sub; sub = (sub - 1) & mask) {
        cout << sub << '\n';
    }

    return 0;
}
```

- 这是竞赛和进阶题里的经典技巧
- 用来枚举 `mask` 的所有非空子集

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：优先级写错
    - `x >> k & 1` 虽然常能工作，但建议写成 `((x >> k) & 1)` 更稳
    - `(x & 1 == 0)` 是错的，应该写 `(x & 1) == 0`
- 最常见错误 2：把 `^` 当成幂运算
    - 在 C++ 里 `^` 是异或，不是乘方
- 最常见错误 3：移位越界或乱移负数
    - 位移位数不要乱写
    - 对负数右移的细节不要随便依赖
- 最常见错误 4：`1 << k` 溢出
    - 如果 `k` 可能很大，优先写 `1LL << k`
- 最常见错误 5：符号位问题
    - `int` 通常是 32 位有符号数
    - 做高位操作时要小心符号扩展
- 子集枚举常见错误：
    - 循环范围写错
    - 忘了总数是 `1 << n`
    - `n` 太大时，`2^n` 会爆炸，不能硬枚举
- 异或题常见误区：
    - 不理解“成对抵消”的前提，就生搬硬套
    - 题目条件一变，解法可能完全不同

## 🚀 Performance / Tips（性能优化）

- 位运算通常是常数级非常快的操作
    
- 高频复杂度记忆：
    
    - 判断某一位：`O(1)`
    - 设置 / 清除某一位：`O(1)`
    - `x & (x - 1)` 去一个 `1`：单次 `O(1)`
    - 枚举所有子集：`O(2^n)`
- 经验判断：
    
    - `n <= 20` 左右，子集枚举通常还有机会
    - `n >= 25` 往上，纯 `2^n` 往往就危险了
- 如果题目出现：
    
    - 集合状态
    - 开关状态
    - 二进制限制
    - 异或性质
    
    那就要高度警惕位运算 / 状压
    
- 写位运算代码时建议：
    
    - 关键表达式加括号
    - 注释写上二进制含义
    - 不要为了短而牺牲可读性

## 🧪 Common Scenarios（常见使用场景）

- **奇偶判断**：`x & 1`
- **2 的幂判断**：`x & (x - 1)`
- **lowbit 问题**：`x & -x`
- **异或消除**：找唯一数、交换性质、前缀异或
- **状态压缩**：
    - 子集 DP
    - 开关问题
    - 选人 / 选物品状态表示
- **集合表示**：
    - 一个整数表示一个集合
    - 某一位表示某元素是否存在
- **位掩码技巧**：
    - 枚举子集
    - 合并状态
    - 判断交集

## 🆚 Bit Manipulation vs 其他常见方法

- **vs Arithmetic（普通算术）**
    - 位运算更贴近底层
    - 某些场景更快、更自然
- **vs Boolean Array（布尔数组）**
    - 布尔数组更直观
    - 位压缩更省空间、更适合状态表示
- **vs Set / Hash Set**
    - set / 哈希集合适合维护真实集合
    - bitmask 适合元素个数不多、状态固定的小集合

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 13;  // 1101

    cout << "奇偶: " << ((x & 1) ? "odd" : "even") << '\\n';
    cout << "第 2 位是否为 1: " << (((x >> 2) & 1) ? "yes" : "no") << '\\n';
    cout << "去掉最低位一个 1 后: " << (x & (x - 1)) << '\\n';
    cout << "最低位的 1: " << (x & -x) << '\\n';
    cout << "把第 1 位翻转后: " << (x ^ (1 << 1)) << '\\n';

    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **位运算就是：把整数当成二进制状态来操作，从而更快地判断、修改、压缩和枚举信息。**