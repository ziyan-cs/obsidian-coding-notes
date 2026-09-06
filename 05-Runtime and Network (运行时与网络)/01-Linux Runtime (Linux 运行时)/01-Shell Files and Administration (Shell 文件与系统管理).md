---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# 30 秒回答

**核心结论**：学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# Shell and Basic Commands (命令行与 Shell)

> [!note] 本节重点：核心考点：Shell 命令分类与速查、文件操作/查找/过滤/压缩/系统管理、重定向与管道、Shell 脚本基础

# 帮助命令

```bash
which <cmd>          # 查找命令的 PATH 路径
type <cmd>           # 判断命令类型：builtin（Shell 内置）或 external（外部二进制）
  # builtin: cd, help, type, echo, export（Shell 自带，当前进程执行）
  # external: ls, cat, find, date（独立二进制，fork 子进程执行）

help <cmd>           # 查看内置命令帮助
<cmd> --help         # 查看外部命令帮助
man <cmd/config>     # 查看命令/配置文件手册（纯文本，快速查参数）
info <cmd/tool>      # 查看 GNU 工具详细文档（结构化，超详细）
```

# 文件与目录操作

## 路径与导航

```bash
pwd                  # 查看当前绝对路径
cd                   # 进入家目录
cd ~                 # 进入家目录
cd <dir>             # 切换至指定目录
cd -                 # 返回上一个目录
cd ..                # 回到上一级目录
```

## 列出目录内容

```bash
ls                   # 读取目录 inode，列出子项
ls -lh               # 列表显示，人类可读大小
ls -a                # 显示所有文件（含隐藏文件）
ls -t                # 按修改时间排序（最新在前）
ls -S                # 按文件大小排序（最大在前）
ls -R                # 递归子目录

tree                 # 树形结构展示目录
tree -L 2            # 指定递归深度
tree -d              # 仅显示目录
tree -I "node_modules" # 排除指定目录
```

## 创建与删除

```bash
mkdir <dir>          # 创建单级目录
mkdir -p a/b/c       # 递归创建多级目录
touch <file>         # 创建空文件 / 更新文件时间戳
```

## 复制、移动、删除

```bash
cp <src> <dst>       # 复制文件
cp -r <src> <dst>    # 递归复制目录
cp -f <src> <dst>    # 强制覆盖

mv <src> <dst>       # 移动 / 重命名文件或目录

rm <file>            # 删除文件
rm -r <dir>          # 递归删除目录
rm -f <file>         # 强制删除（不提示）
rm -rf <dir>         # 递归强制删除（危险！）
```

## 链接 🔥

```bash
ln -s <src> <dst>    # 创建软链接（快捷方式，可跨文件系统，支持目录）
ln <src> <dst>       # 创建硬链接（文件备份，指向同一 inode，不可跨文件系统）

rm -rf <link_dst>
```

# 文件内容查看

```bash
cat <file>           # 全量输出文件内容
cat -n <file>        # 显示行号

more <file>          # 分页查看（q 退出）
less <file>          # 分页查看（功能更强）
  # ↑/↓：上下滚动  PageUp/PageDown：翻页
  # g：跳到开头  Shift+G：跳到末尾
  # /keyword：向下搜索（n 下一个，N 上一个）
  # ?keyword：向上搜索
  # q：退出

head -n 20 <file>    # 显示前 20 行（默认 10 行）
tail -n 20 <file>    # 显示后 20 行（默认 10 行）
tail -f <file>       # 实时追踪文件末尾（查看日志常用）

echo "text"          # 控制台输出
echo "text" > file   # 重定向写入（覆盖）
echo "text" >> file  # 重定向追加

wc <file>            # 统计行数 + 单词数 + 字节数
wc -l <file>         # 仅行数
wc -w <file>         # 仅单词数
wc -c <file>         # 仅字节数
wc -L <file>         # 显示最长行的长度
```

# 查找与过滤 🔥

## find — 文件查找

```bash
find <dir> -name "*.cpp"      # 按文件名查找
find <dir> -type f             # 只查文件（f），目录（d）
find <dir> -mtime -1           # 最近 1 天修改的文件
find <dir> -size +100M         # 大于 100MB 的文件
find <dir> -user alice         # 按所有者查找
find <dir> -name "*.o" -delete # 查找并删除
```

