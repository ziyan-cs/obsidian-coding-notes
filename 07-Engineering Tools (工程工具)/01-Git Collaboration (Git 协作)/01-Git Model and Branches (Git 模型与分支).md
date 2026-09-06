---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

# Git Working Tree Index and HEAD (Git 三区模型)

> [!note] 本节重点：核心考点：三个区域的定义、文件在三区之间的流转、HEAD 的本质

# 三区模型

```text
Three Areas of Git:

┌──────────────────────┐     ┌──────────────────────┐     ┌──────────────────────────┐
│  Working Tree        │     │  Index (Staging)     │     │  Local Repository (.git) │
│                      │     │                      │     │                          │
│  actual files        │     │  blob entries        │     │  commit history          │
│  src/main.cpp        │     │  managing snapshot   │     │  (commit object DAG)     │
│                      │     │  to be committed     │     │                          │
│                      │     │                      │     │  HEAD → current branch   │
│                      │     │                      │     │         points to latest │
└──────────────────────┘     └──────────────────────┘     │         commit           │
          │                           ▲                   └──────────────────────────┘
          │        git add            │                              ▲
          └───────────────────────────┘                              │
                                      │            git commit        │
                                      └──────────────────────────────┘
                                                              │
          git checkout (checkout) ────────────────────────────┘
          git reset (reset HEAD) ─────────────────────────────┘
```

|区域|别名|存储位置|含义|
|---|---|---|---|
|Working Tree|工作区|项目目录下的实际文件|你正在编辑的文件|
|Index|暂存区、Stage|`.git/index`|下次 commit 将要提交的内容快照|
|Repository|本地仓库|`.git/objects/`|已提交的历史记录，永久保存|

## 为什么需要暂存区？

暂存区允许你**精确控制每次提交的内容**，而不是把所有改动一股脑提交：

bash

```bash
git add src/core/Epoll.cpp    # 只暂存这一个文件
git add -p                 # 交互式选择文件中的具体行（hunk）
git commit -m "feat(core): add Epoll class"   # 只提交暂存的内容
```

---

# HEAD 是什么

HEAD 是一个指针，**指向当前所在的提交**，通常通过分支间接指向：

```
HEAD → main → commit C3 → commit C2 → commit C1
```

bash

```bash
cat .git/HEAD          # 输出: ref: refs/heads/main
cat .git/refs/heads/main   # 输出: abc1234（commit hash）
```

## Detached HEAD（分离头指针）

当 HEAD 直接指向某个 commit（而不是分支）时，就是 detached HEAD：

bash

```bash
git checkout abc1234   # HEAD 直接指向 commit，而非分支
```

- 此时做的新 commit 没有分支引用，垃圾回收时可能丢失
- 若要保留，需立即创建分支：`git checkout -b new-branch`

---

# 文件状态流转

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

# 关联笔记

- Conflict Resolution (冲突解决实操)
- reset vs revert vs restore (撤销三兄弟)
- stash, tag, reflog (实用命令)
- CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)
- 01b1-merge vs rebase vs cherry-pick (三种合并对比)

---

# merge rebase and cherry pick (合并策略)

> [!note] 本节重点：核心考点：三者的本质区别、各自的使用场景、rebase 的黄金法则

# merge（合并）

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

# rebase（变基）

将当前分支的 commit 「移植」到目标分支的最新 commit 之后，**重写提交历史**：

```
      A---B---C  feature
     /
D---E---F---G  main

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
```

---

# cherry-pick（摘取）

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

# 三者对比

| |merge|rebase|cherry-pick|
|---|---|---|---|
|历史形态|非线性（有分叉）|线性|线性（复制单个）|
|改写历史|否|是|是（新 hash）|
|使用粒度|整个分支|整个分支|单个/多个 commit|
|冲突处理|一次性解决|逐 commit 解决|逐 commit 解决|
|适用场景|功能合并、保留历史|整理本地提交|跨分支移植特定修复|

---

# 关联笔记 · 延伸要点 2
- 01b2-Git Flow & Feature Branch Workflow (团队协作流)
- Core Concepts：Working Tree, Index, HEAD (三区模型)
- Conflict Resolution (冲突解决实操)
- reset vs revert vs restore (撤销三兄弟)
- stash, tag, reflog (实用命令)

