---
tags:
  - cs/fundamentals
status: 🌱
---

# What is a Computer — 计算机本质

> [!important] **核心考点**：计算机的五大逻辑部件、冯·诺依曼架构、存储程序概念、指令与数据同存

## 计算机的本质

计算机是一种**存储程序**的通用电子设备：将指令和数据以二进制形式存储在内存中，运行时逐条取指令、译码、执行。

## 冯·诺依曼架构

```
                     ┌────────────────────┐
                     |    Control Unit    │
                     └─────────┬──────────┘
           ┌───────────────────┼────────────────────┐
           │                   │                    │
           ▼                   ▼                    ▼
   ┌────────────────┐   ┌─────────────┐   ┌──────────────────┐
   │  Input Device  │   │     ALU     │   │   Output Device  │
   └───────┬────────┘   └──────┬──────┘   └─────────┬────────┘
           │                   │                    │
           └───────────────────┼────────────────────┘
                               │
                               ▼
                       ┌────────────────┐
                       │     Memory     │
                       └────────────────┘
```

**Five Components:**

| Component                       | Function                                   |
| ------------------------------- | ------------------------------------------ |
| **Control Unit**                | Fetch, decode, coordinate other components |
| **ALU** (Arithmetic Logic Unit) | Execute arithmetic & logic operations      |
| **Memory**                      | Store instructions & data (main memory)    |
| **Input Device**                | Send external data into the computer       |
| **Output Device**               | Output computation results                 |

## 存储程序概念

- 指令和数据**没有区别**——都是二进制数，存在同一内存中
- 指令由 **操作码（opcode）+ 操作数（operand）** 组成
- 程序执行 = 重复：取指令 → 译码 → 执行
- 这意味着**程序可以修改自身**（自修改代码，现代架构通常禁止）

## 指令执行基本流程

```
PC → Address → Fetch instruction → IR
                                    ↓
	                              Decode
                                    ↓
                           Control signals
                                    ↓
                     ALU / Mem access / Write back
                                    ↓
                          PC = PC + instr len
                                    ↓
                                Repeat...
```

## 现代 CPU 的改进

冯·诺依曼架构的**瓶颈**：CPU 和内存之间的速度差距（**冯·诺依曼瓶颈**），即内存带宽限制了 CPU 的指令/数据吞吐。

现代改进：
- **哈佛架构**：指令和数据分开存储（嵌入式 MCU 常用）
- **modified Harvard**：指令缓存和数据缓存分离（现代 CPU）
- **缓存层次结构**：L1/L2/L3 Cache 缓解主存速度瓶颈
- **流水线**：将取指、译码、执行等阶段并行

---


计算机架构与抽象详见 → [Hardware vs Software（软硬件关系）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals%20(计算机基础)/01-Computer%20Overview%20(计算机系统总览)/02-Hardware%20vs%20Software%20(软硬件关系).md) · [System Abstraction（系统抽象层次）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals%20(计算机基础)/01-Computer%20Overview%20(计算机系统总览)/03-System%20Abstraction%20(系统抽象层次).md)