## locate — 快速数据库查找

```bash
updatedb                      # 更新文件名数据库
locate <file>                 # 基于预建数据库快速查找
```

## grep — 文本搜索 🔥

```bash
grep <pattern> <file>         # 在文件中搜索
grep -n <pattern> <file>      # 显示匹配行号
grep -i <pattern> <file>      # 忽略大小写
grep -r <pattern> <dir>       # 递归搜索目录
grep -v <pattern> <file>      # 反向匹配（显示不包含的行）
grep -c <pattern> <file>      # 仅统计匹配行数

grep "^start" <file>          # 以 start 开头
grep "end$" <file>            # 以 end 结尾
grep "." <file>               # 匹配任意单个字符
grep "a*" <file>              # 匹配 a 出现 0 次或多次
```

## sort / uniq — 排序与去重

```bash
sort <file>                   # 按行排序
sort -n <file>                # 按数字大小排序
sort -r <file>                # 倒序
sort -k 2 <file>              # 按第 2 列排序
sort -t ',' <file>            # 指定分隔符

uniq <file>                   # 去重（必须先排序！）
uniq -c <file>                # 显示重复次数
uniq -d <file>                # 只显示重复行
uniq -u <file>                # 只显示不重复行
```

## awk — 按列处理 🔥

```bash
awk '{print $1}' <file>       # 输出第 1 列（默认空格分隔）
awk -F ',' '{print $1,$3}'    # 指定逗号分隔符，输出第 1、3 列
awk '{sum+=$1} END {print sum}' # 计算第 1 列总和

  # $0：整行内容  $1~$N：第 N 列  $NF：最后一列
  # NR：当前行号  NF：当前行字段数
  # BEGIN{}：处理前执行  END{}：处理后执行
```

## sed — 流编辑器

```bash
sed 's/old/new/g' <file>      # 全局替换（不改原文件）
sed -i 's/old/new/g' <file>   # 直接修改文件（慎用）
sed '/pattern/d' <file>       # 删除匹配行
sed -n '5,10p' <file>         # 打印第 5-10 行
```

## cut / tr / xargs

```bash
cut -d ',' -f 1,3 <file>      # 按逗号分隔，提取第 1、3 列
cut -c 1-5 <file>             # 按字符位置提取

tr 'a-z' 'A-Z' < <file>       # 小写转大写
tr -d '\n' < <file>           # 删除换行符
tr -s ' ' < <file>            # 压缩连续空格

xargs -n 1 < <file>           # 每行一个参数执行
find . -name "*.tmp" | xargs rm  # 查找并删除（慎用）
xargs -I {} cp {} /backup/    # 用 {} 作占位符
```

# 压缩与解压

## tar 🔥

```bash
tar -cvf archive.tar <dir>       # 打包不压缩
tar -xvf archive.tar             # 解包

tar -zcvf archive.tar.gz <dir>   # 打包 + gzip 压缩
tar -zxvf archive.tar.gz         # 解压
tar -zxvf archive.tar.gz -C /dst # 解压到指定目录
tar -ztvf archive.tar.gz         # 查看包内文件（不解压）

tar -jcvf archive.tar.bz2 <dir>  # 打包 + bzip2 压缩
tar -jxvf archive.tar.bz2        # 解压
```

> **注意**：`-f` 必须紧跟压缩包名，放在最后。跨平台优先用 `.tar.gz` 或 `.zip`。

## gzip / bzip2 / zip

```bash
gzip <file>                    # 压缩单个文件（默认删除原文件）
gzip -k <file>                 # 压缩并保留原文件
gunzip <file>.gz               # 解压

zip -r archive.zip <dir>       # 递归压缩
unzip archive.zip              # 解压到当前目录
unzip archive.zip -d /dst      # 解压到指定目录
unzip -l archive.zip           # 只查看压缩包内容
```

# 系统信息

