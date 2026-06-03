> **核心考点**：用户与组管理、服务管理 systemctl、进程管理 ps/top/kill、系统监控常用命令
### 增删用户 & 组

- 创建用户 & 组
	- `useradd [user_name]`：创建新用户（默认家目录：`/home/[user_name]` ）
	- `useradd -d [home_path] [user_name]`： 指定自定义家目录路径
	- `useradd -g [group_name] -u [user_name]`： 指定所属用户组/UID
	- `adduser [user_name]`:  *Ubuntu* 交互式创建用户命令
	- `groupadd [group_name]`：创建新用户组

- 删除用户 & 组
	- `userdel [user_name]`：删除用户（保留家目录，无法登录）
	- `userdel -r [user_name]`：删除用户（删除家目录，彻底清除）
	- `groupdel [group_name]` ：删除用户组

### 修改用户 & 组

- 修改用户
	- `passwd`：修改当前用户密码
	- `passwd [user_name]`：修改用户密码
	- `usermod -l [new_user_name] [old_user_name]`：修改用户名
	- `usermod -d [home_path] -m [user_name]`：修改家目录并移动文件
		- 提醒：新家目录需要有足够权限访问
		
	- `usermod -g [group_name] [user_name]`：修改用户主组
	- `usermod -aG [group_name] [user_name]`：添加附加组
	- `gpasswd -d [user_name] [group_name]`：取消附加组
	- `usermod -G "" [user_name]`：一次性清空所有附加组 

- 修改组
	- `groupmod -n [new_group_name] [old_group_name]`：修改组名
	- `groupmod -g [new_GID] [group_name]`：修改组 GID
	- `groupmod -f [new_GID] [group_name]`：强制修改 GID（即使 GID 已被占用）

### 用户切换

- 切换
	- `su [user_name]`：切换用户身份，不切换环境
	- `su - [user_name]`：切换用户身份与环境
	- `newgrp`：临时切换用户的有效组（新文件的默认所属组归属）

- 回退
	- `exit`：退回上一级用户
	- `logout`：退回最初登录系统的用户

### 信息查看

- `id`：查看当前用户 `UID GID 组`
- `id [user_name]`：查看用户 `UID GID 组`

- `Whoami`：查看当前有效用户身份
- `Who am i`：查看最初登录系统的用户身份
- `who`：查看所有当前登录系统的用户
- `w`：`who` 的增强版，额外显示用户的操作、负载等信息

- `cat /etc/group`：查看系统所有组信息
# 权限管理（）

### rwx 权限本质

- `ls -l`：查看文件类型、权限位、所有者、所属组
	- 文件 `-` 、目录 `d` 、链接 `l` 、字符设备 `c` 、块设备 `d` ``
	- 权限位由三组 `rwx` 组成：对应所有者、所属组、其他用户
	- 数量
		- 文件：硬链接数（默认初始1）
		- 目录：子目录数+文件数
	- 占用
		- 普通文件：实际占用字节
		- 目录：目录项元数据大小，固定 4096 字节（不代表目录内文件总大小）

- `rwx` 权限
	- 对于文件
		- `r`：`cat`、`less` 查看文件内容
		- `w`：`vim` 修改
		- `x`：执行该文件
			
	- 对于目录 
		- `r`：`ls` 查看文件名，不支持 `ls -l` 查看文件详情
		- `w`：创建、删除、重命名、移动 文件 / 子目录
		- `x`：`cd` 进入目录、访问目录内文件
		- `rx`：`ls -l` 查看文件详情

### chmod / chown / chgrp

- 修改文件权限
1. 所有者与所属组
	- `chown [user_name]:[group_name] [file_name/dir_name]`
	- `chown [user_name] [file_name/dir_name]`
	- `chgrp [group_name] [file_name/dir_name]`
		- 对目录修改：`chown -R [] [dir_name]`（递归操作文件和子目录）

2. 符号法修改文件权限
	- `chmod [用户范围][操作符][权限符] [file_name/dir_name]`
	- `u`  `g`  `o`  `a`
	- `+` 添加、`-` 移除、`=` 设置

3. 数字法修改文件权限
	- `chmod 755 [file_name]`：（三组权限值的各自和）
	- `r` = 4  `w` = 2   `x` = 1   `-` = 0

### umask 默认权限（🔥）

- 类似「权限掩码」，控制新创建文件 / 目录的默认权限
	- 文件默认最大权限：`666`
	- 目录默认最大权限：`777`
	- 实际权限 = 默认最大权限 - **umask**

- `umask`：查看当前 umask 值
- `umask 022`：临时修改 umask（重启失效）
	- 文件：`666 - 022 = 644`（rw-r--r--）
	- 目录：`777 - 022 = 755`（rwxr-xr-x）

- 永久生效：在 `~/.bashrc` 或 `/etc/profile` 中配置 `umask`

### 特殊权限（🔥）

SUID/SGID/ 粘滞位（SGID 用于目录，让新文件继承目录的所属组）

# 磁盘管理

- `hdx~`：**IDE** 硬盘
- `sdx~`：**SCSI** 硬盘（主流）
	- `x`：盘号
	- `~`：分区号

