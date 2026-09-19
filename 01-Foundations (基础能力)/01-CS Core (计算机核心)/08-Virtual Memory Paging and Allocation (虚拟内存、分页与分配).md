---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [cs/os, learning/foundation]
---

> [!abstract] 学习目标
> 沿“虚拟地址 → VMA → 页表 → TLB → 物理页”解释访问过程，区分各类缺页，并理解 Linux 分配与回收内存的基本层次。

# 虚拟内存的契约

虚拟内存让每个进程看到独立地址空间。它提供隔离、按页权限、稀疏映射、共享映射与按需分配；“地址连续”仅指虚拟地址连续，不保证物理页连续，也不保证已占用 RAM。

```text
虚拟地址
  → 检查是否命中 TLB
  → 未命中时进行硬件/软件页表遍历
  → 得到物理页框与权限
  → 访问缓存/内存

映射不存在或权限不符
  → page fault
  → 内核修复映射、装入数据，或向进程报告错误
```

进程的虚拟内存区域（VMA）描述一段地址的权限、用途和后备对象，例如可执行文件段、堆、栈、匿名映射或文件映射。页表保存更细粒度的页级映射；两者不能互相替代。

# 页表与 TLB

页表把虚拟页号映射到物理页框，并携带 present、读写、用户/内核、执行禁止、访问/脏等架构相关状态。Linux 提供最高五级的通用页表抽象，具体硬件可能折叠部分层级；x86-64 机器也可能使用四级或五级分页。因此不要把“64 位 = 2⁶⁴ 字节可用地址”或“永远四级页表”当成事实。

TLB 是页表转换缓存。大页能扩大 TLB 覆盖范围，也会增加内部碎片、内存整理成本与延迟抖动；Transparent Huge Pages 是否受益取决于工作集和访问模式。

## 地址空间切换

不同进程通常有不同页表根。现代 CPU 的 PCID/ASID 可区分不同地址空间的 TLB 项，因此切换进程不必然清空整个 TLB。修改映射后，内核仍需执行有范围的 TLB invalidation，并在多核间协调 shootdown。

# 缺页不是一种故障

| 类型 | 典型原因 | 是否需要存储 I/O |
|---|---|---:|
| 匿名页首次访问 | 延迟分配零页 | 否 |
| 写时复制（COW） | 写入共享只读页 | 否，通常复制内存 |
| 次缺页（minor fault） | 页已在内存，但映射尚未建立 | 否 |
| 主缺页（major fault） | 所需文件页/换出页不在内存 | 通常是 |
| 保护错误 | 写只读页、执行 NX 页等 | 不能修复时发信号 |

页错误处理器先结合 VMA 判断地址和权限是否合法，再决定分配页、建立映射、执行 COW、读取后备存储或终止访问。把所有 page fault 都叫“缺页中断并读磁盘”会误判性能。

# 映射、堆与分配器

用户程序常经过多层分配：

```text
new / malloc
  → 用户态 allocator（复用 arena、size class）
  → brk 或 mmap 向内核申请虚拟映射
  → 首次触碰时分配物理页
  → 内核伙伴系统提供页框
```

`free` 通常先把块还给用户态分配器，不保证立即归还操作系统。RSS、虚拟空间大小和 allocator 统计因此可能不同。

Linux 伙伴系统按 2 的幂次页块管理空闲物理页。例如申请略大于 16 页而不能拆分组合时，可能需要一个 32 页阶块，而不是随意拼接 16 页与 8 页。SLUB 等 slab allocator 在页之上缓存常见内核对象。外部碎片可能导致“总空闲内存不少，却找不到足够大的连续块”。

DMA 是否要求物理连续取决于设备能力、scatter-gather、IOMMU 和分配接口；不能一概而论。需要长期大块连续内存的场景还可能使用 CMA 等机制。

# 页面回收与压力

内存紧张时，内核需要在匿名页和文件页间选择回收对象：干净文件页可直接丢弃并按需重读；脏页需要回写；匿名页若要回收通常依赖 swap。若回收速度跟不上分配，任务会进入 direct reclaim，严重时触发 OOM 处理。

“Linux 使用 Clock 算法”已不足以描述现代实现。Linux 提供基于 folio 的 **Multi-Gen LRU**：用代际近似冷热，结合页表访问、反向映射与 refault 反馈进行 aging 和 eviction。具体是否启用及行为取决于内核版本和配置。

# 局部性与可观察性

```bash
cat /proc/$$/maps
cat /proc/$$/smaps_rollup
cat /proc/vmstat | head
cat /proc/buddyinfo
perf stat -e page-faults,minor-faults,major-faults -- ./program
```

设计实验时按字节而非“元素个数”计算步长。若页大小为 4 KiB，`int` 为 4 字节，则跨页步长约为 1024 个 `int`；实际页大小应由系统查询。比较顺序访问、跨页访问和随机访问时，同时记录 faults、cache/TLB 事件、运行时间和工作集大小。

# 检查理解

1. 预留巨大虚拟地址范围为什么不等于占用同等 RAM？
2. minor fault、major fault 与保护错误分别说明什么？
3. 大页为什么可能改善 TLB，却恶化碎片或延迟？
4. `free()` 后 RSS 为什么可能不立刻下降？

> [!summary] 本篇结论
> 虚拟内存通过 VMA、页表和 TLB把地址空间映射到物理页；缺页是建立或修复映射的入口，不等同于磁盘 I/O。性能分析必须穿透用户态分配器、页分配、回收和存储后备，而不是只看一个内存数字。

## 权威依据

- [Linux Page Tables](https://docs.kernel.org/mm/page_tables.html)
- [Linux Multi-Gen LRU](https://docs.kernel.org/mm/multigen_lru.html)
- [Linux Memory Management](https://docs.kernel.org/admin-guide/mm/index.html)

下一步：[09-File Systems and Storage (文件系统与存储)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/09-File%20Systems%20and%20Storage%20(文件系统与存储).md)
