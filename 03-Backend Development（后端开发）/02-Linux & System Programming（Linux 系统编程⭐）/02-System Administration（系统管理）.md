

# 0. 核心

- 系统整体思维：从单点命令操作上升到系统环境管理
- 掌握 `Linux` 系统运维的核心流程
	- 用户管理   
	- -> 权限控制
	- -> 服务运维
	- -> 配置调优

# 1. 用户 & 组管理

### 1.0 核心

- 组：用户的集合，用于批量权限管理
	- 主组 `UID`：默认身份组，用户只有 1 个，创建文件时的默认所属组
	- 附加组 `GID`：额外权限组，用户可以有多个，仅用于给用户自身额外的文件权限

### 1.1 增删用户 & 组

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

### 1.2 修改用户 & 组

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

### 1.3 切换用户

- 切换
	- `su [user_name]`：仅切换用户身份，不切换环境
	- `su - [user_name]`：完整切换用户身份与环境（推荐）
	- `newgrp`：临时切换用户的有效组（用于新文件的默认所属组）

- 回退
	- `exit`：退回上一级用户
	- `logout`：退回最初登录系统的用户

### 1.4 查看信息

- `id`：查看当前用户 `UID GID 组`
- `id [user_name]`：查看用户 `UID GID 组`

- `Whoami`：查看当前有效用户身份
- `Who am i`：查看最初登录系统的用户身份
- `who`：查看所有当前登录系统的用户
- `w`：`who` 的增强版，额外显示用户的操作、负载等信息

- `cat /etc/group`：查看系统所有组信息
# 2. 权限管理

### 2.1 与文件关系

- 核心用户分类
	- 所有者 `u` ：默认是文件创建者
	- 所属组 `g` ：默认是文件创建者的 **主组**
		- 改文件所属组 = **附加组**（实现共享权限）
	- 其他用户 `o` ：所有者和所属组以外
	- 所有用户 `a`：`u`+`g`+`o`

- 操作顺序：先确认文件的所属组，再设置所属组权限

### 2.2 文件权限查看与修改

- 查看文件权限
	- `ls -l`：查看文件类型、权限位、所有者、所属组
		- 如 `-rwxrw-r-- 1 root root 2343 Feb 2 12:30 hello.txt`
	- 第 1 段
		- 文件类型：文件 `-` 、目录 `d` 、链接 `l` 、字符设备 (键鼠) `c` 、块设备 (硬盘) `d` ``
		- 权限位由三组 `rwx` 组成：对应 **所有者 所属组 其他用户** 的文件权限
	- 第 2 段
		- 文件：硬链接数（默认初始1）
		- 目录：子目录数+文件数
	- 第 3-4 段
		- 文件所有者
		- 文件所属组
	- 第 5 段
		- 普通文件：实际占用字节
		- 目录：目录项元数据大小，固定 4096 字节（不代表目录内文件总大小）
	- 第 6 段
		- 记录文件 / 目录 最后一次内容修改的时间 `mtime`
	- 第 7 段：
		- 文件 / 目录的名称，隐藏文件以 `.` 开头


- `rwx` 权限详解
	- 作用到文件
		- `r`：`cat`、`less` 查看文件内容
		- `w`：`vim` 修改
		- `x`：执行该文件
			
	- 作用到目录 
		- `r`：`ls` 查看文件名，不支持 `ls -l` 查看文件详情
		- `w`：创建、删除、重命名、移动 文件 / 子目录
		- `x`：`cd` 进入目录、访问目录内文件
		- `rx`：`ls -l` 查看文件详情

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

### 2.3 特殊权限

SUID/SGID/ 粘滞位（SGID 用于目录，让新文件继承目录的所属组）

# 3. 服务管理


# 4. 系统核心配置文件
### 4.1 用户与环境

- `/etc/passwd`：用户信息库
	- **作用**：存储系统所有用户的基础信息，所有用户可读，仅 root 可修改
	- **字段格式**：`用户名:密码占位符:UID:GID:用户注释:家目录:登录Shell`
	    - `x`：密码占位符（真实密码存在 `/etc/shadow`）
	    - `UID`：用户 ID（0=root，1-999 = 系统用户，1000+= 普通用户）
	    - `GID`：用户主组 ID（对应 `/etc/group`）
	    - `登录Shell`： `bash`（国内用）； `csh` ；`tcsh`
	- **常用操作**：
	    - `cat /etc/passwd`：查看所有用户
	    - `grep [user] /etc/passwd`：查看指定用户信息

- `/etc/shadow`：用户密码（加密）库
	- **作用**：存储加密后的用户密码、密码有效期等安全信息，仅 root 可读可写
	- **字段格式**：`用户名:加密密码:最后修改时间:最小修改间隔:密码有效期:警告天数:过期宽限:过期时间:保留`
	    - `*`/`!`：账户锁定，无法登录
	    - `$6$` 开头：SHA-512 加密（Ubuntu 默认）
	- **常用操作**：
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

### 4.2 网络与主机
- `/etc/hostname`：主机名
- `/etc/hosts` ：主机名与映射
- `/etc/resolv.conf`：DNS 配置

### 4.3 系统资源

# 5. Root 密码找回

- 核心原理
	- GRUB 修改启动参数 → 进入`单用户 Shell` → 绕过登录 → 修改密码

- CentOS 步骤
	- 1. 开机默认 GRUB 菜单，按 `e` 进入编辑
	- 2. `linux16` 行末尾加 `init=/bin/sh`，`Ctrl+x` 启动 进入单用户模式
	- 3. 输入 `mount -o remount,rw /` 读写挂载根目录
	- 4. 输入 `passwd`
	- 5. 依次 输入两次新密码
	- 6. 输入 `touch /.autorelabel`
	- 7. 输入 `exec /sbin/init`
	- 8. 耐心等待……
	- 9. 系统自动重启，新密码生效

- Ubuntu 步骤
	- 1. 开机长按 `Shift` 调出 GRUB 菜单，按 `e` 编辑
	- 2. `linux` 行末尾 `ro` 改为 `rw init=/bin/bash`，`Ctrl+x` 启动 进入单用户模式
	- 3. 输入 `passwd root`
	- 4. 依次 输入两次新密码
	- 5. 输入 `reboot -f` 
	- 6. 系统强制重启，新密码生效

- 关键说明
	
	- 必须读写挂载根目录，否则无法修改密码
	- 仅适用于物理机/拥有服务器物理控制权的场景
	- 「 *Ubuntu* 默认禁用 `root` 账户，`passwd root` 会同时解锁 `root` 并设置密码 」

# 6. 计划模式
# 7. 磁盘管理
