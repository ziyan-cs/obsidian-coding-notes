---
tags:
  - devtools/git
status: 🌱
---



> [!important] **核心考点**：三者的本质区别、各自的使用场景、rebase 的黄金法则

## merge（合并）

将两个分支的历史合并，产生一个新的 merge commit：

```
      A---B---C  feature
     /         \
D---E-----------F  main（merge commit）
```

```bash
git checkout main
git merge feature
```

- ✅ 保留完整历史，能清楚看到分支何时创建、何时合并
- ✅ 安全，不改写历史
- ❌ 历史线复杂，多分支时图形乱
- 适合：合并长期存在的功能分支、公共分支合并

**Fast-forward merge**：若 main 没有新 commit，直接移动指针，不产生 merge commit：

```bash
git merge --no-ff feature   # 强制产生 merge commit，保留分支痕迹
```

---

## rebase（变基）

将当前分支的 commit 「移植」到目标分支的最新 commit 之后，**重写提交历史**：

```
# rebase 前
      A---B---C  feature
     /
D---E---F---G  main

# rebase 后（feature 上的 A B C 被重新应用，变成 A' B' C'）
              A'--B'--C'  feature
             /
D---E---F---G  main
```

```bash
git checkout feature
git rebase main
```

- ✅ 历史线性整洁，易于阅读和 bisect
- ❌ **改写了 commit hash**，已推送到远端的分支不能随意 rebase
- 适合：整理本地未推送的提交、将功能分支同步主干最新代码

**⚠️ rebase 黄金法则：永远不要 rebase 已经推送到公共仓库的分支。** 这会导致他人的本地历史与远端不一致，产生混乱。

**交互式 rebase**（整理提交历史）：

```bash
git rebase -i HEAD~3    # 对最近 3 个 commit 进行交互式操作
# pick   → 保留
# squash → 合并到上一个 commit
# reword → 修改 commit message
# drop   → 删除这个 commit
# edit   → 在这个 commit 处暂停，允许修改
```

---

## cherry-pick（摘取）

将指定的某个（或某几个）commit 应用到当前分支，不合并整个分支：

```
      A---B---C  feature
     /
D---E---F  main

git cherry-pick B

D---E---F---B'  main   （只把 B 摘过来，变成 B'）
```

```bash
git cherry-pick abc1234           # 摘取单个 commit
git cherry-pick abc1234 def5678   # 摘取多个
git cherry-pick A..C              # 摘取范围（不含 A，含 C）
```

- ✅ 精准控制，只取需要的变更
- ❌ 产生重复 commit，游离于原分支之外
- 适合：hotfix 需要同时应用到多个版本分支、从误提交的分支上救回某个 commit

---

## 三者对比

| |merge|rebase|cherry-pick|
|---|---|---|---|
|历史形态|非线性（有分叉）|线性|线性（复制单个）|
|改写历史|否|是|是（新 hash）|
|使用粒度|整个分支|整个分支|单个/多个 commit|
|冲突处理|一次性解决|逐 commit 解决|逐 commit 解决|
|适用场景|功能合并、保留历史|整理本地提交|跨分支移植特定修复|

---

## 关联笔记

- [01b2-Git Flow & Feature Branch Workflow (团队协作流)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b2-Git%20Flow%20&%20Feature%20Branch%20Workflow%20(团队协作流).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [stash, tag, reflog (实用命令)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