- 硬盘分为 主分区 + 扩展分区（共 4 个）
	- 主分区 → 直接挂载
	- 扩展分区 → 逻辑分区 → 挂载
- 主分区：编号 1-4
- 扩展分区：编号 1-4
- 逻辑分区：编号 5 开始，数量无限

### df / du 磁盘查询

- `lsblk`：树结构查看磁盘分区情况
- `df`：查看设备磁盘挂载、使用情况
- `du [dir_name]`：查看目录磁盘占用情况
	- `-h`：（ K / M / G）
	- `-a`：（所有，默认只显示子目录）
	- `-s`：（不展开子目录详情）
	- `-c`：（列出明细，额外输出最终汇总值）
	- `--max-depth=[num]`：（子目录的递归深度）

### 分区与格式化

- `fdisk /dev/sdb`（创建分区）
	- `n` → `p` →`[size]`：添加
	- `w`：保存分区表（关键！）
	- `q`：不保存退出
	- `d`：删除分区

- `mkfs -t ext4 /dev/sdb1`（格式化）

### mount / umount 挂载

- `mount /dev/sdb1 [dir_name]`：分区挂载
- `umount /dev/sdb1`：分区卸载
- `mount -a`：验证挂载情况

### 开机自动挂载

- `vim /etc/fstab`：打开配置文件
- `blkid /dev/sdb1`：查看 UUID
- 添加 `/dev/sdb1 [dir_path] ext4 defaults 0 0`



# 任务调度

### crontab 定时任务（🔥）

- `systemctl start crond`：启动守护进程
- `sudo systemctl enable cron`：设置关闭

- `crontab -e`：进入编辑
- `crontab -l`：查看
- `crontab -r`：清空

- `* * * * * [命令/脚本路径]`：设置定时任务
	- 分 - 时 - 日 - 月 - 周（0 / 7  均为周日）
	- `*`：任意时间点
	- `,`：分隔多个时间点
	- `-`：连续时间段
	- `*/[num]`：每隔 `[num]` 时间单位

### at 一次性任务

- `systemctl start atd`：启动守护进程

- `atq`：查看
- `atrm [ID]`：指定删除

- `at [时间]`：提交定时任务
	- `-m`：任务完成时邮件提醒
	- `-f [file_name]`：从指定文件读取任务
	- `hh:mm`：具体时间点
	- `[am/pm]`：上 / 下午
	- `midnight` / `noon` 、`today` / `tomorrow`
	- `yy-mm-dd`：年 - 月 - 日
	- `now + [num] [time_units]`
		- `minutes`、`hours`、`days`、`weeks`

- 按两次 `Ctrl + D` 结束输入



### journalctl / tail / grep

```shell
journalctl                  # 查看全部日志
journalctl -f               # 实时跟踪日志
journalctl -n 3             # 查看最近 3 条日志

journalctl -u sshd          # 查看 SSH 服务日志

journalctl --since "1 hour ago"
journalctl --since "today"
journalctl --since "2026-04-01" --until "2026-04-02"

journalctl -p err           # 只看错误级别日志（报错排查首选）

journalctl | grep sshd      # 过滤出包含 sshd 的日志

journalctl -p err | grep "error" # 错误日志里找error信息

journalctl -k               # 查看内核日志（替代 dmesg）
```

- `journalctl -u 服务名`：查看指定服务日志
- `tail -f /var/log/syslog`：实时监控日志，排查定时任务、服务启动问题
- `grep "error" /var/log/auth.log`：过滤错误信息，快速定位问题

## 进程查看

```bash
ps aux               # 查看所有进程
ps aux | grep nginx  # 过滤进程
top                  # 动态查看（M 按内存排序，P 按 CPU）
htop                 # 更友好的 top（需安装）
pstree -p            # 以树形展示进程父子关系
```

## 系统资源

```bash
free -h              # 内存使用情况
df -h                # 磁盘空间使用
du -sh dir/          # 目录大小
lscpu                # CPU 信息
uname -a             # 内核版本和系统信息
uptime               # 系统运行时间和负载
vmstat 1             # 每秒刷新：CPU/内存/IO 统计
iostat -x 1          # 磁盘 IO 统计
```

## 网络管理

```bash
ip addr              # 查看网卡和 IP（替代 ifconfig）
ip route             # 查看路由表
ss -tlnp             # 查看监听端口和对应进程（替代 netstat）
ping host            # 连通测试
curl -v URL          # HTTP 请求（-v 显示详情）
wget URL             # 下载文件
traceroute host      # 路由追踪
```

## 用户与权限管理

```bash
useradd -m alice     # 创建用户（-m 创建家目录）
passwd alice         # 设置密码
usermod -aG sudo alice  # 将 alice 加入 sudo 组
su - alice           # 切换到 alice
sudo cmd             # 以 root 权限执行
visudo               # 安全编辑 sudoers 文件
```

## 服务管理（systemd）

```bash
systemctl start nginx     # 启动服务
systemctl stop nginx      # 停止
systemctl restart nginx   # 重启
systemctl status nginx    # 查看状态
systemctl enable nginx    # 开机自启
systemctl disable nginx   # 取消自启
journalctl -u nginx -f    # 查看服务日志（实时）
```