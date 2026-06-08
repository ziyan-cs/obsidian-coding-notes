---
tags:
  - linux
  - basics
---

> **核心考点**：Shell 命令分类与速查、文件操作/查找/过滤/压缩/系统管理、重定向与管道、Shell 脚本基础

## 帮助命令

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

## 文件与目录操作

### 路径与导航

```bash
pwd                  # 查看当前绝对路径
cd                   # 进入家目录
cd ~                 # 进入家目录
cd <dir>             # 切换至指定目录
cd -                 # 返回上一个目录
cd ..                # 回到上一级目录
```

### 列出目录内容

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

### 创建与删除

```bash
mkdir <dir>          # 创建单级目录
mkdir -p a/b/c       # 递归创建多级目录
touch <file>         # 创建空文件 / 更新文件时间戳
```

### 复制、移动、删除

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

### 链接 🔥

```bash
ln -s <src> <dst>    # 创建软链接（快捷方式，可跨文件系统，支持目录）
ln <src> <dst>       # 创建硬链接（文件备份，指向同一 inode，不可跨文件系统）

# 删除软链接时目录末尾不要加 /
rm -rf <link_dst>
```

## 文件内容查看

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

## 查找与过滤 🔥

### find — 文件查找

```bash
find <dir> -name "*.cpp"      # 按文件名查找
find <dir> -type f             # 只查文件（f），目录（d）
find <dir> -mtime -1           # 最近 1 天修改的文件
find <dir> -size +100M         # 大于 100MB 的文件
find <dir> -user alice         # 按所有者查找
find <dir> -name "*.o" -delete # 查找并删除
```

### locate — 快速数据库查找

```bash
updatedb                      # 更新文件名数据库
locate <file>                 # 基于预建数据库快速查找
```

### grep — 文本搜索 🔥

```bash
grep <pattern> <file>         # 在文件中搜索
grep -n <pattern> <file>      # 显示匹配行号
grep -i <pattern> <file>      # 忽略大小写
grep -r <pattern> <dir>       # 递归搜索目录
grep -v <pattern> <file>      # 反向匹配（显示不包含的行）
grep -c <pattern> <file>      # 仅统计匹配行数

# 正则表达式
grep "^start" <file>          # 以 start 开头
grep "end$" <file>            # 以 end 结尾
grep "." <file>               # 匹配任意单个字符
grep "a*" <file>              # 匹配 a 出现 0 次或多次
```

### sort / uniq — 排序与去重

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

### awk — 按列处理 🔥

```bash
awk '{print $1}' <file>       # 输出第 1 列（默认空格分隔）
awk -F ',' '{print $1,$3}'    # 指定逗号分隔符，输出第 1、3 列
awk '{sum+=$1} END {print sum}' # 计算第 1 列总和

# 内置变量
  # $0：整行内容  $1~$N：第 N 列  $NF：最后一列
  # NR：当前行号  NF：当前行字段数
  # BEGIN{}：处理前执行  END{}：处理后执行
```

### sed — 流编辑器

```bash
sed 's/old/new/g' <file>      # 全局替换（不改原文件）
sed -i 's/old/new/g' <file>   # 直接修改文件（慎用）
sed '/pattern/d' <file>       # 删除匹配行
sed -n '5,10p' <file>         # 打印第 5-10 行
```

### cut / tr / xargs

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

## 压缩与解压

### tar 🔥

```bash
# 打包（.tar）
tar -cvf archive.tar <dir>       # 打包不压缩
tar -xvf archive.tar             # 解包

# 打包并压缩（.tar.gz）⭐ 最常用
tar -zcvf archive.tar.gz <dir>   # 打包 + gzip 压缩
tar -zxvf archive.tar.gz         # 解压
tar -zxvf archive.tar.gz -C /dst # 解压到指定目录
tar -ztvf archive.tar.gz         # 查看包内文件（不解压）

# 高压缩率（.tar.bz2）
tar -jcvf archive.tar.bz2 <dir>  # 打包 + bzip2 压缩
tar -jxvf archive.tar.bz2        # 解压
```

> **注意**：`-f` 必须紧跟压缩包名，放在最后。跨平台优先用 `.tar.gz` 或 `.zip`。

