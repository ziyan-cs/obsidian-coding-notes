---
study_stage: learn
review_due: 2026-10-10
tags: [cs/toolchain, learning/foundation]
---

> [!abstract] 阅读方式
> 本文沿一条真实路径展开：C++ 源文件如何变成目标文件，链接器如何解析符号和重定位，内核与动态加载器如何把 ELF 变成进程，最终怎样进入 `main`。CPU 内部流水线只作为执行背景，不与构建流程混写。

> [!summary] 核心摘要
>
> 编译负责把单个翻译单元变成可重定位目标文件；链接负责跨文件符号解析、节布局和重定位；加载负责建立进程地址空间、装入解释器与共享库并转移控制。排错时先判断问题属于预处理、编译、链接、加载还是运行时，能显著缩小搜索范围。

# 从源文件到进程

```text
headers + source
       │ preprocessing
       ▼
translation unit
       │ compile + optimize
       ▼
assembly / machine IR
       │ assemble
       ▼
relocatable object (.o) ──┐
static archives (.a) ─────┼─ link ─► executable / shared object
shared-library metadata ──┘                    │
                                              │ execve
                                              ▼
                              kernel + dynamic loader
                                              │
                                              ▼
                               process image → _start → main
```

编译器驱动（如 `g++`）会协调预处理器、编译器、汇编器和链接器。日常应优先调用驱动而不是直接调用 `ld`，因为驱动会加入正确的启动文件、运行库和平台参数。

## 五个错误边界

| 阶段 | 典型问题 | 常见诊断关键词 |
| --- | --- | --- |
| 预处理 | 头文件路径、宏展开、条件编译 | `file not found`、错误宏分支 |
| 编译 | 类型、语法、模板实例化 | `no matching function`、类型错误 |
| 链接 | 缺少定义、多重定义、库顺序 | `undefined reference`、`multiple definition` |
| 加载 | 找不到动态库、ABI 不兼容、权限 | `cannot open shared object`、`Exec format error` |
| 运行 | 非法内存、异常、数据竞争 | signal、sanitizer、崩溃栈 |

# 预处理、编译与汇编

## 翻译单元

一个 `.cpp` 文件经过 `#include` 展开和条件编译后形成一个**翻译单元（translation unit）**。头文件不是独立编译后再“粘进来”，而是文本包含；这解释了 include guard、`#pragma once`、ODR 和编译时间问题。

```bash
g++ -std=c++20 -E main.cpp -o main.ii  # 只预处理
g++ -std=c++20 -S main.cpp -o main.s   # 生成汇编
g++ -std=c++20 -c main.cpp -o main.o   # 生成可重定位目标文件
g++ main.o util.o -o app                # 最终链接
```

编译器前端完成词法、语法、语义与模板处理；中端执行与语言规则一致的优化；后端面向目标 ISA 选择指令、分配寄存器并生成机器代码。优化级别不是简单“越高越快”：`-O2` 可能改变布局、内联和调试体验，但仍必须保持抽象机允许观察到的行为。

## 汇编器输出仍不是可运行程序

`.o` 包含机器指令和数据，但其中许多地址尚未确定。对外部函数的调用、全局变量地址或跨节引用会留下 relocation entry，等待链接器修正。

```bash
file main.o
readelf -h -S -s -r main.o
objdump -drC main.o
```

`-C` 可反解 C++ name mangling，`-r` 将 relocation 与反汇编并列展示。

# ELF、节、段与符号

## Section 与 Segment 解决不同问题

- **Section（节）**主要服务编译和链接，例如 `.text`、`.rodata`、`.data`、`.bss`、symbol table 与 relocation section。
- **Segment（段）**主要服务加载；program header 描述哪些文件区域映射为可读、可写或可执行的内存区域。

不要把“代码段/数据段”口语混用成完全相同的概念。链接器把多个输入 section 组织成输出 section，随后按 linker script 映射到可加载 segment。

| 常见 section | 内容 | 关键边界 |
| --- | --- | --- |
| `.text` | 机器指令 | 通常映射为可读、可执行 |
| `.rodata` | 只读常量 | 常与代码邻近，但权限可独立 |
| `.data` | 已初始化的可写静态存储对象 | 文件中保存初值 |
| `.bss` | 零初始化/未显式初始化对象 | ELF 常以 `SHT_NOBITS` 表示，文件不保存同等大小的零字节 |
| `.symtab` / `.dynsym` | 链接或动态加载所需的符号信息 | strip 后用途和保留范围不同 |
| relocation sections | 待修正位置、符号与类型 | 目标架构相关 |

## 符号解析和 ODR

链接器必须为引用找到合适定义。C++ 还涉及 name mangling、模板实例化、inline、weak/COMDAT 与 One Definition Rule；“头文件里定义函数一定多重定义”或“inline 只是请求内联优化”都不完整。

