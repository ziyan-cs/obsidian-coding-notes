---
study_stage: backlog
---

> [!note] 方法论坐标
> 通用故障定位与动态分析见 [Debugging and Failure Localization (调试与故障定位)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/02-Debugging%20and%20Failure%20Localization%20(调试与故障定位).md) 和 [Memory Safety and Dynamic Analysis (内存安全与动态分析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/03-Memory%20Safety%20and%20Dynamic%20Analysis%20(内存安全与动态分析).md)；本篇保留 C++ 异常安全、GDB 与编译器工具的落地细节。


> [!summary] 核心摘要
>
> RAII 让资源释放沿作用域自动发生，异常安全则要求失败后仍维持不变量；调试先复现、缩小范围、验证假设，再依据证据定位根因。

# Exception Handling (异常处理)

> [!note] 本节重点：异常安全保证、栈展开、noexcept 优化、RAII 与异常

## 三种异常安全保证

| 保证 | 抛出后仍须成立的条件 | 典型实现 |
| --- | --- | --- |
| 基本保证（basic guarantee） | 不泄漏资源，对象仍满足不变量，但内容可以改变 | RAII 管资源，分步更新时维护合法状态 |
| 强保证（strong guarantee） | 操作失败后，对外可观察状态保持不变 | 先在临时对象中完成可能失败的工作，再以不抛异常的提交步骤替换 |
| 不抛出保证（no-throw guarantee） | 操作不会以异常退出 | 析构、资源释放、真正不抛异常的 `swap` |

下面先构造新状态；只有成功后才提交。`std::vector` 在这段代码中负责内存释放，避免手写 `new[]` 时遗漏拷贝构造失败等路径。

```cpp
#include <utility>
#include <vector>

class Numbers {
    std::vector<int> values_;
public:
    void append(int value) {
        auto next = values_;       // 复制失败：原对象不变
        next.push_back(value);     // 分配失败：原对象不变
        values_.swap(next);        // 在此模型中，默认分配器的 swap 不抛出
    }
};
```

这不是所有操作都必须复制一份的建议；实际 `std::vector::push_back` 本身也有条件化的异常保证，应按元素类型、性能需求选择实现。

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
// 若类型也可拷贝，扩容时实现可能选择拷贝来维持异常保证；
// 本例只定义了移动构造，不能据此断言它一定会拷贝。

std::vector<Good> v2;
// 不抛异常的移动构造有利于容器在扩容时保持异常保证；
// 是否更快仍需按类型和负载测量。

// 只有实现确实不会抛异常时才声明 noexcept；不能为了提速而误标。
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

## 异常边界与 RAII

`std::unique_ptr`、容器、锁守卫在栈展开时析构，资源释放不依赖逐层 `catch`。只有能恢复、添加上下文或转换成接口错误的层，才应捕获异常。

```cpp
#include <memory>

void use() {
    auto resource = std::make_unique<Foo>();
    bar();  // 即使抛出，resource 也会析构
}
```

不能机械地给所有 `swap`、移动操作标 `noexcept`：先确认成员操作确实不会抛出；若承诺被违反，运行时会调用 `std::terminate()`。析构函数应设计为不抛异常，尤其要避免栈展开期间再抛出。

## 异常、错误返回与预期失败

| 维度 | 异常 | 显式结果（错误码、`std::expected` 等） |
| --- | --- | --- |
| 传播 | 沿栈展开；调用点不必逐层检查 | 调用者显式处理或向上传递 |
| 控制流 | 不在返回类型中直接体现；适合无法就地处理的失败 | 失败是常规分支时更清晰 |
| 资源 | 栈展开依靠 RAII 清理 | 同样需要 RAII 清理局部资源 |
| 性能 | 正常路径和抛出路径的成本需按实现与负载测量 | 检查与传递也有成本，不能笼统说更快 |

文件不存在可能是正常分支，也可能是致命配置错误；判断依据是**接口契约和调用场景**，不是错误类型的名字。C++23 提供 `std::expected<T, E>`；旧标准可用项目已有的结果类型。不要假定“错误码只有整数、异常一定包含丰富信息”，两者都取决于具体类型设计。

> [!tip]- **工程要点**：异常安全首先靠 RAII 管理资源，而非到处补 `try-catch`。正常的 C++ 异常栈展开会析构已构造的自动对象；但 `std::terminate`、进程异常退出或被跳过的析构路径不能据此保证清理。应在能恢复、转换错误或建立业务边界的层级捕获异常。

---

# Debugging gdb & Sanitizers (调试工具)

> [!note] 本节重点：GDB 核心命令、AddressSanitizer 使用、Segment Fault 调试、Core Dump 分析

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

# AddressSanitizer (ASan)

```bash
g++ -fsanitize=address -g -O1 main.cpp -o main

./main
```

## ASan 检测的问题类型

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

在支持 core dump 的 Linux 环境，可先查看 `ulimit -c` 与系统的 core dump 配置；许多发行版由 systemd-coredump 接管，不一定在当前目录生成 `core.*` 文件。

```bash
ulimit -c unlimited
coredumpctl list
coredumpctl debug <PID>
# 若已经拿到 core 文件：gdb ./main /path/to/core
```

不应把“向 `/proc/sys/kernel/core_pattern` 写值”当作普通用户调试步骤：它更改系统级策略，需要权限，还可能影响其他进程。

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

> [!tip] 工具选择
> ASan/UBSan 能发现**被测试路径触发**的一部分内存与未定义行为问题，不是“跑过即安全”；TSan 通常单独构建运行。GDB 用于追踪崩溃现场和验证假设。具体 Sanitizer 组合与平台支持要在项目中验证。

> [!tip]- **工程要点**：把 Sanitizer 作为可重复的测试配置，而不是“跑过一次就安全”。ASan/UBSan 常适合日常 CI；TSan 通常独立运行；GDB 用于观察真实崩溃现场。具体组合以项目平台、依赖与测试时长为准。

---
