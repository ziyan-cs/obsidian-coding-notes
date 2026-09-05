---
tags:
  - cs/computer-org
status: 🌱
---

> [!important] **核心考点**
> 存储金字塔结构、Cache 的位置与作用、局部性原理的硬件利用、不同层级的速度差异

## 存储金字塔

```
	Register         ← Fastest (~0.3ns), Small Capacity (~KB)
	L1 Cache         ← ~1ns, ~32KB
	L2 Cache         ← ~5ns, ~256KB
	L3 Cache         ← ~15ns, ~MB
	Main Memory(RAM) ← ~100ns, ~GB
	SSD / NVMe       ← ~100μs, ~TB
	HDD Hard Disk    ← ~10ms, ~TB+
```

**核心规律：** 每向下一层，速度慢约 10 倍、容量大约 10 倍、单位成本约 1/10。

## 层级管理的核心思想

- **缓存 (Caching)**：利用局部性原理，将频繁使用的数据放在更快的存储中
- **预取 (Prefetching)**：预测程序的访问模式，提前将数据加载到缓存
- **写缓冲 (Write Buffer)**：CPU 写入时不直接写内存，先写入缓存

## 各级存储的关键属性

| | 寄存器 | Cache | 主存 | 磁盘 |
|--|--------|-------|------|------|
| 访问时间 | 0.3-1 ns | 1-30 ns | 80-200 ns | 3-10 ms |
| 容量 | 几十-几百 B | 几十 KB-MB | GB-TB | TB+ |
| 易失性 | 易失 | 易失 | 易失 | 非易失 |
| 制造技术 | 触发器 | SRAM | DRAM | 磁/闪存 |

## 缓存为什么有效

```
访问模式         局部性类型    缓存效果
顺序访问         空间局部性    ✅ 极佳
循环访问同一数据  时间局部性    ✅ 极佳
随机访问         空间/时间都差  ❌ 差
```

程序运行中 90% 的时间花在 10% 的代码上（**90/10 法则**），这是缓存有效的根本原因。

## Cache Miss 的代价

```cpp
// 对 L1 Cache（32KB）友好：32KB 数据
const int N = 8192;  // 32KB / 4B = 8192
int a[N];
for (int i = 0; i < N; i++) total += a[i];
// 几乎全部 L1 hit，~1ns/次

// L1 Cache miss：16MB 数据
const int N = 4 * 1024 * 1024;
int a[N];
for (int i = 0; i < N; i++) total += a[i];
// 大量 L3/主存访问，~100ns/次 → 慢约 100 倍
```

---

Cache 的映射策略与替换算法详解 → [Cache 机制](../04-Cache%20Mechanism%20(缓存机制)%20⭐.md)
- [Basic Input & Output（基础输入输出）](/01-CS%20Core%20(计算机核心基础)/02-Computer%20Organization%20(计算机组成原理)/06-Basic%20Input%20&%20Output%20(基础输入输出).md)