```bash
date                           # 当前完整时间
date "+%Y-%m-%d %H:%M:%S"      # 自定义格式
sudo date -s "2026-06-03 12:00:00"  # 设置时间
hwclock -w                     # 系统时间写入硬件

uptime                         # 系统运行时长 + 用户数 + 平均负载

ifconfig                       # 查看网卡信息（旧）
ip addr                        # 查看网卡信息（新，推荐）
ip link                        # 查看网卡启用状态

history                        # 命令历史
history 10                     # 最近 10 条
history -c                     # 清空历史 ⚠️
!!                             # 重复上一条命令
!5                             # 执行编号 5 的命令
```

## 常用快捷键

| 快捷键 | 作用 |
|--------|------|
| `Tab` | 命令/文件名补全 |
| `Ctrl + C` | 终止当前进程 |
| `Ctrl + Z` | 后台挂起当前进程 |
| `Ctrl + D` | 退出当前 Shell |
| `Ctrl + L` | 清屏 |
| `Ctrl + A` / `Ctrl + E` | 跳到行首 / 行尾 |
| `Ctrl + U` / `Ctrl + K` | 删除光标前 / 后所有字符 |
| `Ctrl + R` | 搜索历史命令 |
| `Ctrl + S` | ⚠️ 会卡住终端（用 `Ctrl + Q` 恢复） |

# 重定向与管道

```bash
cmd > file           # 标准输出重定向（覆盖）
cmd >> file          # 标准输出重定向（追加）
cmd 2> err.log       # 标准错误重定向
cmd > out.log 2>&1   # 标准输出和错误合并重定向
cmd > /dev/null      # 丢弃所有输出

cmd1 | cmd2          # 管道：cmd1 的输出作为 cmd2 的输入
cmd | tee file       # 同时输出到终端和文件
```

# Shell 脚本基础

## 脚本结构与执行

```bash
#!/bin/bash                    # shebang，必须第一行
#!/usr/bin/env bash            # 更通用的写法

set -euo pipefail              # 严格模式：出错即停、未定义变量报错、管道失败报错
```

## 变量

```bash
name="world"
readonly year="2026"           # 只读变量
local tmp=$1                   # 局部变量（函数内使用）

echo "Hello, ${name}!"         # 推荐加 {} 避免歧义
echo $name                     # 简单引用可省略 {}

dt=$(date)                     # 推荐
dt2=`date`                     # 旧写法，不推荐
```

## 特殊变量

```bash
$0       # 脚本自身文件名
$1 ~ $9  # 位置参数（${10} 获取第 10 个参数）
$#       # 参数个数
$@       # 所有参数（独立列表："a" "b" "c"）
$*       # 所有参数（单个字符串："a b c"）
$?       # 上一条命令退出码（0=成功，非0=失败）
$$       # 当前脚本 PID
$!       # 上一个后台进程 PID
```

## 条件判断

```bash
[[ "$str" == "hello" ]]        # 相等
[[ -z "$str" ]]                # 为空
[[ -n "$str" ]]                # 非空

(( a > 1 ))                    # >, >=, <, <=, ==, !=
(( a > b && a > c ))           # 支持 && 和 ||

[[ "$a" -eq "$b" ]]            # -eq, -ne, -gt, -lt, -ge, -le

[[ -e "$path" ]]               # 存在（文件或目录）
[[ -f "$path" ]]               # 是普通文件
[[ -d "$path" ]]               # 是目录
[[ -x "$file" ]]               # 可执行
[[ -r "$file" ]]               # 可读
```

## 控制流程

```bash
if (( $# < 2 )); then
    echo "Usage: $0 <arg1> <arg2>"
    exit 1
fi

for i in "$@"; do
    echo "$i"
done

for (( i=0; i<=100; i++ )); do
    ((sum += i))
done

while (( cnt <= 5 )); do
    echo "Count: $cnt"
    ((cnt++))
done

case $1 in
    "start") echo "Starting...";;
    "stop")  echo "Stopping...";;
    *)       echo "Unknown: $1";;
esac
```

## 函数

```bash
function greet() {
    local name=$1              # local 限制变量作用域
    echo "Hello, $name"
}

add() {
    local a=$1 b=$2
    echo $((a + b))            # 通过 echo 返回结果
}

result=$(add 3 5)
greet "Alice"
```

