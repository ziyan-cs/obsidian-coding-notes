
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