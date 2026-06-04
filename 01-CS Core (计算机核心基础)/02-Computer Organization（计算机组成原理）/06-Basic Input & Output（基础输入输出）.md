---
tags:
  - cs
  - computer-org
---

> **核心考点**：I/O 三种方式（程序控制/中断/DMA）、中断处理流程、DMA 传输机制

## I/O 的三种方式

### 1. 程序控制 I/O（轮询）

CPU 不断查询设备状态寄存器，直到设备就绪后传输数据。

```c
// 伪代码
while ((status & READY) == 0) { /* 空转等待 */ }
data = DEVICE_DATA_REGISTER;
```

- **优点**：简单，无需硬件支持
- **缺点**：CPU 被占用，不能做其他事

### 2. 中断驱动 I/O

设备准备就绪时通过中断信号通知 CPU。

```
CPU 执行程序 → 设备发出中断 → CPU 保存现场
    ↑                              ↓
    └────── Recovery现场 ← Interrupthandledone ──┘
```

**典型流程：**
1. 设备就绪，向 CPU 发送中断信号
2. CPU 执行完当前指令，检测到中断
3. 保存 PC 和状态寄存器
4. 根据中断号查询中断向量表，跳转到处理函数
5. 处理数据
6. 恢复现场，返回原程序

- **优点**：CPU 和设备可以并行工作
- **缺点**：每次传输数据量大时中断次数过多

### 3. DMA（直接存储器访问）

DMA 控制器直接在设备和内存之间传输数据，不经过 CPU。

```
CPU 启动 DMA → DMA 控制器执行数据传输 → CPU 做其他事
                                        ↓ 传输完成
                                   DMA 发送中断通知 CPU
```

```c
// 伪代码：配置 DMA
DMA.saddr = disk_buffer;     // 源地址
DMA.daddr = memory_buf;     // 目标地址
DMA.count = 4096;            // 传输字节数
DMA.mode = READ;             // 读磁盘
DMA.start();                 // 启动，CPU 返回执行其他任务
// ... 传输在后台进行 ...
DMA.interrupt_handler() {    // 传输完毕
    // 数据已在 memory_buf 中
}
```

## 三种方式对比

| 方式 | CPU 占用 | 传输速度 | 适用场景 |
|------|---------|---------|---------|
| 程序控制 I/O | 100%（轮询） | 慢 | 简单设备，轮询频率低 |
| 中断驱动 | 仅处理中断时 | 中 | 低速设备（键盘、鼠标） |
| DMA | 仅配置时 | 快 | 高速设备（磁盘、GPU） |

## 内存映射 I/O（MMIO）

- 将设备寄存器映射到 CPU 的地址空间中
- CPU 使用普通 load/store 指令访问设备寄存器
- vs **端口映射 I/O（PMIO）**：使用专用 I/O 指令（x86 的 in/out）

---

## 关联笔记

- [CPU Execution Model（CPU执行模型）](/01-CS%20Core%20(计算机核心基础)/02-Computer%20Organization（计算机组成原理）/01-CPU%20Execution%20Model（CPU执行模型）.md)
- [Instruction Cycle（指令执行流程）](/01-CS%20Core%20(计算机核心基础)/02-Computer%20Organization（计算机组成原理）/02-Instruction%20Cycle（指令执行流程）.md)
- [Memory Hierarchy（存储层级结构）](/01-CS%20Core%20(计算机核心基础)/02-Computer%20Organization（计算机组成原理）/03-Memory%20Hierarchy（存储层级结构⭐）.md)
- [Cache Mechanism（缓存机制）](/01-CS%20Core%20(计算机核心基础)/02-Computer%20Organization（计算机组成原理）/04-Cache%20Mechanism（缓存机制⭐）.md)
- [Bus System（总线系统）](/01-CS%20Core%20(计算机核心基础)/02-Computer%20Organization（计算机组成原理）/05-Bus%20System（总线系统）.md)
