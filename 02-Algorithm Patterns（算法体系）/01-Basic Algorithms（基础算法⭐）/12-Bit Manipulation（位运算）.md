#algorithm #bit #bitmask #xor

## 核心

- 本质是直接操作二进制位
- 必会 5 个：
    - `&`
    - `|`
    - `^`
    - `~`
    - `<< >>`

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

### 只出现一次的数字

```cpp
int singleNumber(vector<int>& nums) {
    int ans = 0;
    for (int x : nums) ans ^= x;
    return ans;
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

- `1 << k` 溢出，必要时写 `1LL << k`
- 负数右移问题没考虑清楚
- 不理解异或性质：
    - `a ^ a = 0`
    - `a ^ 0 = a`

## 只记这个

- 状态压缩、奇偶、去重异或、lowbit 都离不开位运算