### gzip / bzip2 / zip

```bash
gzip <file>                    # 压缩单个文件（默认删除原文件）
gzip -k <file>                 # 压缩并保留原文件
gunzip <file>.gz               # 解压

zip -r archive.zip <dir>       # 递归压缩
unzip archive.zip              # 解压到当前目录
unzip archive.zip -d /dst      # 解压到指定目录
unzip -l archive.zip           # 只查看压缩包内容
```

## 系统信息

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

### 常用快捷键

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

## 重定向与管道

```bash
cmd > file           # 标准输出重定向（覆盖）
cmd >> file          # 标准输出重定向（追加）
cmd 2> err.log       # 标准错误重定向
cmd > out.log 2>&1   # 标准输出和错误合并重定向
cmd > /dev/null      # 丢弃所有输出

cmd1 | cmd2          # 管道：cmd1 的输出作为 cmd2 的输入
cmd | tee file       # 同时输出到终端和文件
```

## Shell 脚本基础

### 脚本结构与执行

```bash
#!/bin/bash                    # shebang，必须第一行
#!/usr/bin/env bash            # 更通用的写法

set -euo pipefail              # 严格模式：出错即停、未定义变量报错、管道失败报错
```

### 变量

```bash
# 变量定义（等号两边不能有空格）
name="world"
readonly year="2026"           # 只读变量
local tmp=$1                   # 局部变量（函数内使用）

# 使用变量
echo "Hello, ${name}!"         # 推荐加 {} 避免歧义
echo $name                     # 简单引用可省略 {}

# 命令替换
dt=$(date)                     # 推荐
dt2=`date`                     # 旧写法，不推荐
```

### 特殊变量

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

### 条件判断

```bash
# 字符串判断
[[ "$str" == "hello" ]]        # 相等
[[ -z "$str" ]]                # 为空
[[ -n "$str" ]]                # 非空

# 数值判断（推荐 (( ))）
(( a > 1 ))                    # >, >=, <, <=, ==, !=
(( a > b && a > c ))           # 支持 && 和 ||

# 兼容写法
[[ "$a" -eq "$b" ]]            # -eq, -ne, -gt, -lt, -ge, -le

# 文件判断
[[ -e "$path" ]]               # 存在（文件或目录）
[[ -f "$path" ]]               # 是普通文件
[[ -d "$path" ]]               # 是目录
[[ -x "$file" ]]               # 可执行
[[ -r "$file" ]]               # 可读
```

### 控制流程

```bash
# if
if (( $# < 2 )); then
    echo "Usage: $0 <arg1> <arg2>"
    exit 1
fi

# for
for i in "$@"; do
    echo "$i"
done

for (( i=0; i<=100; i++ )); do
    ((sum += i))
done

# while
while (( cnt <= 5 )); do
    echo "Count: $cnt"
    ((cnt++))
done

# case
case $1 in
    "start") echo "Starting...";;
    "stop")  echo "Stopping...";;
    *)       echo "Unknown: $1";;
esac
```

### 函数

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

### 数值运算

```bash
# 推荐：$(( )) — 仅支持整数
result=$(((2 + 3) * 4))

# expr — 兼容 POSIX，操作符前后必须有空格
result=$(expr 2 + 3)

# let — 直接赋值
let "result = (2 + 3) * 4"
```

### 输入输出

```bash
read -p "Enter name: " name           # 带提示符
read -t 10 -p "Enter number: " num    # 10 秒超时

# 单引号：原样输出，不解析变量
# 双引号：解析变量，保留空格
# 无引号：简单值，有空格会分词
```

> **工程要点**：
> - 脚本中始终使用 `set -euo pipefail` 及早暴露错误
> - 变量引用始终加双引号 `"$var"` 防止分词问题
> - `[[ ]]` 比 `[ ]` 功能更强，支持 `&&` `||` 和正则
> - `$(( ))` 是最推荐的整数运算方式

Shell 命令与脚本详解见 → [File System & Permissions (文件系统与权限)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/02-File%20System%20&%20Permissions%20(文件系统与权限).md) · [System Administration Basics (系统管理基础)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/03-System%20Administration%20Basics%20(系统管理基础).md)
