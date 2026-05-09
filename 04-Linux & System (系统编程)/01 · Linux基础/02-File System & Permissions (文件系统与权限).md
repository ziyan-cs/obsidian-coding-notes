

# 1. Vim 使用

### 1.基础操作

- 模式分类
	- 正常模式：移动光标、复制删除 
	- 插入模式：编辑输入内容
	- 命令模式：保存、退出、替换等

- 模式切换
	- `i`：进入插入模式
	- `:`：进入底行命令模式
	- `esc`：回退正常模式

- 光标移动
	- `w`：跳到下个单词开头
	- `b`：跳到上个单词开头
		
	- `0`：跳转到行首
	- `^`：跳转到行首
	- `$`：跳转到行尾
		
	- `gg`：跳到文件首行
	- `5gg`：跳到第 5 行
	- `Shift + G`：跳到文件尾行
	- `Ctrl + U`（上翻半页）
	- `Ctrl + D`（下翻半页）

- 内容操作
	- `dd`：剪切当前整行
	- `5dd`：剪切光标后的 5 行
	- `d$`：剪切从光标到行尾
	- `yy`：复制当前整行
	- `5yy`：复制光标后的 5 行
	- `p`：粘贴到光标后
	- `u`：撤销上一步操作
	- `Ctrl + R`：恢复撤销

- 编辑模式不能用 `Ctrl + S` ，会导致终端卡死
	- `Ctrl + Q` 恢复

- 查找
	- `:/[关键词]`：向下查找
		- `n`：跳转下一个匹配项
		- `Shift + N`：跳转上一个匹配项

- 替换
	- `:%s/old/new/g`：替换全局
	- `:s/old/new/g`：替换当前行
	- `:%s/old/new/gc`：每个替换并确认

- 行号设置
	- `:set nu`：设置文件行号
	- `:set nonu`：取消文件行号

- `:5`：光标跳到第 5 行

- 文件操作
	- `:w`：保存
	- `:q`：退出
	- `:q!`：强制退出不保存
	- `:wq`：保存并退出（ `zz` 快捷键）
	- `:w [file']`：另存为新文件 
	- `:e [file']`：打开另一个文件

### 2. 多文件与分屏

- `vim [file1] [file2]`
	- `:n` 下一个文件
	- `:N` 上一个文件
	- `:ls` 列出所有打开的文件
	- `:b [文件名/序号]` 直接切换到指定文件

- `vim -o file1 file2` / `:sp`：水平分屏
- `vim -O file1 file2` 或 `:vsp`：垂直分屏
- `Ctrl + w` 后按 `h/j/k/l` / 方向键：切换窗口
- `:q`：关闭当前窗口
- `:qa`：关闭所有窗口

### 3.  .vimrc🔥

- 配置文件路径：`~/.vimrc`

- 生效方式：保存后 `:source ~/.vimrc` 或重启 Vim

- `:noh`：去除语法高亮

```shell
# 基础配置
set number          # 显示行号
set tabstop=4       # Tab 宽度4
set shiftwidth=4    # 自动缩进宽度4
set expandtab       # Tab 转为空格
set autoindent      # 自动缩进
set smartindent     # 智能缩进
set hlsearch        # 搜索高亮
set incsearch       # 边输入边搜索
set encoding=utf-8  # 编码
```

### 环境变量🔥

- `set`：查看所有系统环境变量
- `echo $[变量名]`：查看对应系统环境变量
	- `$PATH`：命令搜索路径
	- `$HOME`：当前用户家目录
	- `$USER`：登录用户名
	- `$PWD`：当前所在目录
	- `$SHELL`：当前使用的 shell
	- `$LANG`：系统语言
	- `$HOSTNAME`：主机名

- 配置系统环境变量
	- `vim /etc/profile`
	- `export [变量名]=[dir_path]`
		- 定义软件根目录
	- `export PATH=$[变量名]/bin:$PATH`
		- 把命令目录加入 **PATH**
	- `source /etc/profile`
	- `echo $[变量名]`：验证

# 2. Shell 脚本结构🔥

### 脚本结构

- `#!/bin/bash`：必须是脚本的第一行
- `#!/usr/bin/env bash`：更通用、兼容性更好
	- 自动找到系统里的 `bash` 路径
- `set -euo pipefail`：严格模式
	- 提前发现变量未定义、命令错误等问题

- 赋值语句：`SUM=0` 不能有空格
- 算术运算：`expr 1 + 2` 必须有空格
- 条件判断：`[]` / `[[]]` / `test` 必须有
- 命令和参数之间：`echo "$@"`必须有
- 字符串拼接：变量和字符串之间不能有
	- `"helllo,$name"`

- 双引号：保留空格，解析变量，最常用
- 单引号：原样输出
- `$( )`：执行命令推荐用
- 不加引号：简单值，不能有空格

```bash
# 单行注释
:<<!
多行注释
!
```

### 变量定义与使用

```bash
# 1. 普通变量和只读变量
name="Alex"         # 可修改,可 unset
readonly year="20"  # 不可修改,不可 unset
dt=$(date)          # 命令替换复制
```

```bash
# 2. 位置参数变量
echo "$0"            # 脚本自身文件名
echo "$1 $2 ${10}"   # .sh 后面输入的参数
echo "$@"            # 单独分开 "a" "b" "c"
echo "$*"            # 整合成一个字符串 "abc"
echo "$#"    # 接收的参数数
```

```bash
# 3. 预定义变量
$$      # 当前脚本 / Shell 进程的 PID
$!      # 上一个后台进程的 PID
$?      # 上一个进程的退出状态码 (0) (非0)
```

```bash
# 4. 环境变量
MY_AGE=20
export MY_AGE
echo $MY_AGE
```

### 数值运算 expr / $(( )) / let

