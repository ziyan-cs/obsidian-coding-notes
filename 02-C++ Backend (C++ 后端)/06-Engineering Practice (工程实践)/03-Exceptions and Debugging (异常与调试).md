---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 03-Exceptions and Debugging (异常与调试)

> [!abstract] 阅读定位
>
> 本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## 30 秒回答

**核心结论**：阅读定位  本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。


## Exception Handling (异常处理)

> [!abstract] 核心考点：异常安全保证、栈展开、noexcept 优化、RAII 与异常

## 三种异常安全保证

```cpp
// 1. 基本保证：抛出异常后，对象处于合法状态
// 2. 强保证：抛出异常后，状态回滚（类似事务）
// 3. 不抛出：绝不抛出异常（noexcept）

class Vector {
    int* data_;
    size_t size_;
public:
    // 强保证：使用 copy-and-swap
    void push_back(int val) {
        auto new_data = new int[size_ + 1];
        std::copy(data_, data_ + size_, new_data);
        new_data[size_] = val;
        std::swap(data_, new_data);  // swap 不抛出
        delete[] new_data;
        ++size_;
    }
};
```

## noexcept

```cpp
// noexcept 有两种作用：
// 1. 承诺函数不抛异常
// 2. 告诉编译器进行优化（比如 vector 的 move 操作）

void safe_func() noexcept;  // 抛出异常会调用 std::terminate

// noexcept 是函数类型的一部分（C++17 起）
void (*f1)() noexcept;      // f1 只能指向 noexcept 函数
void (*f2)();               // f2 可指向任何函数（包括 noexcept）

// 条件性 noexcept
void swap(T& a, T& b) noexcept(std::is_nothrow_swappable_v<T>);
```

**Move 构造与 noexcept 的关系**：

```cpp
struct Bad { Bad(Bad&&) { /* 可能抛异常 */ } };
struct Good { Good(Good&&) noexcept { /* ... */ } };

std::vector<Bad> v1;
// push_back 时，vector 扩容会用拷贝而非移动
// （因为移动可能抛异常，无法保证强异常安全）

std::vector<Good> v2;
// push_back 时，vector 扩容会使用移动操作（快得多！）

// 所以：移动构造函数必须标记 noexcept！
```

## 栈展开（Stack Unwinding）

```cpp
struct Cleanup {
    ~Cleanup() { std::cout << "cleanup"; }
};

void func() {
    Cleanup c;  // 在栈上创建
    throw std::runtime_error("error");  // 抛出异常
    // Cleanup::~Cleanup() 在栈展开时被调用
}

int main() {
    try {
        func();
    } catch (const std::exception& e) {
        // c 已经被正确析构了
    }
}
```

**栈展开的过程**：
1. 从 `throw` 处开始，逐层向上查找 `catch` 子句
2. 每退出一层，该层栈上所有对象的析构函数被调用
3. 找到匹配的 `catch` 后，进入异常处理

## 异常安全编程指南

```cpp
// ✅ 使用 RAII 管理资源（异常安全的核心）
std::unique_ptr<Foo> ptr(new Foo());
// 不需要 try-catch，析构函数自动释放

// ✅ 使用智能指针而不是裸 new
std::unique_ptr<int[]> buffer(new int[100]);

// ❌ 危险的裸 new
void bad() {
    Foo* p = new Foo();
    bar();     // 如果 bar() 抛出异常，p 泄漏！
    delete p;
}

// ✅ 异常中立：让异常继续向上传播
void wrapper() {
    // 不需要处理时，不要 catch
}

// ✅ 析构函数/swap/移动构造/移动赋值 应标记 noexcept
~Foo() noexcept;
void swap(Foo&) noexcept;
```

## 异常 vs 错误码

| | 异常 | 错误码 |
|--|------|--------|
| 传播方式 | 自动展开栈 | 手动传递和检查 |
| 性能 | 正常路径无开销，异常路径慢 | 每条路径都要检查 |
| 信息量 | 多（类型 + what()） | 少（一个整数）|
| 被忽略的可能 | ❌ 无法忽略 | ✅ 可能被忘记检查 |
| 适用场景 | **致命/意外错误** | **频繁发生/性能关键的预期错误** |

```cpp
// ✅ 异常适用：意料之外的错误
int divide(int a, int b) {
    if (b == 0) throw std::runtime_error("division by zero");
    return a / b;
}

// ✅ 错误码适用：预期中的失败
std::error_code ec;
auto result = read_file("config.txt", ec);
if (ec) { /* 处理不存在等预期情况 */ }
```