---

# Branch Workflow (分支协作流)

> [!note] 本节重点：核心考点：Git Flow 五分支模型、Feature Branch Workflow、CI/CD 集成中的分支策略

> [!tip] 先选最小能保障质量的流程
> 对个人项目和多数小团队，短生命周期 feature branch + review + CI 往往足够。只有确有固定发版、并行维护等约束时，再承担 Git Flow 的额外分支成本。

## Feature Branch Workflow（功能分支工作流）

最基础的团队协作模型：每个功能/修复都在独立分支上开发，完成后通过 PR/MR 合并到主分支。

```
main: ─────────────────────────────────────────────>
              ↑                        ↑
feature/login ─ commit ─ commit ─────>   （PR → merge）
feature/pay        ─ commit ─ commit ──> （PR → merge）
```

- 主分支始终保持可发布状态
- 代码审查（Code Review）通过 PR 实现
- 适合：大多数团队的日常开发

---

## Git Flow（经典发版模型）

适合有明确版本发布节奏的项目（如软件产品）：

```
main:      ────●─────────────────────────●─────●──>  (只存放发布版本，打 tag)
               ↑                         ↑     ↑
hotfix:        └─fix────────────────────>┘     |
                                               |
develop:   ──────●──●──●──●──●──●──●───●──●────●──>  (日常集成分支)
                 ↑              ↑  ↑           ↑
feature/A:       └─ commit ────>┘  |           |
feature/B:           └─ commit ───>┘           |
release/1.0:               └─ test/fix ───────>┘     (预发布版本)
```

|分支|作用|创建来源|合并目标|
|---|---|---|---|
|main|生产代码，只有发布版本|—|—|
|develop|日常集成，始终最新|main|main（发布时）|
|feature/*|新功能开发|develop|develop|
|release/*|发版准备（只修 bug）|develop|main + develop|
|hotfix/*|生产紧急修复|main|main + develop|

- ✅ 流程清晰，适合多版本并行维护
- ❌ 流程偏重，小团队/快速迭代场景过于繁琐

---

## Trunk-Based Development（主干开发）

以短分支或直接向主干集成为特征的模式：

- 通过直接提交或极短生命周期 feature branch 保持与主干同步
- 用 Feature Flag 控制功能开关，而非靠分支隔离
- 配合完善的 CI/CD 保证主干质量
- 适合：高频发布、DevOps 成熟的团队

# 30 秒回答

分支策略的目的不是制造流程，而是让主干始终可集成、变更可审查、发布可追溯。个人项目可用短 feature branch；有固定发版节奏时 Git Flow 可能更清晰；高频交付团队可采用 trunk-based。选择依据是发布节奏、CI 可靠度和团队协作成本。

# 自测

1. Git Flow 为什么可能不适合一个快速迭代的小项目？
2. feature branch 合并前，最少要有哪些质量信号？
3. 什么时候 feature flag 比长期分支更合适？

---

# 关联笔记 · 延伸要点 3
- 01b1-merge vs rebase vs cherry-pick (三种合并对比)
- Core Concepts：Working Tree, Index, HEAD (三区模型)
- Conflict Resolution (冲突解决实操)
- reset vs revert vs restore (撤销三兄弟)
- stash, tag, reflog (实用命令)

# 零基础阅读路径

先从最短命令路径跑通一次，再回来看配置字段与高级选项。每读一段命令，都要知道它读取什么、生成什么以及怎样撤销或诊断。

# 常见误区

- 只记命令，不理解它改变了哪些输入、产物或运行环境，发生故障时无法恢复。
- 没有在临时项目中亲自执行并保留输出，就把工具流程当成已经掌握。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Git Model and Branches (Git 模型与分支)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Git Collaboration Map (Git 协作导航)](/07-Engineering%20Tools%20(工程工具)/01-Git%20Collaboration%20(Git%20协作)/00-Git%20Collaboration%20Map%20(Git%20协作导航).md)
- 下一步：[02-Conflicts and Recovery (冲突与恢复)](/07-Engineering%20Tools%20(工程工具)/01-Git%20Collaboration%20(Git%20协作)/02-Conflicts%20and%20Recovery%20(冲突与恢复).md)
