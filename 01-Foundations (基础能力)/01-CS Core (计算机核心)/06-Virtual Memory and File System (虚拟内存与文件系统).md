---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 06-Virtual Memory and File System (虚拟内存与文件系统)

> [!abstract] 阅读定位
>
> 本专题将同一条学习链上的基础概念整合为一篇：先建立整体模型，再阅读机制、边界和例子。

## Virtual Memory (虚拟内存)

> [!abstract] 核心考点：虚拟地址 vs 物理地址、MMU/TLB、缺页中断、页面置换、页表结构

## 虚拟内存基本概念

虚拟内存为每个进程提供独立的、连续的、大小为 2^n 的地址空间（如 64 位 Linux 下 48 位地址空间 = 256TB）。

```text
┌────────────────────────────────────┐
│  VIRTUAL ADDRESS                   │
├────────────────────────────────────┤
│  Virtual Page Number (VPN)         │
│  Page Offset                       │
└──────────────┬─────────────────────┘
               │
               ▼
┌──────────────┴─────────────────────┐
│  MMU (Page Table Walk in Hardware) │
└──────────────┬─────────────────────┘
               │
               ▼
┌──────────────┴──────────────────────┐
│  TLB (Translation Lookaside Buffer) │
├─────────────────────────────────────┤
│  ├── TLB hit → Physical Frame       │
│  │              Number (PFN)        │
│  │                                  │
│  └── TLB miss → Page Table (memory) │
│                  ↓                  │
│               Physical Frame Number │
└─────────────────────────────────────┘
               │
               ▼
┌──────────────┴─────────────────────┐
│  PHYSICAL ADDRESS                  │
│  = PFN + Page Offset               │
└────────────────────────────────────┘
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
// ┌───────────┬───────────┬───────────┬───────────┬──────────┐
// │   PML4    │   PDPT    │    PD     │    PT     │  Offset  │
// │ bits 47-39│ bits 38-30│ bits 29-21│ bits 20-12│ bits 11-0│
// └───────────┴───────────┴───────────┴───────────┴──────────┘
//    9 bits      9 bits      9 bits      9 bits      12 bits

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

┌───R=1──→ ┌───R=0──→ ┌───R=1──→ ┌───R=0──→ (回到开头)
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

> [!tip]- **工程要点**：数据库、搜索引擎等内存敏感型应用通常启用 HugePages（2MB 页）以减少 TLB miss。`madvise(MADV_HUGEPAGE)` 可提示内核使用透明大页。监控 `/proc/meminfo` 的 `pgfault` / `pgmajfault` 判断是否过度缺页。

---


分页与分段机制详见 → [Paging & Segmentation（分页与分段）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/04-Memory%20Management%20(内存管理)/02-Paging%20&%20Segmentation%20(分页与分段).md)

---

## Paging and Segmentation (分页与分段)

> [!abstract] 核心考点：分页 vs 分段、多级页表、页表项格式、段页式结合、内存碎片

## 分页（Paging）

将虚拟地址空间划分为固定大小的页（Page, 4KB），物理内存划分为同样大小的页框（Frame）。

### 优势与问题

| 优势 | 问题 |
|------|------|
| 无外部碎片 | 有内部碎片（最后一页平均浪费 2KB） |
| 支持虚拟内存 | 页表占用内存（多级页表解决） |
| 简化分配管理 | 页表切换开销 |

### x86-64 四级页表

```
虚拟地址 48 位：
┌───────┬───────┬───────┬───────┬────────┐
│ PML4  │ PDPT  │ PD    │ PT    │ OFFSET │
│ 9 bit │ 9 bit │ 9 bit │ 9 bit │ 12 bit │
└───┬───┴───┬───┴───┬───┴───┬───┴───┬────┘
    │       │       │       │       │
    ▼       ▼       ▼       ▼       ▼
  512 项   512项    512项   512项   4KB 页内
