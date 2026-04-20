

# 1. Shell 基础语法
### 什么是 Shell（bash/zsh）

- `Shell` 是 **用户与操作系统内核交互的命令行解释器**

- 命令本质是操作四大核心对象：
    - 文件系统（目录 / 文件）
    - 进程
    - 权限
    - 输入输出流

### 执行流程

### 环境变量（🔥）

- `set`：查看所有系统环境变量
- `echo $[变量名]`：查看对应系统环境变量
	- `$PATH`：命令搜索路径
	- `$HOME`：当前用户家目录
	- `$USER`：登录用户名
	- `$PWD`：当前所在目录
	- `$SHELL`：当前使用的 shell
	- `$LANG`：系统语言
	- `$HOSTNAME`：主机名

# 2. Shell Script（核心🔥）

### 脚本结构（#!）

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

### 变量

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

### 数值运算（expr / $(( )) / let）

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
 
### 输入输出（read / echo）

- `read -p "请输入名字：" name`：指定提示符
- `read -t 10 -p "输入num1=" num1`：额外指定等待时间 (s)

### 条件判断与测试

#### `[[ ]]` / `(( ))`

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

#### if / elif / else

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

#### case

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

### 循环（for / while）

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

### 函数

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

# 3. Text Processing（🔥重点）
### grep（必须精通🔥）
### awk（重点🔥）
### sed（了解→进阶）
### sort / uniq

# 4. Pipe & Redirect（核心🔥）

### |
### >
### >>
### 2>

# 5. 常用工具（工程能力🔥）

### xargs

### wc

[01-命令体系 wc](01-Fundamentals（基础）/01-命令体系.md)

### cut
### tr

# 6. 实战脚本（必须写🔥）
### 日志分析脚本
### 批量处理文件
### 自动编译脚本