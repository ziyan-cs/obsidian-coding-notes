

# 0. 核心（进程模型）

- 本质
>项目.cpp → 程序.exe → 进程

- 生命周期
>创建 → 运行 → 阻塞 → 终止 → 回收

# 1. fork（🔥）

- `fork()`：创建子进程
  - 复制父进程的地址空间（写时复制）
  - 返回值：父进程返回子进程PID，子进程返回0，失败返回-1
- 父子进程的区别：PID、PPID、PCB资源、文件描述符偏移量

# 2. exec

# 3. wait / waitpid

# 4. 进程查看与控制（🔥）

### ps

- `ps`：查看进程
	- `-aux`：（所有进程）
	- `USER`-`PID`-`PPID`-`%CPU`/`%MEM`-`VSZ`/`RSS`
	- `STAT`：进程状态
	- `COMMAND (CMD)`：程序所在路径
	- `-ef`：（含PPID父进程的所有进程）
- `pstree`：查看进程树
	- `-p`：显示 `PID`
	- `-u`：显示进程数

### netstart / ss

- `netstat -tulnp`：查看所有 TCP/UDP 监听端口
	- `-t`：（TCP 协议）
	- `-u`：（UDP 协议）
	- `-l`：（只显示监听状态）
	- `-n`：（不解析域名，直接显示IP）
	- `-p`：（显示进程 PID /名称）
- `netstat -anp | grep :8080`：查看占用8080端口的进程

- `ss -tulnp`：查看所有 TCP/UDP 监听端口
- `ss -antp | grep :8080`：查看8080端口的连接状态

### kill

- `kill [PID]`：发送信号 (15)，终止进程
	- `-9`：（强制终止）
	- `-l`：（列出系统支持的所有信号）
- `pidof [process_name]` 获取进程名
- `killall [process_name]`：批量终止
- `pkill [process_name]`：模糊匹配，终止进程

# 5. 进程状态（🔥）

- **STAT**
	- `R` 运行态
	- `S` 可中断睡眠
	- `D` 不可中断睡眠
	- `T` 停止态
	- `Z` 僵尸态

- 常用信号与进程行为
	- `SIGINT(2)`
	- `SIGTERM(15)`
	- `SIGKILL(9)`
	- `SIGCHLD(17)`

# 6. 线程（pthread🔥）

# 7. 线程同步

### mutex

### semaphore