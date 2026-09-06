---
tags:
  - cpp/engineering
status: seed
review_due: 2026-09-12
confidence: 1
verified: NEEDS_VERIFY
---

# Compilation & Linking — 编译与链接

> [!important] **核心考点**：编译的四个阶段、符号解析与重定位、静态链接 vs 动态链接、常见链接错误

## 编译的四个阶段

```text
Compilation Pipeline:

source.cpp  ──→  source.ii  ──→  source.s  ──→  source.o  ──→  a.out
(preprocessing)  (compilation)  (assembly)    (linking)    (executable)
    │                                                                  ▲
    │ g++ -E                        g++ -S      g++ -c       ld       │
    ↓                                                                  │
(expand macros,                                                  ┌─────┴──────┐
 include headers)                                                │  libfoo.a  │
                                                                 │  libbar.so │
                                                                 └────────────┘
```

```bash
# 分步演示
g++ -E main.cpp -o main.i    # 预处理
g++ -S main.i -o main.s       # 编译到汇编
g++ -c main.s -o main.o       # 汇编到目标文件
g++ main.o -o main            # 链接
# 一步到位：
g++ main.cpp -o main
```

## 目标文件的节区（Section）

```text
ELF 目标文件结构：
┌──────────────┐
│ ELF Header   │  文件头（魔数、入口地址、节区表位置）
├──────────────┤
│ .text        │  代码段（只读，机器指令）
├──────────────┤
│ .rodata      │  只读数据（字符串常量、const 变量）
├──────────────┤
│ .data        │  已初始化全局/静态变量
├──────────────┤
│ .bss         │  未初始化全局/静态变量（不占文件空间）
├──────────────┤
│ .symtab      │  符号表（函数名、全局变量名）
├──────────────┤
│ .rel.text    │  重定位表（需要修改的地址）
├──────────────┤
│ .debug       │  调试信息（-g 时生成）
└──────────────┘
```

## 符号解析与重定位

```cpp
// a.cpp
extern int global;       // 引用外部符号
void foo();              // 引用外部符号
int main() {
    foo();
    return global;
}

// b.cpp
int global = 42;         // 定义符号
void foo() { /* ... */ } // 定义符号
```

**链接过程**：
1. **符号解析**：收集所有目标文件的符号表，将引用与定义匹配
2. **重定位**：把符号引用替换为实际地址

**常见链接错误**：

```text
// 未定义引用（undefined reference）
undefined reference to `foo()'
→ 忘记链接 foo 所在的目标文件或库

// 多重定义（multiple definition）
multiple definition of `global'
→ 在头文件中定义了全局变量（应在 .h 中 extern，.cpp 中定义）

// 解决：头文件中应只声明，不定义
// ❌ common.h: int counter = 0;
// ✅ common.h: extern int counter;
// ✅ common.cpp: int counter = 0;
```

## 静态链接 vs 动态链接

| | 静态链接 (.a) | 动态链接 (.so / .dll) |
|--|-------------|-------------------|
| 链接时机 | 编译时 | 运行时（加载时链接）|
| 可执行文件大小 | 大（包含库代码） | 小（只记录依赖）|
| 内存占用 | 不同进程各自一份 | **共享**同一份 .so |
| 更新库 | 需重新链接 | 替换 .so 即可 |
| 部署 | 无外部依赖 | 需确保 .so 存在 |
| 启动速度 | 取决于二进制大小、加载器与系统缓存 | 取决于依赖数量、加载器与系统缓存 |
| 性能 | 取决于编译优化、调用边界与实际工作负载 | 取决于符号解析、调用边界与实际工作负载 |

```bash
# 编译静态库
g++ -c lib.cpp -o lib.o
ar rcs libfoo.a lib.o
g++ main.cpp -L. -lfoo -o main

# 编译共享库
g++ -fPIC -shared lib.cpp -o libfoo.so
g++ main.cpp -L. -lfoo -o main
# 运行时需要 LD_LIBRARY_PATH 或在系统路径下
```

## 动态链接的细节：PLT & GOT

```text
调用共享库函数时的跳转流程：
main() 调用 foo():
  → call foo@PLT      (PLT: 过程链接表)
    → jmp *(foo@GOT)  (GOT: 全局偏移表)
      → 首次: 跳转到动态链接器，解析 foo 地址，更新 GOT
      → 之后: 直接跳转到 foo
```

**延迟绑定（Lazy Binding）**：函数地址只在第一次调用时才解析，提高启动速度。

## 工程最佳实践

```cpp
// ✅ 头文件守卫
#pragma once  // 或 #ifndef...#define...#endif

// ✅ 尽量减少头文件依赖（前向声明代替 #include）
class Foo;  // 前向声明，在头文件中只需要指针/引用时使用

// ✅ 内联函数放在头文件中
inline int square(int x) { return x * x; }

// ❌ 不要在头文件中定义全局变量
// ❌ 不要在两个 .cpp 中定义同名全局函数
// ❌ 不要在头文件中 using namespace std;
```

> **面试重点**：**声明 vs 定义**的区别——声明引入名字，定义提供实体（对对象而言通常也提供存储）。头文件通常放声明；需要放在头文件的 inline 函数、模板、`inline` 变量等是例外。避免在普通头文件定义具有外部链接的全局变量。

## 30 秒回答

编译把源文件分别变成目标文件，链接器再解析跨文件符号并重定位地址。`undefined reference` 先查“声明有了但定义/库没有参与链接”，`multiple definition` 先查“同一外部符号被定义多次”。静态/动态链接是部署、更新、隔离与启动成本的权衡，不能只用“谁更快”概括。

---

头文件与源文件组织详见 → [Header & Source Organization (头文件与源文件组织)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/02-Header%20&%20Source%20Organization%20(头文件与源文件组织).md)