```

**为什么用多级页表：**
- 64 位虚拟地址空间 ⇒ 单级页表需 2^48 / 2^12 × 8B = 2^39 B = 512GB（显然不可行）
- 四级页表：顶级页表仅需 512 × 8B = 4KB，未使用的中间级页表不必分配

### 页表项（PTE）格式

```c
// x86-64 页表项（8 字节）
struct PTE {
    uint64_t present    : 1;   // 页面是否在物理内存中
    uint64_t writable   : 1;   // 是否可写（写时复制的基础）
    uint64_t user       : 1;   // 用户态是否可访问
    uint64_t write_through: 1; // 直写模式
    uint64_t cache_disabled: 1;// 禁用缓存
    uint64_t accessed   : 1;   // 已访问（由 MMU 置位，Clock 算法用）
    uint64_t dirty      : 1;   // 已被修改（回写用）
    uint64_t huge_page  : 1;   // 大页（2MB/1GB 页）
    uint64_t global     : 1;   // 全局页（不刷新 TLB）
    uint64_t rsvd       : 3;   // 保留
    uint64_t pfn        : 36;  // 物理页框号（Physical Frame Number）
    uint64_t rsvd2      : 4;   // 保留
    uint64_t protection_key : 4; // 保护密钥
    uint64_t execute_disable : 1; // 禁止执行（NX 位）
};
```

---

## 分段（Segmentation）

将程序划分为逻辑段（代码、数据、栈、堆），每段有独立的基址和长度。

```c
// x86 分段：逻辑地址 = 段选择子 : 偏移量
// 段描述符（GDT/LDT 中）
struct SegmentDescriptor {
    uint16_t limit_low;      // 段限长低 16 位
    uint16_t base_low;       // 基址低 16 位
    uint8_t  base_mid;       // 基址中间 8 位
    uint8_t  type : 4;       // 段类型（代码/数据/系统）
    uint8_t  s    : 1;       // 系统段?
    uint8_t  dpl  : 2;       // 特权级（0=内核, 3=用户）
    uint8_t  present : 1;    // 存在位
    uint8_t  limit_high : 4; // 段限长高 4 位
    uint8_t  flags : 4;      // G, D/B, L, AVL
    uint8_t  base_high;      // 基址高 8 位
};
```

**分段的不足：**
- 外部碎片（段大小不同，分配释放产生空洞）
- 段限长限制（段不能比物理内存更大）
- 现代 OS（Linux）实际上扁平寻址（所有段基址为 0），主要用分页

---

## 段页式结合

```text
┌──────────────────────────────────────────────┐
│               SEGMENTATION                   │
├──────────────────────────────────────────────┤
│  Logical Address (segment:offset)            │
│         │                                    │
│         ▼                                    │
│  Segment Table (segment → base + limit)      │
│         │                                    │
│         ▼                                    │
│  Linear Address                              │
└─────────────────────┬────────────────────────┘
                      │
                      ▼
┌──────────────────────────────────────────────┐
│                   PAGING                     │
├──────────────────────────────────────────────┤
│  Linear Address                              │
│         │                                    │
│         ▼                                    │
│  Page Directory                              │
│         │                                    │
│         ▼                                    │
│  Page Table                                  │
│         │                                    │
│         ▼                                    │
│  Physical Page Frame                         │
└─────────────────────┬────────────────────────┘
                      │
                      ▼
              Physical Address
```

**Linux 的实际做法：**
- 将 4 个段（内核代码/数据、用户代码/数据）基址全部设为 0，限长设为全部 4GB
- 逻辑地址 = 线性地址，分段被"绕过"
- 内存保护与隔离完全依赖分页机制

---

## 内存碎片对比

| 类型 | 定义 | 产生原因 | 解决方案 |
|------|------|---------|---------|
| 外部碎片 | 空闲内存不连续 | 频繁分配释放不同大小的块 | 分页（固定大小）、伙伴系统 |
| 内部碎片 | 分配大于实际需求 | 固定大小分配 | SLAB 分配器（按大小分级） |

### 伙伴系统（Buddy System）

```cpp
// 伙伴系统核心思想：将内存按 2^n 划分
// 分配：向上取整到 2^n，如有剩余分裂为两个伙伴
// 释放：检查伙伴是否空闲，是则合并

