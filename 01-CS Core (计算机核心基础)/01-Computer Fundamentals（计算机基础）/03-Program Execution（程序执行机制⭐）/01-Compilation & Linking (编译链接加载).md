---
tags:
  - cs
  - fundamentals
---

> **核心考点**：编译四阶段（预处理/编译/汇编/链接）、静态链接 vs 动态链接、链接器符号解析与重定位

## 编译的四个阶段

```
源文件(.c/.cpp) → 预处理(.i) → 编译(.s) → 汇编(.o) → 链接 → 可执行文件
```

| 阶段 | 工具 | 工作内容 |
|------|------|---------|
| **预处理** | cpp / gcc -E | 头文件展开、宏替换、条件编译、删除注释 |
| **编译** | cc1 / gcc -S | 词法/语法/语义分析 → 优化 → 生成汇编代码 |
| **汇编** | as / gcc -c | 汇编代码 → 机器指令 → 目标文件 (.o) |
| **链接** | ld | 符号解析 + 重定位 → 合并段表 → 可执行文件 |

```bash
# 分步执行
gcc -E main.c -o main.i    # 预处理
gcc -S main.i -o main.s    # 编译
gcc -c main.s -o main.o    # 汇编
gcc main.o -o main         # 链接
# 一步到位
gcc main.c -o main
```

## 目标文件格式（ELF）

```
ELF 目标文件：
├── ELF 头（文件type、入口点、段表offset）
├── .text    Code（read-only，机器指令）
├── .data    init global/static vars
├── .bss     uninit global/static vars（不占文件empty间）
├── .rodata  Read-only Data（字符串Const、switch Skip List）
├── .symtab  符号表（函数名、global变量名）
├── .rel.text 重定位表（需要修正的address）
└── .debug   Debuginfo（gcc -g 时generate）
```

## 链接

### 静态链接

程序链接时将所有依赖的目标文件归档合并到可执行文件中：

- 优点：运行时无依赖，部署简单
- 缺点：可执行文件大，内存浪费（同一库被多个进程加载多次）

### 动态链接

程序只记录依赖的共享库名称，运行时由动态链接器（ld.so）加载：

- 优点：可执行文件小，多进程共享同一库物理内存，更新库只需替换 .so
- 缺点：运行时需依赖目标机器有对应 .so

### 符号解析与重定位

```cpp
// main.c
extern int shared_var;  // 外部符号
void foo();             // 外部函数

int main() {
    foo();              // 链接前：call 0x00000000（占位）
    shared_var = 42;    // 链接前：mov [0x00000000], 42
    return 0;
}
```

链接器的工作：将各目标文件的同段合并，将符号引用替换为符号定义的实际地址。

---

## 关联笔记

- [Instruction Cycle (指令执行周期)](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/03-Program%20Execution（程序执行机制⭐）/02-Instruction%20Cycle%20(指令执行周期).md)
- [What is a Computer（计算机本质）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/01-What%20is%20a%20Computer（计算机本质）.md)
- [Hardware vs Software（软硬件关系）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/02-Hardware%20vs%20Software（软硬件关系）.md)
- [System Abstraction（系统抽象层次）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/03-System%20Abstraction（系统抽象层次）.md)
- [Binary & Encoding（二进制与编码）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/02-Data%20Representation（数据表示）/01-Binary%20&%20Encoding（二进制与编码）.md)
