---
tags:
  - cpp
  - syntax
---

> **核心考点**：条件分支（if/switch）、循环（for/while/do-while）、break/continue 控制流

```cpp
// if-else
if (x > 0) { ... }
else if (x < 0) { ... }
else { ... }

// C++17：if 初始化语句
if (auto it = map.find(key); it != map.end()) {
    use(it->second);
}

// switch（只能用整型/枚举，注意 fallthrough）
switch (c) {
    case 'a': case 'e': case 'i':
        std::cout << "vowel\n"; break;
    case 'b': case 'c':
        std::cout << "consonant\n"; break;
    default:
        break;
}

// for
for (int i = 0; i < n; i++) { ... }
for (auto& x : container) { ... }   // 范围 for（C++11）

// while / do-while
while (cond) { ... }
do { ... } while (cond);

// break / continue / goto（goto 只在状态机或跳出多层循环时考虑）
```

---

## 关联笔记

- [Variables, Types & Operators (变量、类型与运算符)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/01-Variables,%20Types%20&%20Operators%20(变量、类型与运算符).md)
- [Functions(函数)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/03-Functions(函数).md)
- [Array & String(数组与字符串)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/04-Array%20&%20String(数组与字符串).md)
- [IO Basic(标准输入输出)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/05-IO%20Basic(标准输入输出).md)
- [Const, Typedef & Enum (类型系统基础)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)