```bash
# 1. expr 兼容 POSIX
EMP=$(expr 2 + 3)
RES1=$(expr $TEMP \* 4) 
echo $RES1
```

```bash
# 2. $((...)) 强推！
RES2=$(((2+3)*4))
echo $RES2
# 仅支持整数运算,不支持浮点数
```

```bash
# 3. $[...] 过时
RES3=$[(2+3)*4]
echo $RES3
```

```bash
# 4. let 直接赋值
let "RES4=(2+3)*4"
echo $RES4
# 加上 " "
```
 
### 输入输出 read / echo

- `read -p "请输入名字：" name`：指定提示符
- `read -t 10 -p "输入num1=" num1`：额外指定等待时间 (s)

# 3. 控制流程

### `[ ]` / `[[ ]]` / `(( ))`

- `[[ ]]`：支持 `&& ||` 连写，不支持 `> >= < <=`
	- 用于字符串 / 文件判断
- `(( ))`：支持 `&& ||` 连写，也支持 `> >= < <=`
	- 用于数值 + 逻辑运算，以及 `if` `for` `while`
- `[ ]`：兼容 POSIX，`test` 命令的别名
	- 用于字符串 / 文件判断

- 字符串判断
	- `[[ "$str" == "hello" ]]`:相同返回真（0）
	- ``[[ -z "$str" ]]``：为空返回真（0）
	- ``[[ -n "$str" ]]``：非空返回真（0）

- 数值判断
	- `if (( a > 1 ));`
	- `if (( a > b && a > c ));`
	- `-eq` `-ne` `-gt` `-lt` `-ge` -`le`：兼容

- 文件 / 目录判断
	- `[[ -e "$path" ]]`：文件 / 目录是否存在
	- `[[ -f "$file" ]]`：文件是否存在
	- `[[ -d "$dir" ]]`：目录是否存在
	- `[[ -x "$file" ]]`：判断文件权限

### if / elif / else

```bash
#!/bin/bash
if (( $# < 2 )); then
	echo "用法：$0 <数字1> <数字2>" 
	exit 1 
fi

if (( $1 >= $2 )); then
	echo "$1 大于等于 $2"
else
    echo "$1 小于 $2"
fi
```

### case 分支

```bash
#!/bin/bash
case $1 in
"1")
echo "Monday"
;;
"2")
echo "Tuesday"
;;
"*")
echo "Others..."
;;
esca
```

### for / while 循环

```bash
#!/bin/bash
for i in "$@"; do
	echo "$i"
done
```

```bash
#!/bin/bash
SUM=0
for (( i=0; i<=100; i++)); do
	((SUM += i))
done
echo "$SUM"
```

```bash
#!/bin/bash
CNT=1
while (( CNT <= 5 )); do
	echo "计数: $CNT"
	((CNT++))
done
```

### 函数定义与调用

```bash
#!/bin/bash
function put() {
	echo "求两数之和"
}
function add() {
    local a=$1      # local 定义局部变量
    local b=$2
    echo $((a + b)) # 有返回值
}
RES=$(add 3 5)
put
echo "3 + 5 = $RES"
```

# 4. 高级工具🔥

### 管道 |

- `|`：前标准输出作为后标准输入，只传正常输出
- `ps aux | grep sshd`
- `netstat -tulpn | grep :80`
- `tail -f /var/log/syslog | grep "error"

### 重定向：> / >> / 2> / &>


### 文本工具实战

- `grep` / `awk` / `sed` / `sort` / `uniq` 组合使用


## Linux 文件系统层级

```
/
├── bin/     → 基本命令（ls, cp...）
├── sbin/    → 系统命令（root 用）
├── etc/     → 配置文件
├── home/    → 用户家目录
├── var/     → 可变数据（日志、缓存）
├── tmp/     → 临时文件（重启清空）
├── proc/    → 内核与进程信息（虚拟文件系统）
├── dev/     → 设备文件
├── lib/     → 共享库
└── usr/     → 用户程序
```

## 权限模型

```
-rwxr-xr--  1  alice  dev  4096  May 1  main.cpp
 ↑↑↑↑↑↑↑↑↑      ↑     ↑
 │││└──┘└──┘    用户   组
 │││ 组权限 其他权限
 ││└─ 用户权限（rwx）
 │└─ 文件类型（- 普通文件，d 目录，l 软链接）
 └─ 第1位
```

|字符|含义|数字|
|---|---|---|
|r|读|4|
|w|写|2|
|x|执行|1|
|-|无权限|0|

```bash
chmod 755 file        # rwxr-xr-x（所有者全权，其他人只读执行）
chmod u+x file        # 给所有者加执行权限
chmod go-w file       # 去掉组和其他人的写权限
chown alice:dev file  # 修改所有者和所属组
chown -R alice dir/   # 递归修改

umask 022             # 默认文件创建掩码（新文件权限 = 666-022=644）
```

## 特殊权限位

```bash
chmod u+s file    # SetUID：以文件所有者身份执行（如 /usr/bin/passwd）
chmod g+s dir/    # SetGID：目录下新文件继承目录的组
chmod +t dir/     # Sticky bit：只有文件所有者才能删自己的文件（如 /tmp）
```

## inode

每个文件有唯一 inode，存储文件元数据（权限、大小、时间戳、数据块位置），**不包含文件名**（文件名存在目录项中）：

```bash
ls -i file           # 查看 inode 号
stat file            # 查看完整元数据
df -i                # 查看 inode 使用情况（inode 用完无法创建新文件）
```

**硬链接 vs 软链接：**

| |硬链接|软链接|
|---|---|---|
|指向|inode|路径字符串|
|跨文件系统|❌|✅|
|原文件删除后|仍可访问|链接失效|
|目录|不支持|支持|