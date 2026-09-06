---
tags:
  - linux/basics
status: 🌱
---

# 03-System Administration Basics (系统管理基础)

> [!abstract] 核心考点：用户与组管理、磁盘管理、任务调度、服务管理 systemctl、进程管理与系统监控、日志查看

## 用户与组管理

### 创建与删除

```bash
# 创建用户
useradd alice                    # 创建用户（默认家目录 /home/alice）
useradd -d /data/alice alice     # 指定家目录
useradd -g dev -u 1001 alice     # 指定所属组和 UID
adduser alice                    # Ubuntu 交互式创建

# 删除用户
userdel alice                    # 删除用户（保留家目录）
userdel -r alice                 # 删除用户及家目录

# 创建/删除组
groupadd dev
groupdel dev
```

### 修改用户与组

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

### 用户切换

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

## 磁盘管理

### 设备命名

```bash
sdX~       # SCSI/SATA 硬盘（主流）
hdX~       # IDE 硬盘（旧）
# X = 盘号（a, b, c...），~ = 分区号

# 主分区 1-4，扩展分区 1-4，逻辑分区从 5 开始
```

### 查看磁盘

```bash
lsblk                     # 树结构查看分区
df -h                     # 查看磁盘挂载和使用情况
du -sh <dir>              # 查看目录大小
du -h --max-depth=1       # 查看一级子目录大小
```

### 分区与格式化

```bash
fdisk /dev/sdb            # 分区工具
  # n → p → [size]  创建主分区
  # w               保存并退出
  # d               删除分区
  # q               不保存退出

mkfs -t ext4 /dev/sdb1    # 格式化为 ext4
```

### 挂载

```bash
mount /dev/sdb1 /mnt/data     # 挂载分区
umount /dev/sdb1              # 卸载分区
mount -a                      # 验证挂载
```

### 开机自动挂载

```bash
blkid /dev/sdb1               # 查看 UUID
vim /etc/fstab                # 添加：
UUID=xxx /mnt/data ext4 defaults 0 0
```

---

## 进程管理

### 查看进程

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

### 进程信号

```bash
kill -l                   # 列出所有信号
kill -15 <PID>            # 发送 SIGTERM（优雅终止，默认）
kill -9 <PID>             # 发送 SIGKILL（强制终止，不可捕获）
killall nginx             # 按名称终止
pkill <name>              # 模糊匹配终止
pidof <name>              # 获取进程 PID
```

### 进程状态

| 状态 | 含义 |
|------|------|
| R | 运行中或在运行队列中 |
| S | 可中断睡眠（等待事件） |
| D | 不可中断睡眠（IO 等待，杀不掉）⚠️ |
| Z | 僵尸态（父进程未 wait 回收） |
| T | 停止 / 被跟踪 |

---

## 系统资源监控

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

## 网络管理

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

## 服务管理（systemd）

```bash
systemctl start nginx     # 启动服务
systemctl stop nginx      # 停止
systemctl restart nginx   # 重启
systemctl status nginx    # 查看状态
systemctl enable nginx    # 开机自启
systemctl disable nginx   # 取消自启
```

---

## 任务调度

### crontab 定时任务 🔥

```bash
systemctl start crond     # 启动 cron 守护进程
systemctl enable cron     # 设置开机自启

crontab -e                # 编辑定时任务
crontab -l                # 查看
crontab -r                # 清空
```

**格式：** `分 时 日 月 周 命令`

```bash
# ┌──────── 分 (0-59)
# │ ┌────── 时 (0-23)
# │       │ ┌──── 日 (1-31)
# │ │     │ ┌── 月 (1-12)
# │ │ │   │ ┌ 周 (0-7, 0/7=周日)
# │ │ │ │ │
  * * * * * <command>

30 2 * * * /backup.sh           # 每天凌晨 2:30 备份
*/5 * * * * /check.sh           # 每 5 分钟检查
0 9 * * 1-5 /report.sh          # 工作日早 9 点
0 0 1 * * /monthly.sh           # 每月 1 号零点
```

### at 一次性任务

```bash
systemctl start atd       # 启动 atd 守护进程

at 14:30                   # 在 14:30 执行（输入命令后 Ctrl+D 结束）
at now + 1 hour            # 一小时后
atq                        # 查看待执行任务
atrm <ID>                  # 删除指定任务
```

---

## 日志查看

### journalctl

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

### tail / grep

```bash
tail -f /var/log/syslog       # 实时监控日志
grep "error" /var/log/auth.log  # 过滤错误信息
```

系统管理基础见 → [Shell & Basic Commands (命令行与Shell编程)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md) · [File System & Permissions (文件系统与权限)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/02-File%20System%20&%20Permissions%20(文件系统与权限).md)
