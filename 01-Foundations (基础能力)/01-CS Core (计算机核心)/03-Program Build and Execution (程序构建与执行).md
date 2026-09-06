---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 03-Program Build and Execution (程序构建与执行)

> [!abstract] 学习定位：本专题将同一条学习链上的基础概念整合为一篇：先建立整体模型，再阅读机制、边界和例子。

## CPU Execution Model (CPU 执行模型)

> [!note] 本节重点：核心考点：CPU 核心组成（数据通路 + 控制单元）、寄存器、ALU、控制器实现方式

## CPU 的核心组成

```text
┌─────────────────────────────────────────────────────────────────┐
│  CPU INSTRUCTION PIPELINE                                       │
├─────────────────────────────────────────────────────────────────┤
│  IF (Fetch) ──→ ID (Decode) ──→ EX (Execute) ──→ MEM (Access)   │
│  ──→ WB (Writeback)                                             │
│                                                                 │
│  ├── IF:  Retrieve instruction from I-Cache                     │
│  ├── ID:  Decode instruction and operands                       │
│  ├── EX:  ALU computation / address calculation                 │
│  ├── MEM: Read/write D-Cache                                    │
│  └── WB:  Write result to register file                         │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│  CPU CORE                                                       │
├─────────────────────────────────────────────────────────────────┤
│  PC (Program Counter)       ──→ supplies next addr to IF        │
│  REG (Register File)        ──→ supplies operands to ID         │
│                             ←── receives results from WB        │
│  ALU (Arithmetic Logic Unit)──→ performs computation in EX      │
└─────────────────────────────────────────────────────────────────┘
```

## 关键寄存器

| 寄存器 | 全称 | 功能 |
|--------|------|------|
| **PC** | Program Counter | 存放下一条指令的地址 |
| **IR** | Instruction Register | 存放当前正在执行的指令 |
| **SP** | Stack Pointer | 指向栈顶 |
| **FP/BP** | Frame Pointer | 指向当前栈帧基址 |
| **通用寄存器** | — | 暂存操作数和结果 |

## ALU 的功能

ALU 是 CPU 中执行实际计算的部件：

- **算术运算**：加、减、乘、除、自增
- **逻辑运算**：与、或、异或、取反、移位
- **比较运算**：相等、大于、小于判断（产生条件码/标志位）

ALU 输出标志位：
- **ZF** (Zero Flag)：结果为 0
- **SF** (Sign Flag)：结果为负
- **CF** (Carry Flag)：无符号溢出
- **OF** (Overflow Flag)：有符号溢出

## 控制器实现

| 方式 | 原理 | 特点 |
|------|------|------|
| **硬布线** | 组合逻辑电路直接产生控制信号 | 快但复杂，难扩展 |
| **微程序** | 将控制信号存为 ROM 中的微指令 | 灵活易扩展，相对慢 |
| 现代 CPU | 硬布线为主 + 少数复杂指令用微码 | 兼顾性能与复杂度 |

---


指令执行流程详见 → [Instruction Cycle（指令执行流程）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Computer%20Organization%20(计算机组成原理)/02-Instruction%20Cycle%20(指令执行流程).md)

---

## Instruction Cycle (指令执行周期)

> [!note] 本节重点：核心考点：取指-译码-执行周期、CPU 流水线各阶段、流水线冒险与解决

## 经典指令周期

每条指令的执行经历三个基本阶段：

```
取指 (Fetch) → 译码 (Decode) → 执行 (Execute)
```

| 阶段 | 操作 |
|------|------|
| **Fetch** | 根据 PC 从指令缓存/内存取指令，PC ← PC + 4 |
| **Decode** | 解析指令的 opcode 和操作数，从寄存器堆读取操作数 |
| **Execute** | ALU 计算 / 访存 / 写回寄存器 |

## 五级流水线

现代 CPU 将指令周期拆为更细的 5 级：

```
F → D → EX → MEM → WB
取指  译码  执行  访存  写回
```

**理想情况：** 每时钟周期完成一条指令，CPI = 1。

## 流水线冒险

| 类型 | 原因 | 解决 |
|------|------|------|
| **结构冒险** | 硬件资源冲突（如同一周期取指和访存争用同一缓存） | 分离指令/数据缓存 |
| **数据冒险** | 后一条指令依赖前一条的结果 | 转发（Forwarding）/ 插入气泡（Stall） |
| **控制冒险** | 分支/跳转导致下一条指令未知 | 分支预测 + 预测执行 |

```assembly
// 数据冒险示例
ADD  R1, R2, R3    ; R1 = R2 + R3
SUB  R4, R1, R5    ; 需要 R1 的结果，必须等 ADD 写回

// 转发解决：SUB 的 EX 阶段直接从 ADD 的 EX 输出取数
```

## RISC vs CISC

| 特性 | RISC（ARM, RISC-V） | CISC（x86） |
|------|-------------------|------------|
| 指令长度 | 固定（32 位） | 可变（1-15 字节） |
| 寻址方式 | 少（load/store） | 多（内存操作数） |
| 寄存器 | 通用寄存器多（32+） | 通用寄存器少（8-16） |
| 译码复杂度 | 简单 | 复杂（需微码） |
| 典型 CPI | 1 | 变长，可 > 1 |

现代 x86 内部实为 **CISC 指令 → RISC 微操作（μop）** 的转换执行。

## 分支预测

预测错误时，已进入流水线的指令全部冲刷（约 15-20 个周期的惩罚）。

