---
tags:
  - cs
  - os
  - memory-mgmt
---

> **核心考点**：虚拟地址 vs 物理地址、MMU/TLB、缺页中断、页面置换、页表结构

## 虚拟内存基本概念

虚拟内存为每个进程提供独立的、连续的、大小为 2^n 的地址空间（如 64 位 Linux 下 48 位地址空间 = 256TB）。

```
进程 A 虚拟地址空间          物理内存
┌─────────────────────────────────────────┐         ┌────────────┐
│  0x0000 - 0x3FFF ├────────┤  页框 0     │
│                                         │         ├────────────┤
│  0x4000 - 0x7FFF │         │  页框 1    │
│                                         │    →    ├────────────┤
│  0x8000 - 0xBFFF │         │  页框 2    │
│                                         │         ├────────────┤
│  0xC000 - 0xFFFF │         │  页框 3    │
└─────────────────────────────────────────┘         └────────────┘
       ↓ 页表映射（MMU）
```

**核心作用：**
1. **隔离**：进程间互不干扰
2. **简化编程**：连续地址空间，无视物理内存碎片
3. **共享**：共享库只需加载一份物理内存
4. **按需加载**：只有用到的页才加载到物理内存（demand paging）

---

## 地址转换

```cpp
// 虚拟地址到物理地址的转换（x86-64 四级页表）
// 虚拟地址（48位）分割：
// ┌──────────┬──────────┬──────────┬──────────┬─────────┐
// │   PML4   │  PDPT    │    PD    │    PT    │  Offset │
// │  bits 47-39│bits 38-30│bits 29-21│bits 20-12│bits 11-0│
// └──────────┴──────────┴──────────┴──────────┴─────────┘
//    9 bits     9 bits     9 bits     9 bits    12 bits

// 地址转换流程（硬件 MMU 自动完成）：
// 1. MMU 从虚拟地址提取 PML4 索引
// 2. 从 CR3 读取 PML4 基址 → 找到 PDPT 页表基址
// 3. 逐级查找直到 PTE（页表项）
// 4. PTE 中提取物理页框号 + offset = 物理地址
```

### TLB（转换后备缓冲器）

MMU 内部的页表缓存，缓存最近使用的虚拟-物理地址映射：

```
访问虚拟地址
    │
    ├── TLB 命中（≈ 0.5-1 cycle）→ 直接得到物理地址 ✓
    │
    └── TLB 未命中 → 硬件遍历页表（多级内存访问 ≈ 10-100 cycles）
                      → 更新 TLB → 重试指令
```

**TLB 刷新：** 进程切换时，如果切换了页表（CR3 变化），TLB 需全部失效（或使用 `PCID` 标记）。

---

## 缺页中断（Page Fault）

```
进程访问虚拟地址 → MMU 查页表 → PTE 无效位为 1（不在内存）
    │
    ▼
CPU 触发缺页异常（Page Fault Handler）
    │
    ├── 有效访问（数据在磁盘交换区）→ 从磁盘读取到物理内存
    │      │  → 更新页表 → 重新执行指令
    │      └── 如果物理内存不足 → 页面置换 → 换出旧页
    │
    └── 无效访问（段错误）→ SIGSEGV → 进程终止
```

```cpp
// 缺页处理时间估算
// SSD 读取 4KB 页 ≈ 10-50μs
// 内存访问 4KB 页 ≈ 0.1μs（L3 命中）
// 缺页 ≈ 100-500× 慢于内存访问

#include <iostream>
#include <vector>
#include <chrono>

int main() {
    const size_t N = 512 * 1024 * 1024;  // 512MB
    std::vector<int> big(N, 0);  // 分配内存，但尚未全部实际使用
    
    auto start = std::chrono::high_resolution_clock::now();
    // 首次访问触发大量缺页（demand paging）
    for (size_t i = 0; i < N; i += 4096) {  // 每 4KB 访问一次
        big[i] = 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "First access (heavy page faults): "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
    return 0;
}
```

---

## 页面置换算法

| 算法 | 策略 | 实现 | 优缺点 |
|------|------|------|--------|
| FIFO | 淘汰最早进入的页 | 队列 | 简单，Belady 异常（帧更多缺页反而增多） |
| LRU | 淘汰最久未使用的页 | 链表 + hash | 最优近似，但实现代价高 |
| Clock | 近似 LRU | 循环扫描引用位 | 折中方案，Linux 实际使用 |
| LFU | 淘汰使用次数最少的页 | 计数器 | 可能保留冷数据过久 |
| Optimal | 淘汰未来最久不用的 | 需预知 | 不可实现，用作理论下界 |

### Clock 算法（Linux 近似 LRU）

```
每次缺页时：
1. 检查当前页框的引用位（R）
2. R=1 → 置 0，移到下一页
3. R=0 → 淘汰此页

┌───R=1──→┌───R=0──→┌───R=1──→┌───R=0──→ (回到开头)
│          │          │          │
└──────────┴──────────┴──────────┘
  保留       淘汰       保留       淘汰
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 虚拟内存的好处 | 隔离、简化、共享、按需加载 |
| TLB miss 的代价 | 硬件遍历页表，多级内存访问，比命中慢 10-100 倍 |
| 页面置换比较 | LRU ≈ Clock > FIFO（Optimal 不可实现） |
| Belady 异常 | FIFO 算法帧增多反使其缺页增加 |
| 匿名页 vs 文件页 | 匿名页（堆/栈）换出到 swap；文件页回写磁盘即可 |
| 大页（HugePages） | 2MB/1GB 页，减少页表级数和 TLB miss |

> **工程要点**：数据库、搜索引擎等内存敏感型应用通常启用 HugePages（2MB 页）以减少 TLB miss。`madvise(MADV_HUGEPAGE)` 可提示内核使用透明大页。监控 `/proc/meminfo` 的 `pgfault` / `pgmajfault` 判断是否过度缺页。

---

## 关联笔记

- [Paging & Segmentation（分页与分段）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/04-Memory%20Management（内存管理⭐）/02-Paging%20&%20Segmentation（分页与分段）.md)
- [What is a Computer（计算机本质）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/01-What%20is%20a%20Computer（计算机本质）.md)
- [Hardware vs Software（软硬件关系）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/02-Hardware%20vs%20Software（软硬件关系）.md)
- [System Abstraction（系统抽象层次）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/03-System%20Abstraction（系统抽象层次）.md)
- [Binary & Encoding（二进制与编码）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/02-Data%20Representation（数据表示）/01-Binary%20&%20Encoding（二进制与编码）.md)