// 示例：分配 70KB（假设最小 8KB，最大 128KB）
// 初始空闲块：128KB
// 1. 70KB → 向上取整到 128KB？不，< 128 且 > 64 → 分配 128KB
//    或分裂：128 = 64 + 64，分配一个 64（不够）
//    再分裂另一个 64 = 32 + 32，合并 64+32 给 70KB 用
```

## SLAB 分配器

Linux 内核为小块对象分配优化的机制，预分配大小固定的缓存：

```
┌─────────────────────────────────────┐
│  kmem_cache (size-32, size-64...)   │
│  ┌────┬────┬────┬────┬────┬────┐    │
│  │ 32 │ 32 │ 32 │ 32 │ 32 │ 32 │    │ ← slab
│  └────┴────┴────┴────┴────┴────┘    │
│  ┌────┬────┬────┬────┬────┬────┐    │
│  │ 32 │ 32 │free│free│free│free│    │ ← slab（部分使用）
│  └────┴────┴────┴────┴────┴────┘    │
│  ┌────┬────┬────┬────┬────┬────┐    │
│  │free│free│free│free│free│free│    │ ← slab（完全空闲）
│  └────┴────┴────┴────┴────┴────┘    │
└─────────────────────────────────────┘
```

---

## 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| 分页 vs 分段 | 分页固定大小（无外部碎片），分段逻辑单元（按段保护） |
| 多级页表省空间的本质 | 未使用的中间级页表不分配 |
| 页表项 dirty 位 | 判断换出时是否需要回写（dirty=1 要写，=0 直接丢弃） |
| Linux 为什么绕过分段 | 简化，扁平虚拟地址空间更易移植 |
| TLB reach | TLB 项数 × 页大小，大页可扩大 TLB reach |
| NX 位阻止代码执行 | 数据页标记为不可执行，缓解缓冲区溢出攻击 |
| 伙伴系统的外部碎片 | 虽为固定大小，但长期运行仍可能碎片化 |

> [!tip]- **工程要点**：分配器选择——频繁分配小对象用 SLAB/kmalloc-cache；大块分配用伙伴系统（2^n 对齐）。了解 `cat /proc/buddyinfo` 可诊断内存碎片化程度。DMA 区域通常需要连续物理内存（`__GFP_DMA`）。

---


虚拟内存机制详见 → [Virtual Memory（虚拟内存）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/04-Memory%20Management%20(内存管理)/01-Virtual%20Memory%20(虚拟内存).md)

---

## File System Fundamentals (文件系统基础)

> [!abstract] 核心考点：文件系统结构、inode、目录结构、文件分配方式、磁盘调度、硬链接 vs 软链接

## 文件系统层次

```
应用程序（open/read/write）
    ↓               系统调用接口
虚拟文件系统（VFS）
    ↓               抽象层（统一 inode/dentry/superblock）
具体文件系统（ext4, NTFS, FAT32...）
    ↓               布局与管理
块设备层（Block Layer）
    ↓               I/O 调度
磁盘驱动（Disk Driver）
```

---

## inode（索引节点）

Linux 文件系统中的核心元数据结构，每个文件/目录对应一个 inode：

```c
// ext4 inode 结构（简化）
struct ext4_inode {
    uint16_t i_mode;       // 文件类型 + 权限（rwxr-xr-x）
    uint16_t i_uid;        // 所有者 UID
    uint32_t i_size;       // 文件大小（字节）
    uint32_t i_atime;      // 最后访问时间
    uint32_t i_ctime;      // 最后状态修改时间
    uint32_t i_mtime;      // 最后内容修改时间
    uint32_t i_dtime;      // 删除时间
    uint16_t i_gid;        // 组 ID
    uint16_t i_links_count;// 硬链接计数
    uint32_t i_blocks;     // 数据块数
    uint32_t i_flags;      // 标志
    union {
        uint32_t i_block[15]; // 数据块指针（EXT2_N_BLOCKS = 15）
        // ext4: 支持 extents 树
    };
    uint32_t i_generation; // 文件版本
    // ...
};
```

**inode 的 15 个块指针（ext2/3）：**

```
i_block[0-11] → 12 个直接块指针（指向数据块）
i_block[12]   → 1 个间接块指针（指向块指针块）
i_block[13]   → 1 个双重间接块指针
i_block[14]   → 1 个三重间接块指针

假设块大小 = 4KB，块指针 = 4B：
  直接：12 × 4KB = 48KB
  间接：1 × (4KB/4B) × 4KB = 4096 × 4KB = 16MB
  双重：4096 × 4096 × 4KB = 64GB
  三重：4096 × 4096 × 4096 × 4KB = 256TB
────────────────────────────────────
  最大文件 ≈ 256TB（理论上）
```

---

## 目录结构

目录是特殊的文件，内容为文件名到 inode 号的映射表：

```
目录文件内容（简化）：
┌──────────┬─────────┐
│ Filename │ inode # │
├──────────┼─────────┤
│ "."      │  101    │  ← 本目录
│ ".."     │   50    │  ← 父目录
│ "file1"  │  203    │
│ "file2"  │  204    │
│ "dir1"   │  305    │
└──────────┴─────────┘
```

### 文件路径解析

```
/bin/ls 的查找过程：
1. 读取根目录（inode 号为 2）→ 找到 "bin" → inode 100
2. 读取 inode 100 → 找到 "ls" → inode 500
3. 读取 inode 500 → 加载文件内容
```

---

## 文件分配方式

| 方式 | 优点 | 缺点 | 文件系统 |
|------|------|------|---------|
| **连续分配** | 顺序读取快，简单 | 外部碎片，需预知大小 | 磁带 |
| **链式分配** | 无外部碎片，大小灵活 | 随机访问慢，链接指针占空间 | FAT32 |
| **索引分配** | 直接/间接访问，大小灵活 | 小文件浪费（但 inode 直接块解决） | ext4, NTFS |

### FAT 表（链式分配）

```
FAT 表：
┌───────────┬──────────────┐
│ Cluster # │ Next Cluster │
├───────────┼──────────────┤
│  100      │   101        │
│  101      │   105        │
│  105      │   EOF        │
└───────────┴──────────────┘

