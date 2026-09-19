---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [engineering/git, engineering/recovery]
---

> [!abstract] 学习目标
> 能从三方合并理解冲突，安全选择 restore、reset、revert、reflog 与 stash，并在执行破坏性操作前明确会改变哪一层状态。

# 冲突是语义决策，不是删除标记

Git 三方合并比较共同祖先 base、当前侧 ours 与另一侧 theirs。两侧对同一区域做了无法自动组合的修改时产生冲突；即使 Git 自动合并成功，语义仍可能错误，因此必须构建和测试。

```text
<<<<<<< HEAD
当前分支内容
=======
另一侧内容
>>>>>>> topic
```

不要机械保留 ours/theirs。先回答两侧分别试图维持什么行为，再写出同时满足新契约的结果。

# 可靠冲突处理流程

```bash
git status
git diff --name-only --diff-filter=U
git diff

# 编辑并验证每个冲突文件
git add path/to/resolved-file
git status

# 根据当前操作继续
git merge --continue
git rebase --continue
git cherry-pick --continue
```

中止应使用与当前操作对应的命令：

```bash
git merge --abort
git rebase --abort
git cherry-pick --abort
git revert --abort
```

处理前先确认未提交修改是否属于自己；不清楚时保存补丁或建立临时提交，不要直接用 `reset --hard` 清场。

`git rerere` 可记录冲突形状与解决结果，在后续重复冲突时复用，但仍需检查上下文并运行验证。

# restore、reset 与 revert

三个命令针对的层次不同：

| 目标 | 命令 | 是否改写分支历史 |
|---|---|---:|
| 恢复工作区/暂存区文件 | `git restore` | 否 |
| 移动当前分支引用，按模式同步 index/工作区 | `git reset` | 是 |
| 新建一个反向提交 | `git revert` | 否 |

## Restore

```bash
git restore file                 # index → 工作区，丢弃未暂存修改
git restore --staged file        # HEAD → index，仅取消暂存
git restore --source=<commit> file
```

第一条会覆盖工作区内容，执行前先查看 `git diff -- file`。

## Reset

```text
--soft ：移动 HEAD；保留 index 与工作区
--mixed：移动 HEAD；重置 index；保留工作区（默认）
--hard ：移动 HEAD；重置 index 与工作区
```

`reset --hard` 会丢弃被覆盖的已跟踪修改，并可能影响未跟踪路径，属于最后手段。对已推送且他人使用的提交，不应仅为“历史好看”执行 reset 后强推。

## Revert

`git revert <commit>` 生成新提交来反向应用目标改动，适合共享历史。revert 合并提交需要用 `-m` 指定主线父提交；它还会影响未来合并语义，执行前要理解原拓扑并验证结果。

# Reflog 与对象恢复

reflog 记录本地引用曾指向的位置：

```bash
git reflog
git show HEAD@{2}
git branch rescue HEAD@{2}
```

安全恢复习惯是先创建 `rescue` 分支，再检查内容，而不是立刻再次 reset。reflog 是本地记录，受过期和清理策略影响，也不会自动存在于其他克隆中，因此不是备份系统。

# Stash 的边界

```bash
git stash push -u -m "wip parser experiment"
git stash list
git stash show -p stash@{0}
git stash apply stash@{0}
```

`apply` 保留 stash，验证无误后再 `drop`；`pop` 会尝试应用并在成功时删除记录。stash 适合短期切换上下文，不适合作为长期工作归档。默认行为、是否包含 untracked/ignored 文件需由参数明确。

# Tag 与发布引用

轻量 tag 只是引用；annotated tag 是对象，包含说明、作者和时间，并可签名。发布通常使用 annotated tag：

```bash
git tag -a v1.2.0 -m "release v1.2.0"
git push origin v1.2.0
```

tag 指向源码版本，不等于制品已经经过同一构建、签名和发布流程；发布系统还需记录制品摘要与 provenance。

# 破坏性操作前的检查

```text
1. 当前分支、HEAD 和 upstream 是什么？
2. 工作区、index 中有哪些未保存修改？
3. 目标提交/路径是否精确？
4. 操作会移动引用、覆盖工作区还是新增反向提交？
5. 提交是否已共享？是否先建 rescue 分支？
```

# 检查理解

1. 自动合并成功为什么仍需测试？
2. 如何分别“取消暂存”和“丢弃工作区修改”？
3. 公共分支错误为何通常用 revert 而不是 reset？
4. reflog 为什么能救回部分提交，却不能当远程备份？

> [!summary] 本篇结论
> 恢复操作必须先定位状态层：restore 面向文件，reset 移动分支并可同步 index/工作区，revert 在公共历史中追加反向提交。冲突解决的目标是恢复业务语义，而不是让标记消失。

## 权威依据

- [Git: reset, restore and revert](https://git-scm.com/docs/git#_reset_restore_and_revert)
- [git-reflog documentation](https://git-scm.com/docs/git-reflog)

下一步：[03-CI CD (持续集成与交付)](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/03-CI%20CD%20(持续集成与交付).md)
