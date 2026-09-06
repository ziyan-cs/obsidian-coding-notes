---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Conflicts and Recovery (冲突与恢复)

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

## Conflict Resolution (冲突解决)

> [!note] 本节重点心考点：冲突何时产生、冲突标记含义、解决流程、高级合并策略

## 冲突何时产生

当两个分支对**同一文件的同一区域**做了不同修改，Git 无法自动决定取谁的，产生冲突。

## 冲突标记

```
<<<<<<< HEAD
这是当前分支（HEAD）的内容
=======
这是被合并分支的内容
>>>>>>> feature/login
```

- `<<<<<<< HEAD` 到 `=======`：当前分支的版本
- `=======` 到 `>>>>>>>`：对方分支的版本
- 解决：手动编辑，保留想要的内容，删除所有标记符

## 解决流程

```bash
git merge feature            # 触发冲突
git add conflicted_file.cpp  # 标记已解决
git commit                   # 完成合并（message 自动生成）
```

## 高级技巧

### 合并策略

```bash
git merge -s recursive -X theirs feature   # 冲突全取对方版本
git merge -s recursive -X ours feature     # 冲突全取自己版本
git merge -s ours feature                  # 完全忽略对方（仅记合并事实）
```

### 快速选择

```bash
git checkout --ours file      # 取当前分支版本
git checkout --theirs file    # 取对方分支版本
git checkout --merge file     # 重新标记冲突
```

### 合并工具

```bash
git mergetool                    # 图形化合并工具
git config merge.tool vscode     # 设置默认工具
git config merge.conflictstyle diff3  # 三方对比（显示共同祖先）
```

### git rerere（复用解决方案）

```bash
git config --global rerere.enabled true
```

### 中止合并

```bash
git merge --abort     # 回到 merge 前
git rebase --abort    # 回到 rebase 前
```

## 避免冲突的实践

| 实践 | 说明 |
|------|------|
| 频繁同步主干 | 经常 rebase/merge 最新代码，减少积压 |
| 小粒度提交 | 每次改动范围小，冲突概率低 |
| 职责划分清晰 | 不同人不同模块，减少同时改同一文件 |
| 统一格式化 | 统一缩进风格，减少伪冲突 |

> [!tip]- **工程要点**：冲突不可怕，关键是理解每段代码去留的业务逻辑。`git log --merge -p` 可查看冲突文件的双方提交历史辅助决策。不要盲目 ours/theirs。

---

## 关联笔记

- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [stash, tag, reflog (实用命令)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
- [CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)

---

## reset revert and restore (撤销操作)

> [!note] 本节重点心考点：三者的作用范围、是否改写历史、适用场景

> [!warning] 先确认目标，再执行会丢数据的命令
> `reset --hard` 与 `restore` 可能丢掉未提交内容。先用 `git status`、`git diff` 确认目标；重要改动先做 commit、stash 或文件级备份。公共分支默认优先考虑 `revert`。

## 速查

|命令|作用范围|改写历史|适用场景|
|---|---|---|---|
|`git reset`|commit 历史 + 可选影响暂存区/工作区|**是**|撤销本地未推送的提交|
|`git revert`|创建新 commit 来抵消旧 commit|**否**|撤销已推送的提交|
|`git restore`|工作区 / 暂存区|—|丢弃未提交的改动|

---

## git reset

将 HEAD（和分支指针）移动到指定 commit，根据模式决定暂存区和工作区的影响：

```bash
git reset --soft  HEAD~1   # 只移动 HEAD，改动保留在暂存区
git reset --mixed HEAD~1   # 移动 HEAD + 清空暂存区，改动保留在工作区（默认）
git reset --hard  HEAD~1   # 移动 HEAD + 清空暂存区 + 丢弃工作区改动（危险！）
```

```
--soft:   [工作区: 不变] [暂存区: 不变] [HEAD: 移动]
--mixed:  [工作区: 不变] [暂存区: 清空] [HEAD: 移动]
--hard:   [工作区: 丢弃] [暂存区: 清空] [HEAD: 移动]
```

> `--hard` 丢弃的工作区改动无法通过 Git 恢复（未提交的内容真的丢了）。

---

## git revert

创建一个新 commit，内容是指定 commit 的**逆操作**，历史不被改写：

```bash
git revert abc1234          # 撤销某个 commit，产生新的 revert commit
git revert HEAD~3..HEAD     # 撤销最近 3 个 commit
git revert -n abc1234       # 不自动 commit，只改动工作区（留给你手动调整）
```

```
Before:  A ← B ← C ← D (HEAD)
After:   A ← B ← C ← D ← D'  （D' 是 D 的逆操作）
```

- ✅ 安全，适合公共分支、已推送的 commit
- ❌ 产生额外 commit，历史略显冗长

---

## git restore

专门用于撤销工作区和暂存区的改动（Git 2.23+ 引入，替代旧的 `git checkout -- file`）：

```bash
git restore file.cpp              # 丢弃工作区改动，恢复到暂存区版本
git restore --staged file.cpp     # 将文件从暂存区移出（不影响工作区）
git restore --source=HEAD~2 file  # 将文件恢复到指定 commit 的版本
```

## 30 秒回答

`reset` 移动当前分支指针，并按模式影响暂存区/工作区，适合重整尚未共享的本地历史；`revert` 新增一个反向 commit，保留公共历史；`restore` 只处理工作区或暂存区的文件内容。选择前先问：改动是否已共享？我是否需要保留历史？是否有未提交内容？

## 自测

1. 已推送到团队共用分支的一次错误提交，为什么通常优先 `revert`？
2. `git restore --staged file` 会如何影响工作区？
3. 使用 `reset --hard` 前，你会做哪两项只读检查？

---

## 关联笔记 · 延伸要点 2
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [stash, tag, reflog (实用命令)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
- [CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)

---

## stash tag and reflog (实用命令)

> [!note] 本节重点心考点：stash 暂存与恢复、tag 标记与版本、reflog 恢复误删操作

## git stash（临时搁置）

将当前工作区和暂存区的改动临时保存，让工作区恢复干净：

```bash
git stash                    # 保存当前改动
git stash push -m "wip: 登录功能"  # 附加描述
git stash list               # 查看所有 stash
git stash pop                # 恢复最近一次 stash（并删除）
git stash apply stash@{1}    # 恢复指定 stash（不删除）
git stash drop stash@{0}     # 删除指定 stash
git stash branch feature/new # 从 stash 创建新分支
```

典型场景：正在开发功能，突然需要切换分支修 bug，先 stash 保存进度。

> `git stash` 默认不保存 untracked 文件，需要 `git stash -u` 才包含。

---

## git tag（标签）

为特定 commit 打上永久标记，常用于标识版本发布：

```bash
git tag v1.0.0

git tag -a v1.0.0 -m "Release version 1.0.0"
git tag -a v1.0.0 abc1234    # 为历史 commit 打标签

git tag                      # 列出所有标签
git show v1.0.0              # 查看标签详情
git push origin v1.0.0       # 推送单个标签（tag 默认不随 push 上传）
git push origin --tags       # 推送所有标签
git tag -d v1.0.0            # 删除本地标签
git push origin :refs/tags/v1.0.0  # 删除远端标签
```

---

## git reflog（操作日志 / 后悔药）

记录本地所有 HEAD 的移动历史，即使 commit 被 reset 也能找回：

```bash
git reflog                   # 查看 HEAD 的所有历史移动记录
git reflog show main         # 查看某个分支的移动记录
```

输出示例：

```
abc1234 HEAD@{0}: commit: feat: add login
def5678 HEAD@{1}: reset: moving to HEAD~1
ghi9012 HEAD@{2}: commit: wip: half-done feature
```

**找回误删的 commit：**

```bash
git reset --hard HEAD@{2}    # 回到 reset 之前的状态
git checkout -b rescue ghi9012  # 从丢失的 commit 创建新分支
```

> reflog 是本地的，克隆新仓库没有 reflog。默认保留 90 天。

---

## 关联笔记 · 延伸要点 3
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)



## 零基础阅读路径

先从最短命令路径跑通一次，再回来看配置字段与高级选项。每读一段命令，都要知道它读取什么、生成什么以及怎样撤销或诊断。

## 常见误区

- 只记命令，不理解它改变了哪些输入、产物或运行环境，发生故障时无法恢复。
- 没有在临时项目中亲自执行并保留输出，就把工具流程当成已经掌握。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Conflicts and Recovery (冲突与恢复)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