```cpp
// api.hpp
int transform(int);          // declaration

// api.cpp
int transform(int x) {       // definition
    return x * 2;
}
```

若只编译 `main.cpp` 而漏链 `api.o`，编译可成功但链接会报告 undefined reference。

# 静态链接、动态链接与位置无关代码

## Static archive 不等于“自动带入全部代码”

`.a` 是目标文件归档。链接器通常只从归档中抽取当前未解析符号需要的成员，因此库在命令行中的顺序可能影响 GNU ld 的解析结果。

```bash
ar rcs libcalc.a calc.o
g++ main.o -L. -lcalc -o app
```

静态链接减少运行时共享库依赖，但可能让多个不同可执行文件各自包含同一库代码。由同一个可执行文件启动的多个进程，其只读文件映射仍可被内核共享，不能笼统说“静态链接的进程完全无法共享代码页”。

## Shared object 与 ABI

动态可执行文件通常记录 `DT_NEEDED` 和动态加载器路径；运行时解析共享对象及其符号。共享库更新必须保持 ABI、symbol version 和行为兼容，不能因为文件名相同就假设可安全替换。

```bash
readelf -lW app             # program headers、INTERP
readelf -dW app             # dynamic section
objdump -T app              # dynamic symbols
ldd ./app                   # 仅对可信二进制使用
```

PIE/PIC 让代码能装载到不同地址，是现代 ASLR 的基础。编译器可能通过 GOT/PLT、PC-relative addressing 或平台特定 relocation 间接访问外部符号；具体机制由 ABI 与工具链决定。

# 加载与进入 main

在 Linux ELF 环境中，概念流程如下：

1. 程序调用 `execve`，内核验证文件格式与执行权限。
2. 内核依据 program headers 建立新的虚拟地址空间并映射 `PT_LOAD` segment。
3. 若存在 `PT_INTERP`，内核先启动指定的动态加载器。
4. 动态加载器映射依赖库、完成必要 relocation，并运行初始化代码。
5. 控制进入 ELF entry point，通常是运行时启动代码 `_start`，而不是直接进入 `main`。
6. 启动代码准备 `argc/argv/envp`、C/C++ runtime、构造函数等，然后调用 `main`。
7. `main` 返回后，runtime 处理退出函数、析构与系统调用。

```text
shell
  └─ fork/clone + execve
       └─ kernel maps executable
            └─ dynamic loader (if needed)
                 └─ _start / CRT
                      └─ main
                           └─ exit path
```

ELF 文件中的虚拟地址不是启动前就存在的物理内存；页表、按需调页、共享映射和 copy-on-write 共同决定运行时驻留。

# 构建结果为何会变化

同一源码的二进制可能受以下因素影响：

- 编译器版本、目标架构和 `-march/-mtune`；
- 宏、feature flags、优化、LTO 和 sanitizer；
- 标准库、ABI、链接器与 linker script；
- build path、时间戳、随机种子和未固定依赖。

可复现构建要求记录工具链与依赖、隔离构建目录，并尽量消除非确定输入。生产问题排查时，符号文件、build ID、编译参数和对应源码版本应作为同一组证据保存。

# 实验与掌握标准

建立两个源文件，逐阶段观察：

```bash
g++ -std=c++20 -O0 -g -c main.cpp -o main.o
g++ -std=c++20 -O0 -g -c calc.cpp -o calc.o

nm -C main.o
readelf -rW main.o
objdump -drC main.o

g++ main.o calc.o -o app
readelf -hSWlWd app
objdump -dC app | less
strace -f -o trace.txt ./app
```

然后完成三次故障注入：

1. 漏掉 `calc.o`，解释为什么是链接错误而非编译错误。
2. 修改共享库搜索路径，观察加载失败发生在 `main` 之前。
3. 分别用 `-O0` 与 `-O2` 构建，比较符号、反汇编和调试栈。

> [!warning] 常见误区
> - 把 section 与运行时 segment 当成同一张表。
> - 看到 undefined reference 就继续改头文件，而不检查链接输入和库顺序。
> - 以为程序入口就是 `main`，忽略内核、动态加载器和 CRT。
> - 用 `ldd` 检查来源不可信的文件；某些环境下这可能执行目标相关代码。
> - 只保存最终二进制，不保存工具链、符号、build ID 和构建参数。

# 资料与后续

- [GCC documentation](https://gcc.gnu.org/onlinedocs/gcc/)
- [GNU Binutils documentation](https://sourceware.org/binutils/docs/)
- [GNU ld linker scripts](https://sourceware.org/binutils/docs/ld/Scripts.html)
- [Linux execve(2)](https://man7.org/linux/man-pages/man2/execve.2.html)
- 下一步：[Memory Hierarchy and IO (存储层级与输入输出)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)
