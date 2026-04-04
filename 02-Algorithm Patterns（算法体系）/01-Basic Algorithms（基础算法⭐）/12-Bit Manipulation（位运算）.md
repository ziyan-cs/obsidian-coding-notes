#algorithm #bit #bitmask #xor

## 核心

- 本质是：**直接操作二进制位**
- 必会 5 个：
    - `&`
    - `|`
    - `^`
    - `~`
    - `<< >>`
- 什么时候用：
	- 状态压缩
	- 奇偶
	- 去重
	- 异或
	- lowbit

## 高频技巧

### 1. 判断奇偶

```cpp
bool odd = x & 1;
```

### 2. 取第 k 位

```cpp
int bit = (x >> k) & 1;
```

### 3. 设置第 k 位为 1

```cpp
x |= (1 << k);
```

### 4. 清除第 k 位

```cpp
x &= ~(1 << k);
```

### 5. 翻转第 k 位

```cpp
x ^= (1 << k);
```

### 6. lowbit

```cpp
int lowbit(int x) {
    return x & -x;
}
```

## 高频例子

### singleNumber

```cpp
int singleNumber(vector<int>& nums) {
    int  = 0;
    for (int x : nums) ans ^= x;
    return ans;
}
```

### singleNumber（进阶）
 ```cpp
 vector<int> singleNumber(vector<int>& nums) {
	int eor = 0;
	for (int x : nums) eor ^= x;  // 得到 a ^ b
	
	int onlyOne = eor & (-eor);
	
	int a = 0, b = 0; 
	for (int x : nums) {
		if (x & onlyOne != 0) a ^= x  // 该位为1的一组，异或出a 
		else b ^= x;  // 该位为0的一组，异或出b 
	} 
	return {a, b}; 
}
 ```
### 枚举子集状态

```cpp
for (int mask = 0; mask < (1 << n); ++mask) {
    for (int i = 0; i < n; ++i) {
        if ((mask >> i) & 1) {
            // 选了第 i 个元素
        }
    }
}
```

## 高频坑点

- 避免`1 << k` 溢出，必要时写 `1LL << k`
- 负数右移问题要考虑清楚
- 要理解异或性质：
    - `a ^ a = 0`
    - `a ^ 0 = a`