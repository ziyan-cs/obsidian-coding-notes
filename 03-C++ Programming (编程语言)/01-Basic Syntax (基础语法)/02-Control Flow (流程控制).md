---
tags:
  - cpp/syntax
status: 🌱
---

> [!important] **核心考点**：条件分支（if/switch）、循环（for/while/do-while）、break/continue 控制流

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

函数设计中的控制流应用详见 → [Functions (函数)](/03-C++%20Programming%20(编程语言)/01-Basic%20Syntax%20(基础语法)/03-Functions%20(函数).md)
