


# 1. 用户 & 组

- 组：用户的集合，用于批量权限管理
	- 主组 `UID`：默认身份组，用户只有 1 个，创建文件时的默认所属组
	- 附加组 `GID`：额外权限组，用户可以有多个，仅用于给用户自身额外的文件权限

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
# 2. 权限管理（🔥）

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


# 3. 磁盘管理

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

- `df`：查看设备磁盘挂载、使用情况
- `du [dir_name]`：查看目录磁盘占用情况
	- `-h`：（ K / M / G）
	- `-a`：（所有，默认只显示子目录）
	- `-s`：（不展开子目录详情）
	- `-c`：（列出明细，额外输出最终汇总值）
	- `--max-depth=[num]`：（子目录的递归深度）

### 分区与格式化

- `fdisk /dev/sdb`（创建分区）
	- `n` → `p`：添加
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


# 4. 软件包管理

### Ubuntu/Debian 系

- `sudo apt update`：更新本地软件源索引
- `sudo apt upgrade`：升级所有可更新软件
- `sudo apt update && sudo apt upgrade -y`：一键更新系统
- `apt search [关键词]`：搜索可用软件包

- `apt list --installed`：查看已安装软件
- `sudo apt install [软件名]`：安装软件
- `sudo apt remove [软件名]`：卸载软件（保留配置）
- `sudo apt purge [软件名]`：彻底卸载软件（删除配置）
- `sudo apt autoremove`：卸载不再需要的依赖包（清理系统）

- `sudo dpkg -i [包名].deb`：本地安装 **deb** 包
- `sudo apt -f install`：修复 **dpkg** 安装后的依赖问题
- `apt show [软件名]`：查看软件包详细信息（版本、依赖、描述）

### CentOS/RHEL 系

- `rpm`：底层软件包管理工具，对应 Ubuntu 的 `.deb`
- `yum / dnf`：上层包管理器，对应 Ubuntu 的 `apt`
- `yum install [软件名]`：CentOS 安装软件命令（对应 `apt install`）

### 手动安装

- 下载最新压缩包
- `Xftp` 传输到 /opt 或 /usr/local
- `tar -zxvf [包名]`：解压
- `mv [src] [dst]`：移动到安装目录
- `vim /etc/profile`：配置环境变量
- `source /etc/profile`：生效
- `xxx --version`：验证
- 编写 systemd 服务（如需开机自启）

# 5. 系统配置文件（🔥）

### 用户与权限

- **/etc/passwd**   用户信息库
	- 系统所有用户基本信息
	- `用户名:x:UID:GID:注释:家目录:登录Shell`

- **/etc/shadow**   用户密码库
	- 用户密码的加密信息及安全策略
	- 仅 **root** 可读写

- **/etc/group**   组信息库
	- 系统所有用户组的信息
	- `组名:密码占位符:GID:组成员列表`

- **/etc/gshadow**   组密码库
	- 用户组的加密密码、组管理员等信息，
	- 仅 **root** 可读写

### 网络与主机

- **/etc/hosts**   静态域名映射
	- 手动配置 IP 与主机名的对应关系
	- 本地解析优先于 DNS，常用于集群按主机名互访

- **/etc/resolv.conf**   DNS 客户端配置
	- 指定系统使用的 DNS 服务器地址
	- `nameserver 8.8.8.8`

- **/etc/hostname**   主机名配置文件
	- 存储系统主机名，重启生效
	- 使用 `hostnamectl set-hostname` 临时 / 永久修改

- **/etc/netplan/**   Ubuntu 网络配置目录

- 现代 Ubuntu 系统用 netplan 管理网卡 IP、网关、DNS

### 系统启动与服务

- **/etc/profile**   系统级全局环境变量配置
	- 所有用户登录 Shell 时加载的全局配置
	- 常用于配置 PATH、系统环境变量
	- 修改后 `source /etc/profile` 生效

- **/etc/bash.bashrc**   全局 Bash 配置
	- 所有用户打开 Bash 终端时加载
	- 常用于配置别名、命令提示符

- **/etc/systemd/system/**   系统服务配置目录
	- 存放自定义 systemd 服务文件（ `.service` ）
	- 控制服务开机自启、启动顺序、依赖关系

### 定时任务与日志

- **/etc/crontab**   系统级定时任务配置
	- 系统全局 crontab 配置文件
	- 所有用户都能看到的系统定时任务

- **/etc/logrotate.conf**   日志轮转主配置
	- 控制系统日志的切割、压缩、保留周期
	- `/etc/logrotate.d/` 目录存放各服务的单独配置

### SSH 服务

- **/etc/ssh/sshd_config**   SSH 服务端配置文件
	- 控制 SSH 登录端口、认证方式、root 登录权限
	- 生产环境常用修改：`Port 2222`、`PermitRootLogin no`

- **/etc/ssh/ssh_config**   SSH 客户端全局配置
	- 控制本机 `ssh` 命令的默认行为（默认端口、超时、别名）

