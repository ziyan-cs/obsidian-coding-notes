---
tags:
  - cs/computer-org
status: 🌱
---

# Instruction Cycle — 指令执行流程

> [!important] **核心考点**：指令执行全流程、各阶段微操作、流水线深度对性能的影响

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
