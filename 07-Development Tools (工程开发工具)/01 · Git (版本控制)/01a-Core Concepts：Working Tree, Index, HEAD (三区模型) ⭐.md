---
tags:
  - git
status: 🌱
---


> **核心考点**：三个区域的定义、文件在三区之间的流转、HEAD 的本质

## 三区模型

```
┌──────────────────────────────────────────────────────────────────────────────────────┐    git add     ┌──────────────┐    git commit    ┌──────────────┐
│  Working Tree │ ────────────> │    Index      │ ───────────────> │  Repository       │
│  （工作区）    │               │  （暂存区/索引）│                  │  （本地仓库）  │
│              │ <──────────── │               │ <─────────────── │                    │
└──────────────────────────────────────────────────────────────────────────────────────┘  git restore   └──────────────┘  git restore      └──────────────┘
                                                    --staged
```

|区域|别名|存储位置|含义|
|---|---|---|---|
|Working Tree|工作区|项目目录下的实际文件|你正在编辑的文件|
|Index|暂存区、Stage|`.git/index`|下次 commit 将要提交的内容快照|
|Repository|本地仓库|`.git/objects/`|已提交的历史记录，永久保存|

### 为什么需要暂存区？

暂存区允许你**精确控制每次提交的内容**，而不是把所有改动一股脑提交：

bash

```bash
# 只把部分改动加入暂存区
git add src/feature.cpp    # 只暂存这一个文件
git add -p                 # 交互式选择文件中的具体行（hunk）
git commit -m "feat: add feature"   # 只提交暂存的内容
```

---

## HEAD 是什么

HEAD 是一个指针，**指向当前所在的提交**，通常通过分支间接指向：

```
HEAD → main → commit C3 → commit C2 → commit C1
```

bash

```bash
cat .git/HEAD          # 输出: ref: refs/heads/main
cat .git/refs/heads/main   # 输出: abc1234（commit hash）
```

### Detached HEAD（分离头指针）

当 HEAD 直接指向某个 commit（而不是分支）时，就是 detached HEAD：

bash

```bash
git checkout abc1234   # HEAD 直接指向 commit，而非分支
```

- 此时做的新 commit 没有分支引用，垃圾回收时可能丢失
- 若要保留，需立即创建分支：`git checkout -b new-branch`

---

## 文件状态流转

```
Untracked → (git add) → Staged → (git commit) → Unmodified
                                                      ↓ 编辑文件
                                                   Modified
                                                      ↓ git add
                                                   Staged → commit...
```

|命令|作用|
|---|---|
|`git status`|查看各区域文件状态|
|`git diff`|工作区 vs 暂存区|
|`git diff --staged`|暂存区 vs 最新 commit|
|`git diff HEAD`|工作区 vs 最新 commit|

---

## 关联笔记

- [Conflict Resolution (冲突解决实操)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [stash, tag, reflog (实用命令)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
- [CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)
