---
tags:
  - cs
  - os
---

> **核心考点**：文件系统结构、inode、目录结构、文件分配方式、磁盘调度、硬链接 vs 软链接

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
┌──────────┬────────┐
│ 文件名    │ inode #│
├──────────┼────────┤
│ "."      │  101    │  ← 本目录
│ ".."     │   50    │  ← 父目录
│ "file1"  │  203    │
│ "file2"  │  204    │
│ "dir1"   │  305    │
└──────────┴────────┘
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
┌──────┬──────────┐
│ 簇号  │ 下一簇   │
├──────┼──────────┤
│  100  │   101    │
│  101  │   105    │
│  105  │    EOF   │
└──────┴──────────┘

read(fd, buf, 4096):
  → 读簇 100 → 查 FAT → 读簇 101 → 查 FAT → 读簇 105 → EOF
```

---

## 硬链接 vs 软链接

```bash
# 硬链接：两个文件名指向同一个 inode
ln file1.txt file2.txt       # file2 是 file1 的硬链接
# 特征：inode 号相同，链接计数 +1，删除一个不影响其他

# 软链接（符号链接）：独立文件指向另一个文件路径
ln -s file1.txt link.txt     # link.txt → file1.txt
# 特征：不同 inode，内容为目标路径，删除原文件后链接失效
```

| 特性 | 硬链接 | 软链接 |
|------|--------|--------|
| inode | 相同 | 不同 |
| 跨文件系统 | 不行 | 可以 |
| 指向目录 | 一般不行（有空闲的除外） | 可以 |
| 原文件删除后 | 仍可访问 | 失效（dangling link） |
| 大小 | 0（与原文件共享数据） | 路径字符串长度 |

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

## 经典题型速查

| 题型 | 要点 |
|------|------|
| inode 与文件名 | 文件名存在目录中，inode 存元数据，数据块存内容 |
| 软链接 vs 硬链接 | 硬链接共享 inode，删除原文件不影响；软链接记录路径 |
| 文件系统挂载 | `mount /dev/sda1 /mnt` → 将设备关联到目录树 |
| RAID 级别 | RAID0（条带）、RAID1（镜像）、RAID5（奇偶校验） |
| 日志（Journaling） | ext3/4 先写日志再写数据，崩溃恢复时可回放/回滚 |
| VFS 的作用 | 统一抽象（ext4/NTFS/FAT32 都能通过 open/read/write 访问） |
| df 与 du 的区别 | df 看超级块统计，du 遍历目录计算（不一致时可能 inode 泄漏） |

> **工程要点**：大量小文件场景（如 Git、邮件服务器），inode 可能先于磁盘空间耗尽 ⇒ `df -i` 检查 inode 使用率。ext4 格式时可调 `-I 256` 增大 inode 密度。文件系统层数过深（>5）会影响路径解析性能。

---


操作系统概览详见 → [OS Overview（操作系统总览）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/00-OS%20Overview（操作系统总览）.md)
