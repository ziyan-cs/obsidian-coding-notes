---
tags:
  - devtools/git
status: 🌱
---


> [!important] **核心考点**：三者的作用范围、是否改写历史、适用场景

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

---

## 关联笔记

- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [stash, tag, reflog (实用命令)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
- [CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)
