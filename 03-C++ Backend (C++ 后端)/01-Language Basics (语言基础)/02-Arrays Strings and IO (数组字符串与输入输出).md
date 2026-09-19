---
status: stable
confidence: high
content_verified: 2026-09-19
previous_review_due: 2026-09-08
---

> [!abstract] 学习目标
> 能根据长度、所有权和读写需求选择数组、`vector`、`string` 或视图；能正确处理一行输入及文件打开失败。

# 序列的长度、所有权与边界

C 数组 `int raw[3]{1, 2, 3};` 自带三个元素，但传给接收 `int*` 的函数时会转换为首元素指针，长度信息丢失。`std::array<T, N>` 固定大小且持有元素，`std::vector<T>` 动态扩容且持有元素；`std::span<T>`（C++20）是带长度的非拥有视图。视图不延长底层存储的生命周期。

```cpp
#include <array>
#include <vector>
#include <iostream>

int main() {
    std::array<int, 3> fixed{1, 2, 3};
    std::vector<int> dynamic{1, 2, 3};
    dynamic.reserve(8);         // 预留容量，不增加 size()
    dynamic.push_back(4);
    std::cout << fixed.at(1) << ' ' << dynamic.size() << '\n';
}
```

`operator[]` 要求索引有效；`at()` 越界抛 `std::out_of_range`。`reserve` 可能使所有指向 `vector` 元素的迭代器、指针、引用失效；`shrink_to_fit` 只是请求，不能依赖它一定缩容。函数接口应优先保留长度信息，不要把裸指针和“可能不一致”的长度拆开传。

# 字符串：文本不是字节缓冲区的同义词

C 风格字符串以 `'\0'` 终止，`strlen` 需要扫描至终止符；若目标缓冲区不足，`strcpy` 等函数可能越界。普通文本优先用 `std::string`，它管理长度和存储；但按字节计数的 `size()` 不等于 Unicode 字符数。

```cpp
#include <string>
#include <string_view>
#include <iostream>

int main() {
    std::string text{"alpha,beta"};
    const auto comma = text.find(',');
    if (comma != std::string::npos) {
        std::string_view first{text.data(), comma}; // 借用 text 的存储
        std::cout << first << '\n';
    }
}
```

`std::string_view` 不拥有数据，也不保证 `data()` 指向的片段以 NUL 结尾；不能把任意 `view.data()` 交给期待 C 字符串的接口。修改或销毁底层 `string` 后，先前保存的视图可能悬空。需要独立持有则复制成 `std::string`。调用 `c_str()` 获得的指针同样只在字符串未失效的期间有效。

# 输入输出：把失败作为正常分支

`operator>>` 读取 token，遇空白停止；`std::getline` 读取一行。先用 `>>` 再用 `getline` 时，残余换行符会让后者读取空行。下面直接按行读并解析，避免混用。

```cpp
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::ifstream in{"input.txt"};
    if (!in) {
        std::cerr << "cannot open input.txt\n";
        return 1;
    }
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream fields{line};
        int id{};
        std::string name;
        if (!(fields >> id >> name)) {
            std::cerr << "invalid row: " << line << '\n';
            continue;
        }
        std::cout << id << ": " << name << '\n';
    }
    if (in.bad()) {
        std::cerr << "read error\n";
        return 1;
    }
}
```

文件流析构会关闭文件，但写入错误可能要到 `flush()` 或 `close()` 才暴露；关键输出应显式检查流状态。`std::cerr` 是与标准错误关联的流，不应宣称它在所有环境中“立即输出”或绝对不缓冲。`std::endl` 会写换行并刷新；只需换行时用 `'\n'`。

## 动手验证

1. 先 `reserve(8)` 后检查 `size()` 与 `capacity()`；再 `push_back`，说明二者差异。
2. 构造 `std::string_view{text.data(), 5}` 并输出；解释为什么不能直接传给 `strlen`。
3. 用正常行、空行、非法 ID、文件不存在四种输入运行文件示例，记录各分支行为。

> [!info]- 延伸阅读
> - [01-Sequence Containers (顺序容器)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/01-Sequence%20Containers%20(顺序容器).md)
> - 规范依据：[string_view](https://eel.is/c++draft/string.view)、[文件流](https://eel.is/c++draft/fstream)
