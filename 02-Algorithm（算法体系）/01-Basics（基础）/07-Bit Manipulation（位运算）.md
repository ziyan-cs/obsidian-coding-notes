#algorithm #bit #bitmask #xor

# 0. 核心

- **直接操作二进制位**
- 必会的 5 个：
    - `&`
    - `|`
    - `^`
    - `~`
    - `<< >>`
- 常见应用场景：
	- 状态压缩
	- 奇偶
	- 去重
	- lowbit

# 1. 高频技巧

### 1.1 判断奇偶

```cpp
bool odd = x & 1;
```

### 1.2 取第 k 位

```cpp
int bit = (x >> k) & 1;
```

### 1.3 设置第 k 位为 1

```cpp
x |= (1 << k);
```

### 1.4 清除第 k 位

```cpp
x &= ~(1 << k);
```

### 1.5 翻转第 k 位

```cpp
x ^= (1 << k);
```

### 1.6 清除最后一个 1

```cpp
x &= x - 1;
```


### 1.7 lowbit

```cpp
int lowbit(int x) {
    return x & -x;
}
```

### 1.8 判断是否为 2 的幂

```cpp
bool isPowerOfTwo(int x) {
    return x > 0 && (x & (x - 1)) == 0;
}
```

### 1.9 交换两个数（异或）

```cpp
a ^= b;
b ^= a;
a ^= b;
```

# 2. 异或经典案例

### 2.1 singleNumber 基础（只出现一次的数字 I）

```cpp
// 题目：除了一个元素只出现一次，其余都出现两次，找出只出现一次的元素
// 1. a ^ a = 0
// 2. a ^ 0 = a
// 3. 异或满足交换律、结合律

int singleNumber(vector<int>& nums) {
    int ans = 0;
    for (int x : nums) ans ^= x;
    return ans;
}
```
**（ LeetCode 136 ）**

### 2.2 singleNumber 拓展（只出现一次的数字 II）

```cpp
// 题目：除了一个元素只出现一次，其余都出现三次，找出只出现一次的元素
// 1. 统计每一位上1出现的次数，对3取余
// 2. 余数为1，说明只出现一次的数在该位为1；余数为0，说明该位为0

int singleNumber(vector<int>& nums) {
	int ans = 0;
	for (int i = 0; i < 32; ++i) {
		int cnt = 0;
		for (int x : nums) {
			cnt += (x >> i) & 1;
		}
		if (cnt % 3 !=0){
			ans |= (1 << i);
		}
		return ans;
	}
}
```
** （LeetCode 137 ）**

### 2.3 singleNumber 进阶（只出现一次的数字 III）
 ```cpp
// 题目：除了两个元素只出现一次，其余都出现两次，找出只出现一次的元素
// 1. 全员异或得到 a ^ b（a≠b，所以结果不为0，至少有一位为1）
// 2. 用 lowbit 提取最后一个为1的位，作为分组依据
// 3. 按该位是否为1，将数组分为两组，两组分别异或，得到a和b
 
 vector<int> singleNumber(vector<int>& nums) {
	int eor = 0;
	for (int x : nums) eor ^= x;
	
	long long onlyOne = eor & (-eor);
	
	int a = 0, b = 0; 
	for (int x : nums) {
		if (x & onlyOne != 0) a ^= x;
		else b ^= x;
	} 
	return {a, b}; 
}
 ```
**（ LeetCode 260 ）**

# 3. 技巧应用

### 3.1 汉明重量（位 1 的个数）

```cpp
// 题目：计算输入整数的二进制中1的个数
// 时间复杂度：O(k)，k为二进制中1的个数（远优于O(32)的遍历法）

int hammingWeight(uint32_t n) {
    int cnt = 0;
    while (n != 0) {
        n &= n - 1;
        cnt++;
    }
    return cnt;
}
```
**（ LeetCode 191 ）**

### 3.2 枚举子集状态（状态压缩）

```cpp
// 状态压缩：枚举n个元素的所有子集（共2^n个）
// 时间复杂度：O(N * 2^N)，仅适用于n≤20的场景

for (int mask = 0; mask < (1 << n); ++mask) {
	int sub = mask;
	do {
		sub = (sub - 1) & mask;
	} while (sub != mask);
}

```