## 数值运算

```bash
result=$(((2 + 3) * 4))

result=$(expr 2 + 3)

let "result = (2 + 3) * 4"
```

## 输入输出

```bash
read -p "Enter name: " name           # 带提示符
read -t 10 -p "Enter number: " num    # 10 秒超时

```

> [!tip]- **工程要点**：
> - 脚本中始终使用 `set -euo pipefail` 及早暴露错误
> - 变量引用始终加双引号 `"$var"` 防止分词问题
> - `[[ ]]` 比 `[ ]` 功能更强，支持 `&&` `||` 和正则
> - `$(( ))` 是最推荐的整数运算方式

Shell 命令与脚本详解见 → [File System & Permissions (文件系统与权限)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/02-File%20System%20&%20Permissions%20(文件系统与权限).md) · [System Administration Basics (系统管理基础)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/03-System%20Administration%20Basics%20(系统管理基础).md)

---

# File System and Permissions (文件系统与权限)

> [!note] 本节重点：核心考点：Linux 文件系统层次结构、文件权限 rwx 与 chmod/chown、inode 与硬软链接

# Vim 使用

## 光标移动

| 操作 | 作用 |
|------|------|
| `w` / `b` | 跳到下/上一个单词开头 |
| `0` / `^` / `$` | 行首 / 首个非空字符 / 行尾 |
| `gg` / `5gg` / `Shift+G` | 文件首 / 第 5 行 / 文件尾 |
| `Ctrl+U` / `Ctrl+D` | 上/下翻半页 |

## 内容操作

| 操作 | 作用 |
|------|------|
| `dd` / `5dd` | 剪切当前行 / 后 5 行 |
| `d$` / `d0` | 剪切光标到行尾 / 行首 |
| `yy` / `5yy` | 复制当前行 / 后 5 行 |
| `p` / `P` | 粘贴到光标后 / 前 |
| `u` / `Ctrl+R` | 撤销 / 恢复撤销 |

⚠️ 编辑模式下 `Ctrl+S` 会卡住终端，用 `Ctrl+Q` 恢复。

## 查找与替换

```vim
:/keyword       " 向下搜索（n 下一个，N 上一个）
?keyword        " 向上搜索

:%s/old/new/g   " 全局替换
:s/old/new/g    " 替换当前行
:%s/old/new/gc  " 逐个确认替换
```

## 行号

```vim
:set nu         " 显示行号
:set nonu       " 取消行号
:5              " 跳到第 5 行
```

## 文件操作

```vim
:w              " 保存
:wq / ZZ        " 保存并退出
:q!             " 强制退出不保存
:w newfile      " 另存为新文件
:e otherfile    " 打开另一个文件
```

## 多文件与分屏

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

# 环境变量 🔥

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

## 配置环境变量

```bash
vim /etc/profile

export APP_HOME=/opt/myapp          # 定义软件根目录
export PATH=$APP_HOME/bin:$PATH     # 将命令目录加入 PATH

source /etc/profile                 # 生效
echo $APP_HOME                      # 验证
```

---

# Linux 文件系统层级

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

# 权限模型

## 权限表示

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

## rwx 权限详解

| 权限 | 对文件 | 对目录 |
|------|--------|--------|
| `r` (4) | cat/less 查看内容 | ls 查看文件名 |
| `w` (2) | vim 修改内容 | 创建/删除/重命名子项 |
| `x` (1) | 执行文件 | cd 进入目录 |
| `rx` (5) | — | ls -l 查看文件详情 |

## 修改权限

```bash
chmod 755 file        # rwxr-xr-x（数字法：r=4, w=2, x=1）
chmod u+x file        # 给所有者加执行权限（符号法）
chmod go-w file       # 去掉组和其他人的写权限

chown alice:dev file  # 修改所有者和所属组
chown alice file      # 仅修改所有者
chgrp dev file        # 仅修改所属组
chown -R alice dir/   # 递归修改目录
```

## umask 默认权限 🔥

umask 控制新文件/目录的默认权限：**实际权限 = 最大权限 - umask**

