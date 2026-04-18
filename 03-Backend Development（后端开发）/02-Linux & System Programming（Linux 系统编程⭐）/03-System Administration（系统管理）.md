


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

### 切换用户

- 切换
	- `su [user_name]`：切换用户身份，不切换环境
	- `su - [user_name]`：切换用户身份与环境
	- `newgrp`：临时切换用户的有效组（新文件的默认所属组归属）

- 回退
	- `exit`：退回上一级用户
	- `logout`：退回最初登录系统的用户

### 查看信息

- `id`：查看当前用户 `UID GID 组`
- `id [user_name]`：查看用户 `UID GID 组`

- `Whoami`：查看当前有效用户身份
- `Who am i`：查看最初登录系统的用户身份
- `who`：查看所有当前登录系统的用户
- `w`：`who` 的增强版，额外显示用户的操作、负载等信息

- `cat /etc/group`：查看系统所有组信息
# 2. 权限管理（🔥加强）

### rwx 本质

- `ls -l`：查看文件类型、权限位、所有者、所属组
	- 文件类型：文件 `-` 、目录 `d` 、链接 `l` 、字符设备 `c` 、块设备 `d` ``
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
	- 同时修改：`chown [user_name]:[group_name] [file_name/dir_name]`
	- 修改文件 / 目录 所有者：`chown [user_name] [file_name/dir_name]`
	- 修改文件 / 目录 所属组：`chgrp [group_name] [file_name/dir_name]`
		- 对目录修改：`chown -R [] [dir_name]`（递归操作文件和子目录）

2. 符号法修改文件权限
	- `chmod [用户范围][操作符][权限符] [file_name/dir_name]`
		- 如 `chmod g+rwx [file_name/dir_name]`：给所属组添加读、写、执行权限
		- 如 `chmod o-r [file_name/dir_name]`：移除其他用户读权限
	- 用户范围：`u` 、`g` 、`o` 、`a`
	- 操作符：`+` 添加、`-` 移除、`=` 设置
	- 权限符：可读 `r` = 4、可写 `w` = 2、`x` = 1、无权限 `-` = 0

3. 数字法修改文件权限
	- `chmod 755 [file_name]`：（三组权限值的各自和）

- `umask`：默认权限掩码（控制新文件 / 目录的默认所有者、组权限）
### umask（🔥）
### 特殊权限（🔥）

SUID/SGID/ 粘滞位（SGID 用于目录，让新文件继承目录的所属组）

# 3. 服务管理

### systemctl（🔥）

- `systemctl status`：整体运行状态
- `systemctl list-units --type=service --all`
	- 查看正在运行的服务
- `systemctl list-units --type=service --state=running`
	- 查看全部已安装服务
- `systemctl --failed`
	- 查看异常、启动失败的服务

- `systemctl [..] [process_name]`：
	- `status`：（查看服务状态）
	- `stop`：（停止）
	- `start`：（启动）
	- `restart`：（重启）
	- `reload`：（重新加载配置）
	- `mask`：彻底锁定，禁止启动
	- `unmask`：解除锁定

### service

- `service [process_name] [..]`：
	- `statu`：（查看服务状态）
	- `stop`：（停止）
	- `start`：（启动）
	- `restart`：（重启）
	- `reload`：（重新加载配置）

### 开机自启

- `systemctl [..] [process_name]`
	- `enable`：服务开机自启
	- `disable`：关闭开机自启
	- `is-enabled`：查询是否开机自启

# 4. 系统配置文件（🔥整理逻辑）

### /etc/passwd

- 用户信息库
	- 作用：存储系统所有用户的基础信息，所有用户可读，仅 root 可修改
	- `用户名:密码占位符:UID:GID:用户注释:家目录:登录Shell`
	    - `x`：密码占位符（真实密码存在 `/etc/shadow`）
	    - `UID`：用户 ID（0=root，1-999 = 系统用户，1000+= 普通用户）
	    - `GID`：用户主组 ID（对应 `/etc/group`）
	    - `登录Shell`： `bash`（国内用）； `csh` ；`tcsh`
	- 常用操作
	    - `cat /etc/passwd`：查看所有用户
	    - `grep [user] /etc/passwd`：查看指定用户信息