### 其他核心配置

- **/etc/sudoers**   sudo 权限配置文件
	- 控制哪些用户 / 组可以使用 `sudo` 提权
	- 推荐用 `visudo` 命令编辑

- **/etc/fstab**   文件系统挂载表
	- 系统开机时自动挂载的磁盘分区配置
	- `设备路径 挂载点 文件系统类型 挂载选项 备份频率 自检顺序`
	- 如 `/dev/sdb1 /data ext4 defaults 0 0`

- **/etc/hosts.allow & /etc/hosts.deny**   TCP 访问控制
	- 控制哪些 IP 可以访问本机服务（如 SSH）
	- 简单的防火墙替代方案，常用于限制 SSH 登录来源

# 6. 服务与防火墙管理

### service 传统命令

- `service [process_name] [..]`：
	- `statu`：（查看服务状态）
	- `stop`：（停止）
	- `start`：（启动）
	- `restart`：（重启）
	- `reload`：（重新加载配置）

### systemctl 服务控制（🔥）

- `systemctl [..] [process_name]`：
	- `status`：（查看服务状态）
	- `stop`：（停止）
	- `start`：（启动）
	- `restart`：（重启）
	- `reload`：（重新加载配置）
	- `mask`：彻底锁定，禁止启动
	- `unmask`：解除锁定

- `systemctl get-default`：查看运行级别
	-  `runlevel`；传统使用
- `systemctl set-default multi-user.target`
	- 设置开机默认 纯字符多用户 ( **3** )
- `systemctl set-default graphical.target`
	- 设置开机默认 图形桌面 ( **5** )

- `systemctl status`：系统的整体运行状态摘要
- `systemctl list-units --type=service --all`
	- 查看运行的服务
- `systemctl list-units --type=service --state=running`
	- 查看全部服务
- `systemctl list-unit-files --type=service`
	- 查看异常、启动失败的服务

### 服务开机自启

- `systemctl [..] [process_name]`
	- `enable`：服务开机自启
	- `disable`：服务关闭开机自启
	- `is-enabled`：查询服务状态

### 防火墙与端口管理

- `telnet [IP] [端口/协议]` 
	- 测试端口状态
- `firewall-cmd --permanent --add-port=[端口/协议]`
	- 开放端口
- `firewall-cmd --permanent --remove-port=[端口/协议]`
	- 关闭端口
- `firewall-cmd --reload`
	- 重载生效
- `firewall-cmd --query-port=[端口/协议]`
	- 查询端口状态

# 7. 系统基础运维

### 运行级别与目标

- ( **0** )、( **6** ) 开机后重复关机 / 重启
- ( **1** ) 单用户，用于 root 密码找回、系统修复
- ( **2** ) 多用户无网络服务
- ( **3** ) 多用户有网络服务
- ( **5** ) 图形化界面
- 服务器一般使用 ( **3** )，桌面版使用 ( **5** )
- `init [num]`：（临时切换）

### 静态 IP 配置

- **CentOS 步骤**
	- 1. `sudo vim /etc/sysconfig/network-scripts/ifcfg-ens33`
	- 2. 写入静态 IP 配置
	- 3. `sudo systemctl restart network`：重启网络服务
	- 4. 验证配置
		- `ipconfig`
		- `ping 192.168.200.2 -c 3`：网关连通性
		- `ping baidu.com`：外网连通性

```cpp
// 静态 IP 配置

# 1. 关键模式：从 dhcp 改为 static
BOOTPROTO=static
# 2. 静态 IP / 子网掩码
IPADDR=192.168.200.130
NETMASK=255.255.255.0
# 3. 网关    
GATEWAY=192.168.200.2
# 4. DNS / 备用 DNS
DNS1=192.168.200.2
DNS2=114.114.114.114
```

- **Ubuntu 步骤**
	- 1. `sudo vim /etc/netplan/01-network-manager-all.yaml`
	- 2. 写入静态 IP 配置
		- 缩进必须用空格，冒号后必须跟空格
	- 3. `sudo netplan applysss`：重启网络服务
	- 4. 验证配置
		- `ip a`
		- `ping 192.168.200.2`：网关连通性
		- `ping baidu.com`：外网连通性

```cpp
// 静态 IP 配置

network:
  ethernets:
    ens33:
      # 1. 静态IP/掩码（/24 等价于 255.255.255.0）
      addresses:
        - 192.168.200.130/24
      # 2. 网关
	  gateway4: 192.168.200.2
	  # 3. DNS
	  nameservers:
        addresses:
          - 192.168.200.2
          - 114.114.114.114
  version: 2
```

### Root 密码找回

- **CentOS 步骤**
	- 1. 开机默认 GRUB 菜单，按 `e` 编辑
	- 2. `linux16` 行末尾加 `init=/bin/sh`，`Ctrl+X`
	- 3. `mount -o remount,rw /` 读写挂载根目录
	- 4. `passwd`
	- 5. `touch /.autorelabel`
	- 6. `exec /sbin/init`
	- 7. 自动重启，新密码生效

