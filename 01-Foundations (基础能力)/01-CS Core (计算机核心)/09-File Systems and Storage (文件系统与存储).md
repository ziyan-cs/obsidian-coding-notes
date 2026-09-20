---
study_stage: backlog
tags: [cs/os, learning/foundation]
---

> [!abstract] 学习目标
> 从文件描述符一路追踪到 VFS、目录项、inode、页缓存和持久介质，理解命名、打开、读写、同步与崩溃一致性的边界。

# 文件不是只有一个名字

应用看到路径和文件描述符，内核需要把它们解析成对象并连接具体文件系统：

```text
路径字符串
  → mount namespace + 路径遍历
  → dentry（名称与目录关系，内存缓存）
  → inode（文件身份、元数据与数据映射）
  → page cache / filesystem implementation
  → block layer → device
```

Linux VFS（Virtual File System）给 ext4、XFS、tmpfs、procfs 等不同实现提供统一接口。以下对象要严格区分：

| 对象 | 作用 | 共享关系 |
|---|---|---|
| 文件描述符 fd | 进程表中的小整数索引 | `dup` 后可指向同一打开实例 |
| open file description | 当前偏移、状态标志等 | `dup`、`fork` 后可以共享 |
| dentry | 某个目录中的名称到对象的关联 | 主要驻留内存，用于路径缓存 |
| inode | 文件身份、权限、时间与数据映射 | 多个硬链接可指向同一 inode |

文件名存放在目录项中，不在 inode 里。一个打开的文件也不再依赖原路径字符串。

# 路径解析与链接

绝对路径从进程根目录开始，相对路径从当前工作目录或 `openat` 指定目录开始。解析要处理挂载点、`.`、`..`、符号链接、权限和 namespace；“逐段查目录”只是概念入口。

- **硬链接**：增加指向同一 inode 的目录项，通常不能跨文件系统，也通常不能给目录创建普通硬链接。
- **符号链接**：自身是独立文件，内容是待解析路径；目标可不存在，也可跨文件系统。
- `unlink` 删除目录项。只有链接计数归零且不再被打开/映射等引用时，数据才可回收。

这解释了日志轮转中的经典现象：删除正在写入的文件名后，进程仍能通过旧 fd 写数据，磁盘空间也可能暂未释放。

# 读写与页缓存

普通 buffered I/O 常经过页缓存：

```text
read：命中页缓存 → 复制给用户
      未命中 → 提交 I/O → 数据进入页缓存 → 返回

write：修改页缓存并标脏 → 稍后 writeback
       fsync/fdatasync → 请求把规定范围推进到持久层
```

`write()` 成功通常只说明数据被内核接受，不代表介质已持久化。`fsync()` 的语义还受文件系统、存储设备缓存和硬件错误处理影响；需要原子更新时，常采用“写临时文件 → fsync 文件 → rename → 必要时 fsync 目录”的协议，而不是只调用一次 `write`。

`mmap` 让文件页映射进地址空间，读写由缺页和脏页回写驱动；它不是“天然零拷贝”，也不自动提供事务或持久化顺序。

# 文件系统如何定位数据

早期教材常用直接块、一级/二级/三级间接块解释 inode 寻址。这对理解索引思想有用，但不能当成现代 ext4 的准确布局：ext4 通常使用 **extents** 表示连续逻辑块范围，并配合树结构扩展。不同文件系统可能使用 B/B+ 树、allocation group、copy-on-write tree 等组织方式。

分配策略需要同时考虑顺序访问、随机访问、空间利用、碎片、并发与恢复。目录也可能从线性表升级为哈希树或其他索引，具体取决于文件系统和目录规模。

# 崩溃一致性与日志

一次高层更新可能涉及数据块、inode、目录项和空闲空间元数据。突然掉电会让它们只完成一部分。常见方案包括：

- **日志（journaling）**：先以可恢复顺序记录操作或元数据，再更新主结构；模式和保证因文件系统而异。
- **写时复制（copy-on-write）**：写新块，再原子切换根/引用；仍需处理硬件与写入顺序。
- **应用层日志/WAL**：数据库自行定义事务恢复，不把文件系统日志误当数据库事务。

`rename` 在满足接口条件时可提供命名层面的原子替换，但不等于所有数据已经稳定落盘。需要按故障模型设计并实际做断电/故障注入测试。

# 从磁盘到 NVMe

磁盘的寻道和旋转使相邻访问与调度顺序很重要；SSD/NVMe 没有机械寻道，却仍有闪存擦写、FTL、队列深度、写放大和尾延迟。Linux blk-mq 支持多队列块层。继续死背 SSTF/SCAN 可帮助理解机械盘历史，但生产分析必须先识别介质、文件系统、I/O scheduler 和工作负载。

# 动手验证

```bash
stat file
ls -li file
ln file hard-link
ln -s file soft-link
strace -e openat,read,write,fsync,close ./program
lsof +L1                 # 查看已删除但仍打开的文件
cat /proc/filesystems
```

实验：打开文件后 `unlink`，继续通过 fd 读写，再关闭并观察链接数与空间何时变化。不要在重要数据上直接做故障实验。

# 检查理解

1. fd、open file description、dentry 与 inode 分别保存什么？
2. 为什么 `write` 成功不等于断电后仍存在？
3. 删除文件名后，已打开 fd 为什么仍有效？
4. 为什么 ext2/3 的间接块示意不能直接描述 ext4？

> [!summary] 本篇结论
> 路径是名字解析入口，fd 是进程句柄，inode 表示文件对象，页缓存连接内存与持久介质。可靠存储必须明确可见性、写回、顺序和崩溃模型，而不是把“调用成功”当作“已经持久化”。

## 权威依据

- [Linux Virtual File System](https://docs.kernel.org/filesystems/vfs.html)
- [Linux pathname lookup](https://docs.kernel.org/filesystems/path-lookup.html)
- [Linux ext4 documentation](https://docs.kernel.org/filesystems/ext4/index.html)

下一步：[10-Synchronization Primitives (同步原语)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/10-Synchronization%20Primitives%20(同步原语).md)