```cpp
// 可预测的分支（排序后遍历）
for (int x : sorted_data)
    if (x > threshold) action(x);   // 分支预测器准确率 > 99%

// 不可预测的分支（随机数据）
for (int x : random_data)
    if (x > threshold) action(x);   // 约 50% 预测错误率，性能骤降
```

> [!tip]- **工程要点**：分支预测失败是现代 CPU 性能损失的常见原因。对热点代码避免难以预测的条件分支（用 CMOV 指令或查表替代）。

---


编译链接加载详见 → [Compilation & Linking (编译链接加载)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/01-Computer%20Fundamentals%20(计算机基础)/03-Program%20Execution%20(程序执行机制)/01-Compilation%20&%20Linking%20(编译链接加载).md)

---

## Program Execution Flow (程序执行流程)

> [!note] 本节重点：核心考点：指令执行全流程、各阶段微操作、流水线深度对性能的影响

## 指令执行详解

### 取指阶段 (Fetch)

```
MAR ← PC              // 将要取的指令地址送入地址寄存器
MDR ← Memory[MAR]     // 从内存读取指令
IR ← MDR              // 指令存入指令寄存器
PC ← PC + 4           // PC 自增指向下一条（RISC 固定 4 字节）
```

### 译码阶段 (Decode)

```
opcode ← IR[31:26]    // 提取操作码
rs     ← IR[25:21]    // 提取源寄存器 1
rt     ← IR[20:16]    // 提取源寄存器 2
rd     ← IR[15:11]    // 提取目标寄存器（R-type）
imm    ← IR[15:0]     // 提取立即数（I-type）
control_signals ← decode(opcode)  // 译码器输出控制信号
```

### 执行阶段 (Execute)

```
// R-type（寄存器 - 寄存器）
ALU_out ← Reg[rs] OP Reg[rt]

// I-type（立即数）
ALU_out ← Reg[rs] OP Imm

// Load/Store
ALU_out ← Reg[rs] + Imm   // 计算内存地址

// Branch
ALU_out ← PC + (Imm << 2)  // 计算目标地址
if (Reg[rs] == Reg[rt]) PC ← ALU_out  // 条件跳转
```

### 访存阶段 (Memory)

```
// Load
MDR ← Memory[ALU_out]
Reg[rt] ← MDR

// Store
Memory[ALU_out] ← Reg[rt]
```

### 写回阶段 (Write Back)

```
// R-type
Reg[rd] ← ALU_out
```

## 现代 CPU 的指令级并行

| 技术 | 说明 |
|------|------|
| **流水线** | 多条指令处于不同阶段，CPI 趋近 1 |
| **超标量** | 每周期发射多条指令（如 4-wide → CPI < 1） |
| **乱序执行** | 不依赖顺序，先执行就绪指令 |
| **推测执行** | 分支预测后提前执行，错了冲刷 |

> [!tip]- **工程要点**：理解指令周期的实际意义——写出缓存友好的代码（减少 cache miss 导致的流水线停顿）和可预测的分支（减少分支预测错误冲刷）是工程中最大化 CPU 效率的关键。

---


CPU执行模型详见 → [CPU Execution Model（CPU执行模型）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Computer%20Organization%20(计算机组成原理)/01-CPU%20Execution%20Model%20(CPU执行模型).md)

---

## Compilation Linking and Loading (编译链接与加载)

> [!note] 本节重点：核心考点：编译四阶段（预处理/编译/汇编/链接）、静态链接 vs 动态链接、链接器符号解析与重定位

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
gcc -E main.c -o main.i    # 预处理
gcc -S main.i -o main.s    # 编译
gcc -c main.s -o main.o    # 汇编
gcc main.o -o main         # 链接
gcc main.c -o main
```

## 目标文件格式（ELF）

```
ELF 目标文件：
├── ELF 头（文件类型、入口点、段表偏移）
├── .text    代码段（只读，机器指令）
├── .data    已初始化全局/静态变量
├── .bss     未初始化全局/静态变量（不占文件空间）
├── .rodata  只读数据（字符串常量、switch 跳表）
├── .symtab  符号表（函数名、全局变量名）
├── .rel.text 重定位表（需要修正的地址）
└── .debug   调试信息（gcc -g 时生成）
```

## 链接

### 静态链接

程序链接时将所有依赖的目标文件归档合并到可执行文件中：

- 优点：运行时无依赖，部署简单
- 缺点：可执行文件大，内存浪费（同一库被多个进程加载多次）

### 动态链接

程序只记录依赖的共享库名称，运行时由动态链接器（ld.so）加载：

- 优点：可执行文件通常更小，多个进程可共享库的只读映射页
- 缺点：运行时需有兼容的库与 ABI；替换共享库必须满足兼容性与发布策略，不能直接假设可热替换

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

## 30 秒回答

预处理展开宏和头文件，编译生成汇编，汇编生成可重定位目标文件，链接才解决跨文件符号与地址。静态链接把依赖代码带进产物；动态链接把兼容性与部署边界留到运行时。排查构建问题时，先分清是编译期类型/语法错误，还是链接期缺符号/多定义。

---


指令执行周期详见 → [Instruction Cycle (指令执行周期)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/01-Computer%20Fundamentals%20(计算机基础)/03-Program%20Execution%20(程序执行机制)/02-Instruction%20Cycle%20(指令执行周期).md)



## 零基础阅读路径

先读本页的总览与术语，再沿“数据/指令 → 硬件状态 → 操作系统抽象 → 可见结果”追踪一个例子。遇到性能数字先跳过，等能解释状态流转后再回来比较。

## 常见误区

- 把 **03-Program Build and Execution (程序构建与执行)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Program Build and Execution (程序构建与执行)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