```bash
umask                  # 查看当前值（通常是 022）
umask 022              # 临时修改（重启失效）

echo "umask 002" >> ~/.bashrc
```

## 特殊权限位 🔥

```bash
chmod u+s file        # SetUID：以文件所有者身份执行（如 /usr/bin/passwd）
chmod g+s dir/        # SetGID：目录下新文件继承目录的组
chmod +t dir/         # Sticky bit：仅文件所有者能删自己的文件（如 /tmp）

chmod 4755 file       # 4 = SUID
chmod 2755 dir/       # 2 = SGID
chmod 1777 /tmp       # 1 = Sticky bit
```

---

# inode

每个文件有唯一 **inode**，存储文件元数据（权限、大小、时间戳、数据块位置），**不包含文件名**（文件名存在目录项中）。

```bash
ls -i file           # 查看 inode 号
stat file            # 查看完整元数据
df -i                # 查看 inode 使用情况（inode 用完无法创建新文件）
```

# 硬链接 vs 软链接

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

文件系统与权限详解见 → [Shell & Basic Commands (命令行与Shell编程)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md) · [System Administration Basics (系统管理基础)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/03-System%20Administration%20Basics%20(系统管理基础).md)

---

# System Administration Basics (系统管理基础)

> [!note] 本节重点：核心考点：用户与组管理、磁盘管理、任务调度、服务管理 systemctl、进程管理与系统监控、日志查看

# 用户与组管理

## 创建与删除

```bash
useradd alice                    # 创建用户（默认家目录 /home/alice）
useradd -d /data/alice alice     # 指定家目录
useradd -g dev -u 1001 alice     # 指定所属组和 UID
adduser alice                    # Ubuntu 交互式创建

userdel alice                    # 删除用户（保留家目录）
userdel -r alice                 # 删除用户及家目录

groupadd dev
groupdel dev
```

## 修改用户与组

```bash
passwd                    # 修改当前用户密码
passwd alice              # 修改 alice 密码

usermod -l newname alice          # 修改用户名
usermod -d /home/new -m alice     # 修改家目录并移动文件
usermod -g dev alice              # 修改主组
usermod -aG sudo alice            # 添加附加组
gpasswd -d alice sudo             # 取消附加组
usermod -G "" alice               # 清空所有附加组

groupmod -n newname oldname       # 修改组名
groupmod -g 1001 dev              # 修改组 GID
```

## 用户切换

```bash
su alice                  # 切换用户（不切换环境变量）
su - alice                # 切换用户及环境
sudo -i                   # 切换到 root

exit                      # 退回上一级用户
whoami                    # 查看当前有效用户
who am i                  # 查看最初登录用户
who                       # 所有登录用户
w                         # 增强版 who（含操作、负载）
id                        # 查看当前 UID/GID/组
```

---

# 磁盘管理

## 设备命名

```bash
sdX~       # SCSI/SATA 硬盘（主流）
hdX~       # IDE 硬盘（旧）

```

## 查看磁盘

```bash
lsblk                     # 树结构查看分区
df -h                     # 查看磁盘挂载和使用情况
du -sh <dir>              # 查看目录大小
du -h --max-depth=1       # 查看一级子目录大小
```

## 分区与格式化

```bash
fdisk /dev/sdb            # 分区工具
  # n → p → [size]  创建主分区
  # w               保存并退出
  # d               删除分区
  # q               不保存退出

mkfs -t ext4 /dev/sdb1    # 格式化为 ext4
```

## 挂载

```bash
mount /dev/sdb1 /mnt/data     # 挂载分区
umount /dev/sdb1              # 卸载分区
mount -a                      # 验证挂载
```

## 开机自动挂载

```bash
blkid /dev/sdb1               # 查看 UUID
vim /etc/fstab                # 添加：
UUID=xxx /mnt/data ext4 defaults 0 0
```

---

# 进程管理

## 查看进程

```bash
ps aux                    # 查看所有进程（BSD 风格）
ps -ef                    # 查看所有进程（System V 风格，含 PPID）
ps -efL                   # 查看线程（LWP）

pstree -p                 # 树形展示进程父子关系

top                       # 动态查看（快捷键）
  # P：按 CPU 排序  M：按内存排序  k：终止进程  q：退出

htop                      # 更友好的 top（需安装）
```