read(fd, buf, 4096):
  → 读簇 100 → 查 FAT → 读簇 101 → 查 FAT → 读簇 105 → EOF
```

---

## 硬链接 vs 软链接

```bash
ln file1.txt file2.txt       # file2 是 file1 的硬链接

ln -s file1.txt link.txt     # link.txt → file1.txt
```

| 特性 | 硬链接 | 软链接 |
|------|--------|--------|
| inode | 相同 | 不同 |
| 跨文件系统 | 不行 | 可以 |
| 指向目录 | 一般不行（有空闲的除外） | 可以 |
| 原文件删除后 | 仍可访问 | 失效（dangling link） |
| `ls -l` 显示大小 | 与目标文件相同的 inode 属性 | 通常是目标路径字符串的长度 |

```cpp
#include <filesystem>
#include <iostream>

int main() {
    namespace fs = std::filesystem;
    
    // 创建硬链接
    fs::create_hard_link("original.txt", "hardlink.txt");
    
    // 创建软链接
    fs::create_symlink("original.txt", "symlink.txt");
    
    // 判断类型
    std::cout << "is symlink: " << fs::is_symlink("symlink.txt") << "\n";
    std::cout << "hard link count: " << fs::hard_link_count("original.txt") << "\n";
    
    // 读取软链接目标
    if (fs::is_symlink("symlink.txt")) {
        auto target = fs::read_symlink("symlink.txt");
        std::cout << "symlink -> " << target << "\n";
    }
    return 0;
}
```

---

## 磁盘调度

```cpp
// 电梯算法（SCAN）：磁头单向移动，沿途服务请求
// 假设磁盘请求：98, 183, 37, 122, 14, 124, 65, 67
// 磁头当前位置：53，方向：向大号

// SCAN（电梯算法）顺序：
// 53 → 65 → 67 → 98 → 122 → 124 → 183 → (到末尾) → 37 → 14
//
// C-SCAN（循环扫描）：
// 53 → 65 → 67 → 98 → 122 → 124 → 183 → (跳到开头) → 14 → 37
```

| 算法 | 策略 | 特点 |
|------|------|------|
| FCFS | 按请求顺序 | 简单但寻道时间长 |
| SSTF | 选最近请求 | 可能饥饿 |
| SCAN（电梯） | 单向到头再折返 | 无饥饿，中间区域等待时间短 |
| C-SCAN | 单向到头后跳到另一头 | 更均匀的等待时间 |

---

## 经典题型速查 · 延伸要点 3
| 题型 | 要点 |
|------|------|
| inode 与文件名 | 文件名存在目录中，inode 存元数据，数据块存内容 |
| 软链接 vs 硬链接 | 硬链接共享 inode，删除原文件不影响；软链接记录路径 |
| 文件系统挂载 | `mount /dev/sda1 /mnt` → 将设备关联到目录树 |
| RAID 级别 | RAID0（条带）、RAID1（镜像）、RAID5（奇偶校验） |
| 日志（Journaling） | ext3/4 先写日志再写数据，崩溃恢复时可回放/回滚 |
| VFS 的作用 | 统一抽象（ext4/NTFS/FAT32 都能通过 open/read/write 访问） |
| df 与 du 的区别 | df 看超级块统计，du 遍历目录计算（不一致时可能 inode 泄漏） |

> [!tip]- **工程要点**：大量小文件场景（如 Git、邮件服务器），inode 可能先于磁盘空间耗尽 ⇒ `df -i` 检查 inode 使用率。inode 密度与格式化选项相关，具体参数依文件系统和发行版验证；路径深度的影响也取决于 dentry cache 与负载，不应背固定层数阈值。

## 30 秒回答

目录本质上把文件名映射到 inode，inode 保存元数据并定位数据块；硬链接是多个目录项指向同一 inode，软链接则是保存目标路径的独立文件。删除文件名不等于立刻释放数据：只有链接计数归零且没有进程仍打开该文件，空间才会真正回收。

---


操作系统概览详见 → [OS Overview（操作系统总览）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/00-OS%20Overview%20(操作系统总览).md)

## 常见误区

- 把 **06-Virtual Memory and File System (虚拟内存与文件系统)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
06-Virtual Memory and File System (虚拟内存与文件系统)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
