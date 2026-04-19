

# 0. 核心（进程模型）

- 本质
>项目.cpp → 程序.exe → 进程

- 生命周期
>创建 → 运行 → 阻塞 → 终止 → 回收

# 1. 进程创建
### fork（🔥）

- `fork()`：创建子进程
  - 复制父进程的地址空间（写时复制）
  - 返回值：父进程返回子进程PID，子进程返回0，失败返回-1
- 父子进程的区别：PID、PPID、PCB资源、文件描述符偏移量

### exec

### wait / waitpid

# 2. 进程查看与控制（🔥）

### ps

- `ps`：静态查看进程
	- `-aux`：BSD 风格，列名更友好
	- `-ef`：System V 风格，`PPID` 列更清晰
	- `-efL`：查看线程（LWP 轻量级进程ID）
	- `USER`-`PID`-`PPID`-`%CPU`/`%MEM`-`VSZ`/`RSS`
	- `STAT`：进程状态
	- `TTY`：完整终端名称
	- `COMMAND (CMD)`：启动程序所用命令和参数
- 常用
	- `ps -aux | grep`
	- `ps -efL`

- `pstree`：静态查看进程树
	- `-p [PID]`：（指定进程）
	- `-p`：（显示 `PID`）
	- `-u`：（显示进程数）

### top / htop

- `top`：动态监控进程
	- `-d [num]`：（指定刷新间隔，默认3秒）
	- `-i`：（不显示空闲进程）
	- `-p -PID`：（指定监控）
	- `VIRT`/`RES`/`SHR`
	- `PR`：进程优先级
		- 内核调度的优先级
		- （-20 最高，39 最低）
	- `NI`：进程谦让值
		- 用户设置的优先级偏移量
		- （-20 最高，19 最低）
- 快捷键
	- `P`：（按CPU使用率排序，默认）
	- `M`：（按内存占用率排序）
	- `N`：（按 `PID` 排序）
	- `k`：（终止指定进程）
	- `q`：（退出）

- `htop`：增强版特点与安装

### netstat / ss

- `netstat`
	- `-t`：（仅 TCP 协议）
	- `-u`：（仅 UDP 协议）
	- `-l`：（仅监听状态）
	- `-n`：（显示IP）
	- `-p`：（显示 PID / 进程名）
	- `-a`：（所有）

- `ss -s`：全局连接状态的参数
- `ss -tulnp`：正在监听的 TCP/UDP 端口
- `ss -antp`：所有状态的 TCP 端口
	- `state [listening|established|time-wait]`
		- （指定状态）
	- `src [端口号]`：（发起的连接）
	- `dst [端口号]`：（连接到的连接）
	- `sport = [端口号]`：（指定端口）
	- `| grep :[端口号]`（指定端口）
	- `pid [PID]`：（指定进程）

### kill

- `kill [PID]`：发送 `SIGTERM(15)` 信号
	- `-9`：发送 `SIGKILL(9)` 信号
- `pidof [process_name]` 获取进程名
- `killall [process_name]`：批量终止
- `pkill [process_name]`：模糊匹配，终止进程

# 3. 进程状态（🔥）

- **STAT**
	- `R` 运行中/可运行
	- `S` 可中断睡眠（等待事件）
	- `D` 不可中断睡眠（通常是IO等待，杀不掉）
	- `Z` 僵尸态（父进程没回收）
	- `T` 停止/被跟踪

- `kill -l`：列出系统支持的所有信号
- 常用信号与进程行为
	- `SIGINT(2)`
	- `SIGTERM(15)`
	- `SIGKILL(9)`
	- `SIGCHLD(17)`

# 6. 线程（pthread🔥）

# 7. 线程同步

### mutex

### semaphore