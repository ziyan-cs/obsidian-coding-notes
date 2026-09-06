---
tags:
  - devtools/git
status: 🌱
---

# stash, tag, reflog — 实用命令

> [!important] **核心考点**：stash 暂存与恢复、tag 标记与版本、reflog 恢复误删操作

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
# 轻量标签（只是一个指针）
git tag v1.0.0

# 附注标签（推荐，包含作者、日期、说明）
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
# 或者
git checkout -b rescue ghi9012  # 从丢失的 commit 创建新分支
```

> reflog 是本地的，克隆新仓库没有 reflog。默认保留 90 天。

---

## 关联笔记

- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)
