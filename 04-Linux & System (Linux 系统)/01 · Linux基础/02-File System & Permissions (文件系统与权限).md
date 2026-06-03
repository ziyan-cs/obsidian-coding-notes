---
tags:
  - linux
  - basics
---

> **核心考点**：Linux 文件系统层次结构、文件权限 rwx 与 chmod/chown、inode 与硬软链接

## Vim 使用

### 光标移动

| 操作 | 作用 |
|------|------|
| `w` / `b` | 跳到下/上一个单词开头 |
| `0` / `^` / `$` | 行首 / 首个非空字符 / 行尾 |
| `gg` / `5gg` / `Shift+G` | 文件首 / 第 5 行 / 文件尾 |
| `Ctrl+U` / `Ctrl+D` | 上/下翻半页 |

### 内容操作

| 操作 | 作用 |
|------|------|
| `dd` / `5dd` | 剪切当前行 / 后 5 行 |
| `d$` / `d0` | 剪切光标到行尾 / 行首 |
| `yy` / `5yy` | 复制当前行 / 后 5 行 |
| `p` / `P` | 粘贴到光标后 / 前 |
| `u` / `Ctrl+R` | 撤销 / 恢复撤销 |

⚠️ 编辑模式下 `Ctrl+S` 会卡住终端，用 `Ctrl+Q` 恢复。

### 查找与替换

```vim
:/keyword       " 向下搜索（n 下一个，N 上一个）
?keyword        " 向上搜索

:%s/old/new/g   " 全局替换
:s/old/new/g    " 替换当前行
:%s/old/new/gc  " 逐个确认替换
```

### 行号

```vim
:set nu         " 显示行号
:set nonu       " 取消行号
:5              " 跳到第 5 行
```

### 文件操作

```vim
:w              " 保存
:wq / ZZ        " 保存并退出
:q!             " 强制退出不保存
:w newfile      " 另存为新文件
:e otherfile    " 打开另一个文件
```

### 多文件与分屏

```vim
vim file1 file2
:n              " 下一个文件
:N              " 上一个文件
:ls             " 列出所有打开的文件
:b filename     " 切换到指定文件

vim -o f1 f2    " 水平分屏（或 :sp）
vim -O f1 f2    " 垂直分屏（或 :vsp）
Ctrl+W → h/j/k/l   " 切换窗口
:q              " 关闭当前窗口
:qa             " 关闭所有窗口
```

---

## 环境变量 🔥

```bash
set              # 查看所有系统环境变量
echo $PATH       # 命令搜索路径
echo $HOME       # 当前用户家目录
echo $USER       # 登录用户名
echo $PWD        # 当前目录
echo $SHELL      # 当前 Shell
echo $LANG       # 系统语言
echo $HOSTNAME   # 主机名
```

### 配置环境变量

```bash
vim /etc/profile

export APP_HOME=/opt/myapp          # 定义软件根目录
export PATH=$APP_HOME/bin:$PATH     # 将命令目录加入 PATH

source /etc/profile                 # 生效
echo $APP_HOME                      # 验证
```

---

## Linux 文件系统层级

```
/
├── bin/     → 基本命令（ls, cp, cat...）
├── sbin/    → 系统管理命令（root 使用）
├── etc/     → 系统配置文件
├── home/    → 用户家目录
├── root/    → root 用户家目录
├── var/     → 可变数据（日志 /var/log、缓存 /var/cache）
├── tmp/     → 临时文件（重启清空）
├── proc/    → 内核与进程信息（虚拟文件系统）
├── dev/     → 设备文件
├── lib/     → 共享库 / 内核模块
├── usr/     → 用户程序与数据（/usr/bin, /usr/lib, /usr/local）
└── boot/    → 启动引导文件（内核 vmlinuz、initrd）
```

---

## 权限模型

### 权限表示

```
-rwxr-xr--  1  alice  dev  4096  May 1  main.cpp
 ↑↑↑↑↑↑↑↑↑      ↑     ↑
 │││└──┘└──┘    用户   组
 │││ 组权限 其他权限
 ││└─ 用户权限（rwx）
 │└─ 文件类型（- 普通文件，d 目录，l 软链接）
 └─ 第1位
```

| 类型 | 含义 |
|------|------|
| `-` | 普通文件 |
| `d` | 目录 |
| `l` | 软链接 |
| `c` | 字符设备 |
| `b` | 块设备 |

### rwx 权限详解

| 权限 | 对文件 | 对目录 |
|------|--------|--------|
| `r` (4) | cat/less 查看内容 | ls 查看文件名 |
| `w` (2) | vim 修改内容 | 创建/删除/重命名子项 |
| `x` (1) | 执行文件 | cd 进入目录 |
| `rx` (5) | — | ls -l 查看文件详情 |

### 修改权限

```bash
chmod 755 file        # rwxr-xr-x（数字法：r=4, w=2, x=1）
chmod u+x file        # 给所有者加执行权限（符号法）
chmod go-w file       # 去掉组和其他人的写权限

chown alice:dev file  # 修改所有者和所属组
chown alice file      # 仅修改所有者
chgrp dev file        # 仅修改所属组
chown -R alice dir/   # 递归修改目录
```

### umask 默认权限 🔥

umask 控制新文件/目录的默认权限：**实际权限 = 最大权限 - umask**

```bash
umask                  # 查看当前值（通常是 022）
umask 022              # 临时修改（重启失效）

# 文件最大 666 → 666 - 022 = 644（rw-r--r--）
# 目录最大 777 → 777 - 022 = 755（rwxr-xr-x）

# 永久生效：写入 ~/.bashrc 或 /etc/profile
echo "umask 002" >> ~/.bashrc
```

### 特殊权限位 🔥

```bash
chmod u+s file        # SetUID：以文件所有者身份执行（如 /usr/bin/passwd）
chmod g+s dir/        # SetGID：目录下新文件继承目录的组
chmod +t dir/         # Sticky bit：仅文件所有者能删自己的文件（如 /tmp）

# 数字法
chmod 4755 file       # 4 = SUID
chmod 2755 dir/       # 2 = SGID
chmod 1777 /tmp       # 1 = Sticky bit
```

---

## inode

每个文件有唯一 **inode**，存储文件元数据（权限、大小、时间戳、数据块位置），**不包含文件名**（文件名存在目录项中）。

```bash
ls -i file           # 查看 inode 号
stat file            # 查看完整元数据
df -i                # 查看 inode 使用情况（inode 用完无法创建新文件）
```

## 硬链接 vs 软链接

| 特性 | 硬链接 | 软链接 |
|------|--------|--------|
| 指向 | inode | 路径字符串 |
| 跨文件系统 | ❌ | ✅ |
| 原文件删除后 | 仍可访问 | 链接失效 |
| 目录 | 不支持 | 支持 |
| 占用空间 | 几乎为零 | 路径长度字节 |

```bash
ln -s <src> <dst>    # 创建软链接
ln <src> <dst>       # 创建硬链接
```
