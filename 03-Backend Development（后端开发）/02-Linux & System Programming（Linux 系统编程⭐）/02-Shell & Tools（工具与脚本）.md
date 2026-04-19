

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

```bash
#!/bin/bash
# 首行指定解释器
:<<!
注释内容
!
```

### 变量

```bash
# 1. 普通变量
name="my_script"      # 可被 unset
readonly year="nihao" # 不可被 unset
A=`date`              # 反引号 看作命名
A=$(date)             # 等价
```

```bash
# 2. 位置参数变量
echo "$0"           # 该脚本名
echo "1=$1 2=$2"    # 接收.sh后面输入的参数
echo "所有参数=$@"    # 单独分开
echo "$*"            # 整合成一个字符串
echo "参数个数=$#"    # 接收的参数数
```

```bash
# 3. 预定义变量
$$ # 当前进程 PID
$! # 前一个后台进程的 PID
$? # 前一个进程的退出状态码 
   # 成功(0) 失败(非0)
```

```bash
# 4. 环境变量
export MY_VAR="hello"
echo $MY_VAR
```

### 数值运算（expr / $(( )) / let）

- `$((2+3))`
```bash
TEMP=`expr 2 + 3`
RES1=`expr $TEMP \* 4`
RES2=$((2+3-4))
RES3=$[2+3-4]
```
### 输入输出

```bash
# read：读取用户输入

read -p "请输入名字：" name
echo "你好，$name"

# 脚本参数：`$1`（第 1 个参数）、`$@`（所有参数）

echo "第1个参数: $1" 
echo "所有参数: $@"
```





### 条件判断（if）

```bash
if [ 条件 ]; then
  # 条件成立执行
elif [ 条件 ]; then
  # 其他条件成立执行
else
  # 所有条件不成立执行
fi
```

### 条件测试语法（`[]` / `[[]]`/ test）


### 循环（for / while）

```bash
for i in 1 2 3 4 5; do
  echo "数字: $i"
done
```

```bash
count=1
while [ $count -le 5 ]; do
  echo "计数: $count"
  count=$((count + 1))
done
```

### 函数

```bash
function 函数名() {
  # 函数体
  echo "函数执行"
}

# 调用函数
函数名
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