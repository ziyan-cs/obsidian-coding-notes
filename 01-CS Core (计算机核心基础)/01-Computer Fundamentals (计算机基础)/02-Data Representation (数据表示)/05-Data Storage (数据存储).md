---
tags:
  - cs/fundamentals
status: 🌱
---

> [!important] **核心考点**：字节序（大小端）、数据对齐、存储层次、数据持久化

## 字节序（Endianness）

多字节数据的字节存放顺序：

| 类型 | 低位字节 | 高位字节 | 常见平台 |
|------|---------|---------|---------|
| **小端 (LE)** | 低地址 | 高地址 | x86, x64 |
| **大端 (BE)** | 高地址 | 低地址 | 网络字节序,部分ARM |

```cpp
// 检测大小端
bool isLittleEndian() {
    int x = 1;
    return *(char*)&x == 1;  // 1 在小端中存为 01 00 00 00
}

// 网络序（大端）↔ 主机序
uint32_t htonl(uint32_t x);  // host to network long
uint32_t ntohl(uint32_t x);  // network to host long
```

**跨平台数据交换必须统一字节序，通常使用大端（网络字节序）。**

## 数据对齐

CPU 访问对齐数据比非对齐快得多——现代 x86 能处理非对齐访问但需额外总线周期；ARM/RISC-V 可能直接触发异常。

```cpp
// 自然对齐：数据地址是其大小的整数倍
struct Aligned {
    char c;      // 1 字节 → 偏移 0
                 // 3 字节 padding
    int i;       // 4 字节 → 偏移 4（对齐到 4）
    short s;     // 2 字节 → 偏移 8
};               // 总大小 12（对齐到最大成员 4 字节）

// 改变对齐方式
#pragma pack(push, 1)   // 取消对齐优化（节省空间，牺牲速度）
struct Packed {
    char c;      // 偏移 0
    int i;       // 偏移 1（非对齐！）
    short s;     // 偏移 5
};
#pragma pack(pop)       // 总大小 7
```

**对齐规则：**
- 基础类型对齐到自身大小
- 结构体对齐到最大成员的对齐值
- 结构体总大小为最大成员对齐值的整数倍

## 存储层次

从快到慢、从贵到便宜、从小到大的金字塔结构：

| 层次 | 速度 | 大小 | 管理方式 |
|------|------|------|---------|
| 寄存器 | ~0.3 ns | ~KB | 编译器分配 |
| L1 Cache | ~1 ns | ~32KB | 硬件管理 |
| L2 Cache | ~5 ns | ~256KB | 硬件管理 |
| L3 Cache | ~15 ns | ~MB | 硬件管理 |
| 主存 | ~100 ns | ~GB | 操作系统管理 |
| SSD | ~100 μs | ~TB | 文件系统管理 |
| 磁盘 | ~10 ms | ~TB+ | 文件系统管理 |

**时间差距约 10⁷ 倍**——这就是缓存至关重要的原因。

---


二进制编码与整数表示详见 → [Binary & Encoding（二进制与编码）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals%20(计算机基础)/02-Data%20Representation%20(数据表示)/01-Binary%20&%20Encoding%20(二进制与编码).md) · [Integer Representation（整数表示）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals%20(计算机基础)/02-Data%20Representation%20(数据表示)/02-Integer%20Representation%20(整数表示).md)