- **Ubuntu 步骤**
	- 1. 开机长按 `Shift` 调出 GRUB 菜单，按 `e` 编辑
	- 2. `linux` 行末尾 `ro` 改为 `rw init=/bin/bash`，`Ctrl+X`
	- 3. `passwd root`
	- 4. `reboot -f` 
	- 5. 强制重启，新密码生效

- **Ubuntu** 默认禁用 `root` 账户，`passwd root` 解锁设置密码

# 8. 任务调度

### crontab 定时任务（🔥）

- **crond + crontab**：后台进程 和 命令行工具
- `crontab -e`：编辑
- `crontab -l`：查看
- `crontab -r`：清空
- `crontab -u [user_name] [..]`：（指定用户）

- `* * * * * [命令/脚本路径]`：定时任务格式
	- 分 - 时 - 日 - 月 - 周（0 / 7  均为周日）
	- `*`：任意时间点
	- `,`：分隔多个时间点
	- `-`：连续时间段
	- `/[num]`：每隔 `[num]` 时间单位

### at 一次性任务

- **atd + at**：后台进程 和 命令行工具
- `atq`：查看
- `atrm`：清空
- `at -c [ID]`：指定查看
- `atrm [ID]`：指定删除

- `at [..] [时]`：定时任务格式
	- `-m`：（完成时邮件提醒）
	- `-v`：（显示将被执行时间）
	- `-q [队列]`：使用指定的队列
	- `-f [file_name]`：从指定文件读入任务而不是从标准输入读入
	- `-t [时间参数]`：以时间参数的形式提交要运行的任务
		
	- `04:00`：到点执行，时间已过则第二天执行（时间点）
	- `2pm`：指定上 / 下午时间 `[num]am` 和 `[num]pm`（时间点）
	- `midnight` 0 点、`noon` 12 点、`teatime` 16 点（时间点）
	- `04:00 [日期]`：必须跟在指定时间点后（日期）
		- `yy-mm-dd`：年 - 月 - 日（`2026-04-15`）
		- `mm/dd/yy`：月 / 日 / 年（`04/15/2026`）
		- `dd.mm.yy`：日 .  月 .  年（`15.04.2026`）
	- `today` 今天 `tomorrow` 明天（日期）
	- `now + [num] [time_units]`（直接指定）
		- `minutes`：分钟
		- `hours`：小时
		- `days`：天
		- `weeks`：周
		
	- 按两次 `Ctrl + D` 结束输入

# 9. 日志与故障排查

### 认证与登录日志

- `/var/log/auth.log`
	- SSH 登录 / 失败
	- su、sudo 切换用户
	- 密码修改、权限提升操作
	- （对应 CentOS 的 `/var/log/secure` ）

- `/var/log/btmp`
	- 所有错误登录尝试
	- 只能使用 `lastb` 查看

- `/var/log/wtmp`
	- 所有用户的登录、注销
	- 系统启动、重启、关机事件
	- 只能使用 `last` 查看

- `/var/log/lastlog`
	- 每个用户最后一次登录时间
	- 使用 `lastlog` 查看

- `/var/run/utmp`
	- 当前正在登录的用户信息，实时变化
	- 使用 `w` `who` `users` 查看

### 系统通用日志

- `/var/log/syslog`
	- 系统服务启动 / 异常
	- 定时任务（cron）输出
	- 内核以外的大部分系统消息
	- （对应 CentOS 的 `/var/log/messages` ）

- `/var/log/boot.log`
	- 系统启动过程日志，记录开机时服务启动、硬件检测等信息

- `/var/log/dmesg`
	- 记录内核自检、硬件驱动相关信息
	- 使用 `dmesg` 查看

### 服务与应用日志

- `/var/log/syslog`
	- 定时任务日志
	- （对应 CentOS 的 `/var/log/cron` ）

- `/var/log/mail.log`
	- 邮件服务日志，不配置邮件服务的话通常是空的

- `/var/log/cups/`
	- 打印服务日志，服务器环境一般用不到

### journalctl / tail / grep

```shell
journalctl -f                # 实时跟踪日志
journalctl -u sshd           # 查看 SSH 服务日志
journalctl -u cron           # 查看定时任务日志
journalctl -k                # 查看内核日志（替代 dmesg）
```

- 认证日志
	- CentOS `/var/log/secure` → Ubuntu `/var/log/auth.log`
- 系统通用日志
	- CentOS `/var/log/messages` → Ubuntu `/var/log/syslog`
- 定时任务日志
	- CentOS `/var/log/cron` → Ubuntu `/var/log/syslog`
- 二进制登录日志
	- `btmp`/`wtmp`/`lastlog`/`utmp` 两个系统完全通用，查看命令也一样