### /etc/shadow

 - 用户密码（加密）库
	 - 加密的用户密码、密码有效期等安全信息，仅 **root** 可读可写
	- `用户名:加密密码:最后修改时间:最小修改间隔:密码有效期:警告天数:过期宽限:过期时间:保留`
		- `*`/`!`：账户锁定，无法登录
	    - `$6$` 开头：SHA-512 加密（Ubuntu 默认）
	- 常用操作
		- `sudo cat /etc/shadow`：查看所有用户密码信息
		- `sudo grep [user] /etc/shadow`：查看指定用户密码信息

- `/etc/group`：组信息库
	- **作用**：存储系统所有用户组的信息，所有用户可读，仅 root 可修改
	- **字段格式**：`组名:密码占位符:GID:组成员列表`
	    - 密码占位符通常为 `x`，组密码极少使用
	- **常用操作**：
	    - `cat /etc/group`：查看所有用户组
	    - `groups [user]`：查看用户所属组（等价于 `grep [user] /etc/group`）

- `/etc/gshadow`：用户组密码安全文件
	- **作用**：存储用户组的加密密码、组管理员等信息，仅 root 可读可写
	- **常用操作**：`sudo grep [group] /etc/gshadow`

### /etc/profile

### /etc/hosts

- 主机映射
	- `IP [主机名]`


# 5. 任务调度

### crontab（🔥重点）

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

### at

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

# 6. 磁盘管理

- `hdx~`：**IDE** 硬盘
- `sdx~`：**SCSI** 硬盘（主流）
	- `x`：盘号
	- `~`：分区号

### df / du

- `df`：查看设备磁盘挂载、使用情况
- `du [dir_name]`：查看目录磁盘占用情况
	- `-h`：（ K / M / G）
	- `-a`：（所有，默认只显示子目录）
	- `-s`：（不展开子目录详情）
	- `-c`：（列出明细，额外输出最终汇总值）
	- `--max-depth=[num]`：（子目录的递归深度）

### mount / umount

- `mount [/dev/sdb] [dir_name]`：分区挂载
- `umount [/dev/sdb]`：分区卸载

### 分区

- 硬盘分为 主分区 + 扩展分区（共 4 个）
	- 主分区 → 直接挂载
	- 扩展分区 → 逻辑分区 → 挂载
- 主分区：编号 1-4
- 扩展分区：编号 1-4
- 逻辑分区：编号 5 开始，数量无限

# 7. 系统控制（速查）

### 运行级别

- ( **0** )、( **6** ) 开机后重复关机 / 重启
- ( **1** ) 单用户，用于 root 密码找回、系统修复
- ( **2** ) 多用户无网络服务
- ( **3** ) 多用户有网络服务
- ( **5** ) 图形化界面
- 服务器一般使用 ( **3** )，桌面版使用 ( **5** )
- `init [num]`：（临时切换）

- `systemctl get-default`：查看运行级别
	-  `runlevel`；传统使用
- `systemctl set-default multi-user.target`
	- 设置开机默认 纯字符多用户 ( **3** )
- `systemctl set-default graphical.target`
	- 设置开机默认 图形桌面 ( **5** )

### 配置静态 IP

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

### 硬盘分区

- **虚拟机步骤**
	- 1. 设置添加新硬盘，后重启
	- 2. `fdisk /dev/sdb`（创建分区）
		- `n` → `p`：添加
		- `w`：保存分区表（关键！）
		- `q`：不保存退出
		- `d`：删除分区
	- 3. `mkfs -t ext4 /dev/sdb1`（格式化）
	- 4. `mount /dev/sdb1 [dir_path]`（挂载）
	- 5. `vim /etc/fstab`（设置自动挂载）
	- 6. `blkid /dev/sdb1`（查看 UUID）
	- 7. `/dev/sdb1 [dir_path] ext4 defaults 0 0`
	- 8. `mount -a`：验证

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