ps 输出列含义: `USER PID PPID %CPU %MEM VSZ RSS STAT TTY COMMAND`

## 进程信号

```bash
kill -l                   # 列出所有信号
kill -15 <PID>            # 发送 SIGTERM（优雅终止，默认）
kill -9 <PID>             # 发送 SIGKILL（强制终止，不可捕获）
killall nginx             # 按名称终止
pkill <name>              # 模糊匹配终止
pidof <name>              # 获取进程 PID
```

## 进程状态

| 状态 | 含义 |
|------|------|
| R | 运行中或在运行队列中 |
| S | 可中断睡眠（等待事件） |
| D | 不可中断睡眠（IO 等待，杀不掉）⚠️ |
| Z | 僵尸态（父进程未 wait 回收） |
| T | 停止 / 被跟踪 |

---

# 系统资源监控

```bash
free -h                   # 内存使用情况
df -h                     # 磁盘空间
du -sh <dir>              # 目录大小
lscpu                     # CPU 信息
uname -a                  # 内核版本和系统信息
uptime                    # 运行时长 + 负载
vmstat 1                  # 每秒刷新 CPU/内存/IO 统计
iostat -x 1               # 磁盘 IO 统计
```

---

# 网络管理

```bash
ip addr                   # 查看网卡和 IP（推荐，替代 ifconfig）
ip link                   # 查看网卡启用状态
ip route                  # 查看路由表
ss -tlnp                  # 查看监听端口和对应进程（替代 netstat）
ss -antp                  # 所有状态的 TCP 端口
ping <host>               # 连通测试
curl -v <URL>             # HTTP 请求（-v 显示详情）
wget <URL>                # 下载文件
traceroute <host>         # 路由追踪
```

---

# 服务管理（systemd）

```bash
systemctl start nginx     # 启动服务
systemctl stop nginx      # 停止
systemctl restart nginx   # 重启
systemctl status nginx    # 查看状态
systemctl enable nginx    # 开机自启
systemctl disable nginx   # 取消自启
```

---

# 任务调度

## crontab 定时任务 🔥

```bash
systemctl start crond     # 启动 cron 守护进程
systemctl enable cron     # 设置开机自启

crontab -e                # 编辑定时任务
crontab -l                # 查看
crontab -r                # 清空
```

**格式：** `分 时 日 月 周 命令`

```bash
  * * * * * <command>

30 2 * * * /backup.sh           # 每天凌晨 2:30 备份
*/5 * * * * /check.sh           # 每 5 分钟检查
0 9 * * 1-5 /report.sh          # 工作日早 9 点
0 0 1 * * /monthly.sh           # 每月 1 号零点
```

## at 一次性任务

```bash
systemctl start atd       # 启动 atd 守护进程

at 14:30                   # 在 14:30 执行（输入命令后 Ctrl+D 结束）
at now + 1 hour            # 一小时后
atq                        # 查看待执行任务
atrm <ID>                  # 删除指定任务
```

---

# 日志查看

## journalctl

```bash
journalctl                    # 查看全部日志
journalctl -f                 # 实时跟踪日志
journalctl -n 30              # 查看最近 30 条

journalctl -u sshd            # 查看指定服务日志
journalctl -u nginx -f        # 实时跟踪某服务日志

journalctl --since "1 hour ago"
journalctl --since "today"
journalctl --since "2026-04-01" --until "2026-04-02"

journalctl -p err             # 只看错误级别日志
journalctl -k                 # 查看内核日志（替代 dmesg）
```

## tail / grep

```bash
tail -f /var/log/syslog       # 实时监控日志
grep "error" /var/log/auth.log  # 过滤错误信息
```

系统管理基础见 → [Shell & Basic Commands (命令行与Shell编程)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md) · [File System & Permissions (文件系统与权限)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/02-File%20System%20&%20Permissions%20(文件系统与权限).md)

# 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

# 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Shell Files and Administration (Shell 文件与系统管理)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
