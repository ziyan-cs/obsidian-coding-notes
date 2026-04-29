#cpp #debugging #engineering-basics #bug-fixing #troubleshooting

## ⚡ TL;DR（快速决策）

- 调试本质是：**定位 bug 的真实原因，而不是盲猜**
- 一看到程序出错，优先按这个顺序想：
    1. 能否稳定复现？
    2. 输入输出是否符合预期？
    3. 哪一步开始错？
    4. 是编译错、运行错、逻辑错，还是边界错？
- 最常见调试手段：
    - 打印日志
    - 单步调试
    - 断点
    - 检查变量值
    - 缩小问题范围
- 会调试比会写代码更重要

## 🧩 Core Idea（核心本质）

- 调试不是“多试几次”，而是系统定位问题
- 核心思路：
    - 先复现
    - 再缩小范围
    - 找到触发条件
    - 最后修复并验证
- 一句话理解：
    - **调试就是把“哪里错了”一步步压缩到最小范围。**

## 🔧 Usage Patterns（可复用代码模板）

1. 打印调试

```cpp
#include <iostream>
using namespace std;

int main() {
    int a = 3, b = 4;
    cerr << "a=" << a << ", b=" << b << '\\n';
    cout << a + b << '\\n';
}
```

1. 断言辅助

```cpp
#include <cassert>
assert(x >= 0);
```

1. 二分定位思路

```
先确认前半段逻辑是否正确，逐步缩小出错范围
```

1. 常见调试问题分类

```
- 编译错误
- 运行时错误
- 死循环
- 数组越界
- 空指针
- 逻辑错误
```

## ⚠️ Pitfalls（高频错误）

- 不复现就乱改代码
- 一次改太多，不知道到底哪个改动起效
- 只看结果，不看中间状态
- 忽略边界输入、极端数据、空输入
- 修完不回归验证，容易引入新 bug

## 🚀 Performance / Tips（性能优化）

- 调试重点不是快写，而是快定位
- 常用经验：
    - 先用最小样例复现
    - 再测边界样例
    - 输出关键变量
    - 善用 IDE / gdb 断点和观察窗口
- 如果 bug 很诡异，优先怀疑：
    - 越界
    - 未初始化
    - 指针失效
    - 整数溢出

## 🧪 Common Scenarios（常见使用场景）

- 编译报错排查
- 链接错误排查
- RE / WA / TLE 排查
- 多文件工程问题定位
- 线上 bug 复现与修复

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 5;
    cerr << "x = " << x << '\\n';
    cout << x * 2 << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **调试基础就是：通过复现、观察和缩小范围，系统地找到 bug 的真正原因。**