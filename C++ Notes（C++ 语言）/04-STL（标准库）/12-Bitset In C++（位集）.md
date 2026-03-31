#stl #bitset #cpp #binary #bit-operation

## ⚡ TL;DR（快速决策）

- `bitset` 本质是：**定长的二进制位容器**
- 一看到这些需求，要优先想到 `bitset`：
    - 需要高效存很多 0/1 状态
    - 需要快速做按位与、或、异或、翻转
    - 需要统计有多少位是 1
    - 需要位集判交、状态标记
- `bitset<N>` 的长度在编译期就固定
- 如果你要处理大量布尔状态且长度固定，优先先想 `bitset`

## 🧩 Core Idea（核心本质）

- `bitset` 可以理解成“加强版二进制数组”
- 每一位只存 0 或 1
- 和普通整数位运算相比：
    - `bitset` 可读性更强
    - 更适合处理很多位的状态
- 一句话理解：
    - **`bitset` = 固定长度的位集合。**

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础定义

```cpp
#include <bitset>
#include <iostream>
using namespace std;

int main() {
    bitset<8> bs("10110010");
    cout << bs << '\\n';
    return 0;
}
```

1. 访问和修改某一位

```cpp
bitset<8> bs;
bs.set(1);
bs.set(3);
bs.reset(1);
bs.flip(3);
cout << bs.test(3) << '\\n';
```

1. 统计与判断

```cpp
bitset<8> bs("10110010");
cout << bs.count() << '\\n';
cout << bs.any() << '\\n';
cout << bs.none() << '\\n';
```

1. 位运算

```cpp
bitset<8> a("1100");
bitset<8> b("1010");
cout << (a & b) << '\\n';
cout << (a | b) << '\\n';
cout << (a ^ b) << '\\n';
```

## ⚠️ Pitfalls（高频错误）

- `bitset<N>` 的 `N` 必须是编译期常量
- 下标范围不能越界
- `bitset` 长度固定，不像 `vector<bool>` 那样动态变化
- 字符串初始化时要注意高位和低位显示顺序

## 🚀 Performance / Tips（性能优化）

- 位操作通常非常快
- `count()`、按位与、按位或在很多题里都很实用
- 长度固定时，`bitset` 往往比手写布尔数组更简洁
- 如果长度需要运行时决定，就不能直接用 `bitset<N>`

## 🧪 Common Scenarios（常见使用场景）

- 标记访问状态
- 位集交并运算
- 固定长度二进制模拟
- 一些优化 DP / 状态压缩题

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bitset>
#include <iostream>
using namespace std;

int main() {
    bitset<8> bs("10101010");
    bs.set(0);
    bs.flip(1);
    cout << bs << '\\n';
    cout << bs.count() << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`bitset` 就是：一个长度固定、专门高效管理二进制位状态的容器。**