---
study_stage: backlog
---

> [!summary] 核心摘要
>
> 系统管理的主线是“资源—进程—服务—日志”：先用指标确认 CPU、内存、磁盘或网络瓶颈，再定位进程与服务状态，最后用日志和复现命令闭环。不要在没有证据时直接重启或调参。
>

> [!note] 本节重点：用户与组管理、磁盘管理、任务调度、服务管理 systemctl、进程管理与系统监控、日志查看

# 账户、用户组与权限

## 创建与删除

```bash
useradd --create-home alice      # 显式创建家目录；默认行为取决于发行版配置
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

# 存储设备与文件系统

## 设备命名

```bash
/dev/sdX   # 某些 SATA/SCSI/USB 块设备，字母会变化
/dev/nvme0n1 # NVMe 设备示例；实际名称以 lsblk 输出为准

```

## 查看磁盘

```bash
lsblk                     # 树结构查看分区
df -h                     # 查看磁盘挂载和使用情况
du -sh <dir>              # 查看目录大小
du -h --max-depth=1       # 查看一级子目录大小
```

## 分区与格式化

> [!danger] 下列命令会改分区表或覆盖文件系统。`/dev/sdb` 只是示意，**不要照抄执行**。先用 `lsblk -f`、`findmnt`、`blkid` 确认设备、挂载关系和备份/恢复方案，并在可丢弃的虚拟磁盘中练习。

```bash
fdisk /dev/sdb            # 仅示意：实际目标必须先核实
  # n → p → [size]  创建主分区
  # w               保存并退出
  # d               删除分区
  # q               不保存退出

mkfs -t ext4 /dev/sdb1    # 仅示意：将覆盖目标分区的文件系统
```

## 挂载

```bash
mount /dev/sdb1 /mnt/data     # 挂载分区
umount /dev/sdb1              # 卸载分区
findmnt --verify --verbose     # 检查 fstab 配置；仍需在测试环境验证实际挂载
```

## 开机自动挂载

```bash
blkid /dev/sdb1               # 查看 UUID
vim /etc/fstab                # 添加：
UUID=xxx /mnt/data ext4 defaults 0 0
```

---

# 进程与资源观测

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

# 网络与服务管理
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

# 自动化任务与日志
## 任务调度

### crontab 定时任务 🔥

```bash
systemctl status cron     # Debian/Ubuntu 常见服务名；其他发行版可能为 crond

crontab -e                # 编辑定时任务
crontab -l                # 查看
crontab -r                # 删除当前用户的整张 crontab，勿作日常清理
```

**格式：** `分 时 日 月 周 命令`

```bash
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

## 系统管理的诊断主线

系统管理不是命令记忆比赛。面对“服务不可用、机器变慢、磁盘告警”时，先构造证据链：

1. **现象与影响面**：何时开始，哪些请求、主机、用户受影响。
2. **服务状态**：进程是否存在，systemd 单元为何进入当前状态。
3. **资源状态**：CPU、内存、文件系统、文件描述符、网络连接是否成为约束。
4. **近期变化**：部署、配置、证书、依赖、容量是否发生变化。
5. **假设与验证**：每次只验证一个可证伪假设，记录命令、时间范围和结果。

```text
client symptom
   -> DNS / route / firewall
   -> listening socket
   -> process and systemd unit
   -> application log
   -> downstream dependency
   -> host resource pressure
```

## 身份、权限与文件系统

权限判断同时涉及有效 UID/GID、目录每一级的执行权限、ACL、挂载选项和安全模块。看到 `Permission denied` 不要直接 `chmod 777`；先用 `namei -l path`、`id`、`getfacl` 和挂载信息定位哪一层拒绝访问。

创建用户时显式表达意图：

```bash
sudo useradd --create-home --shell /bin/bash alice
sudo passwd alice
```

`useradd` 是否默认创建家目录取决于发行版和 `/etc/login.defs` 等配置，不能假设所有系统一致。服务账号通常应使用不可登录 shell，并只获得最小目录权限。

磁盘操作分清三个层次：块设备、文件系统、挂载点。`fdisk`、`mkfs` 会改变分区或文件系统，执行前必须确认设备标识、备份与恢复方案；不要根据 `/dev/sdX` 的字母顺序猜目标盘。

```bash
lsblk -f
findmnt
df -hT
du -xhd1 /var | sort -h
```

`df` 观察文件系统，`du` 汇总目录树。两者差异大时，检查已删除但仍被进程打开的文件（`lsof +L1`）、挂载覆盖和保留块。

## systemd 与日志证据

`systemctl status` 是摘要，不是完整根因。继续查看单元定义、依赖和指定时间窗的日志：

```bash
systemctl cat example.service
systemctl show example.service -p ActiveState -p SubState -p ExecMainStatus
journalctl -u example.service --since '30 min ago' --no-pager
```

修改 unit 后先 `systemd-analyze verify`，再 `daemon-reload`；上线前明确 `restart` 是否允许中断，以及失败后的回滚步骤。日志中出现密钥、令牌或个人信息时，应先处理泄露风险再共享证据。

## 容量与故障处置

“CPU 100%”不是根因：区分用户态、内核态、I/O wait、steal time，并定位到进程与线程。内存也要区分工作集、page cache、swap、OOM 事件和 cgroup 限额。先保存证据，再做会改变现场的重启或清理。

安全的处置记录至少包含：

- 时间线与影响范围；
- 执行过的命令和配置变更；
- 变更前后的指标或日志；
- 回滚条件与实际结果；
- 后续修复负责人和验证方式。

## 实战验收

在虚拟机中创建一个故意启动失败的 systemd 服务，分别制造配置错误、端口占用和权限错误。要求只依靠状态、日志、套接字与权限证据定位问题；修复后验证重启、开机启动、日志轮转和资源限制，而不是只证明“当前能运行”。

## 参考资料

- [systemd Documentation](https://systemd.io/)
- [Linux man-pages project](https://www.kernel.org/doc/man-pages/)
- [Filesystem Hierarchy Standard](https://refspecs.linuxfoundation.org/FHS_3.0/fhs/index.html)
