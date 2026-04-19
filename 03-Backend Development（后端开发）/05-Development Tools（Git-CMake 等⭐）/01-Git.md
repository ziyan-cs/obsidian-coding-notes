

# 1. 速览

- **Git**：分布式版本控制系统
	- （跟踪代码修改、协作开发）
- **GitHub**：基于 Git 的远程代码托管平台
	- （代码共享、协作的核心场所）
- 工作区域
	- 工作区：实际编写代码的目录
	- 暂存区：临时存放待提交的修改的区域
	- 本地仓库：本地存储历史版本的区域 `.git` 目录
	- 远端仓库：托管在 **GitHub** 等平台的区域

>流程：*工作区 → 暂存区 → 本地仓库 → 远程仓库


# 2. 环境配置

```cpp
# 配置全局用户名（GitHub 账号名）
git config --global user.name "你的GitHub用户名"

# 配置全局邮箱（GitHub 绑定邮箱）
git config --global user.email "你的GitHub邮箱"
```

# 3. 本地仓库操作

```cpp
# 1. 初始化本地仓库（新项目仅一次）
git init

# 2. 文件添加到暂存区
git add [file_name]        # 添加单个文件
git add .             # 添加当前目录所有文件（常用）

# 3. 提交到本地仓库
git commit            # 系统提示写提交说明
git commit -m "提交说明（如：init/debug/feat）"

# 4. 查看状态
git status

# 5. 查看提交历史
git log                # 完整历史
git log --oneline      # 精简历史（推荐）
```

# 4. 远程仓库关联

```cpp
# 1. 关联远程仓库（只需关联一次）
git remote add origin [GitHub仓库地址]

# 2. 推送代码
git push -u origin main  # 首次推送
git push origin main     # 日常推送

# 3. 拉取远程代码（协作时必用）
git pull origin main     # 拉取 main 分支并合并
```

# 5. 分支管理（协作必用)

```cpp
# 1. 查看分支
git branch

# 2. 创建分支
git branch [分支名]

# 3. 切换分支
git checkout [分支名]

# 4. 创建并切换分支（常用）
git checkout -b [分支名]

# 5. 合并分支（切换到主分支再合并）
git checkout main        # 先切到主分支
git merge [分支名]        # 合并指定分支到主分支

# 6. 删除分支
git branch -d [分支名]    # 删除已合并的分支
```

# 6. 克隆项目

- 已有关联账号 / 邮箱前提，不用 `git init` 
- 创建一个包含完整历史、所有文件、本地仓库 `.git` 的本地目录

```cpp
# 1. 新建/进入空文件夹 
# 2. 直接执行：
   git clone [GitHub仓库地址]
```