> [!tip]- **工程要点**：编写异常安全代码的核心不是 try-catch，而是 **RAII**。资源在构造时获取，析构时释放——析构函数在栈展开时一定会被调用。现代 C++ 中极少需要写 try-catch，除非要做错误转换或日志记录。

---

调试与异常定位详见 → [Debugging gdb & Sanitizers (调试工具)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/05-Debugging%20gdb%20&%20Sanitizers%20(调试工具)%20⭐.md)

---

## Debugging gdb & Sanitizers (调试工具)

> [!abstract] 核心考点：GDB 核心命令、AddressSanitizer 使用、Segment Fault 调试、Core Dump 分析

## GDB 核心命令

```bash
g++ -g -O0 main.cpp -o main

gdb ./main
gdb ./main core        # 分析 core dump
gdb ./main 1234        # 附加到进程 1234
```

### 常用命令速查

| 命令 | 缩写 | 作用 |
|------|------|------|
| `break main` | `b main` | 在 main 函数设断点 |
| `break file.cpp:42` | `b file.cpp:42` | 在文件某行设断点 |
| `run` | `r` | 运行程序 |
| `next` | `n` | 单步跳过（不进入函数）|
| `step` | `s` | 单步进入 |
| `finish` | `fin` | 运行到当前函数返回 |
| `continue` | `c` | 继续运行到下一个断点 |
| `print var` | `p var` | 打印变量值 |
| `backtrace` | `bt` | 查看调用栈 |
| `frame N` | `f N` | 切换到第 N 帧 |
| `info locals` | `i lo` | 查看当前帧局部变量 |
| `list` | `l` | 显示源代码 |
| `watch expr` | — | 监视表达式变化 |
| `display expr` | — | 每次停顿时自动打印 |

```gdb
gdb> b main               # 断点
gdb> r arg1 arg2          # 带参数运行
gdb> bt                   # 崩溃时先看调用栈
gdb> f 3                  # 切换到怀疑的帧
gdb> p variable           # 查看变量
gdb> l                    # 看附近源码
```

## AddressSanitizer (ASan)

```bash
g++ -fsanitize=address -g -O1 main.cpp -o main

./main
```

### ASan 检测的问题类型

```cpp
int* p = new int[10];

p[10] = 42;     // ✅ ASan 检测：heap-buffer-overflow
delete[] p;
*p = 1;         // ✅ ASan 检测：heap-use-after-free

int* q = (int*)malloc(4);
free(q);
free(q);        // ✅ ASan 检测：double-free

int a;
int* r = &a;
delete r;       // ✅ ASan 检测：delete on stack variable
```

## 其他 Sanitizers

```bash
g++ -fsanitize=undefined -g main.cpp -o main

g++ -fsanitize=thread -g -O1 main.cpp -o main

g++ -fsanitize=leak -g main.cpp -o main

g++ -fsanitize=address,undefined -g -O1 main.cpp -o main
```

```cpp
// UBSan 检测的典型 UB：
int x = INT_MAX;
x + 1;            // signed overflow
int* p = nullptr;
*p = 42;          // null dereference
int a = 0;
int b = 1 / a;    // division by zero
```

## Core Dump 分析

```bash
ulimit -c unlimited
echo "core.%p" > /proc/sys/kernel/core_pattern

gdb ./main core.1234

```

## Valgrind 基础

```bash
valgrind --tool=memcheck ./main

valgrind --tool=callgrind ./main

```

| 工具 | 用途 | 速度 |
|------|------|------|
| **AddressSanitizer** | 内存错误检测 | 开销因程序/平台而异，适合日常测试 |
| **Valgrind Memcheck** | 指令级内存检查 | 通常开销更高，适合针对性复现 |
| **ThreadSanitizer** | 数据竞争检测 | 开销因程序/平台而异，需独立测试配置 |
| **UBSan** | 部分未定义行为检测 | 开销因启用检查项而异 |

> [!tip]- **工程要点**：现代 C++ 调试首选 **AddressSanitizer**（快、准）。在 CI 中应开启 ASan + UBSan。与 GDB 配合使用：ASan 告诉你问题类型和位置，GDB 帮你分析上下文。

> [!tip]- **工程要点**：把 Sanitizer 作为可重复的测试配置，而不是“跑过一次就安全”。ASan/UBSan 常适合日常 CI；TSan 通常独立运行；GDB 用于观察真实崩溃现场。具体组合以项目平台、依赖与测试时长为准。

---

性能分析工具详见 → [Performance Profiling perf & valgrind (性能分析)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/06-Performance%20Profiling%20perf%20&%20valgrind%20(性能分析)%20⭐.md)

## 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
03-Exceptions and Debugging (异常与调试)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
