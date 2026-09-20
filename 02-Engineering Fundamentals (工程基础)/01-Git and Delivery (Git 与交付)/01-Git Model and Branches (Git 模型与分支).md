---
study_stage: backlog
tags: [engineering/git, engineering/collaboration]
---

> [!abstract] 学习目标
> 从对象、引用、工作区和暂存区理解 Git；能选择 merge、rebase、cherry-pick 与主干协作方式，并知道哪些操作会改写共享历史。

# Git 保存的是快照与关系

Git 对内容寻址对象建立有向图：

```text
blob：文件内容
tree：目录快照，引用 blob 与子 tree
commit：引用根 tree、父 commit、作者与说明
tag：可选的带注释引用对象
```

分支本质是指向某个 commit 的可移动引用；`HEAD` 通常指向当前分支，分离 HEAD 时直接指向 commit。commit 不保存“第几行被修改”的动作列表，diff 是比较两个快照后计算出的视图。

## 工作区、暂存区与 HEAD

```text
HEAD 中的快照  ← commit ←  Index / staging area  ← add ←  Working tree
```

- working tree：当前可编辑文件；
- index：下一次提交准备采用的快照；
- `HEAD`：当前检出提交的基准。

因此同一文件可以同时存在 staged 和 unstaged 修改。先分别查看：

```bash
git status --short
git diff                 # 工作区 vs index
git diff --staged        # index vs HEAD
git diff HEAD            # 工作区总体 vs HEAD
```

`git add -p` 让一个工作文件中的不同改动进入不同提交。提交应围绕一个可解释目的，而不是按“今天改了什么”打包。

# 引用、远端与同步

`origin/main` 是最近一次 fetch 后本地记录的远端跟踪引用，不是远程服务器的实时状态。

```bash
git fetch origin
git log --graph --decorate --oneline --all
```

`fetch` 获取对象和更新远端跟踪引用，不自动整合当前分支；`pull` 等于 fetch 后再按配置 merge 或 rebase。学习阶段建议分开执行，先观察图再决定如何整合。

# merge、rebase 与 cherry-pick

## Merge

merge 寻找共同祖先并创建合并结果；不能 fast-forward 时通常产生带多个父提交的 merge commit。

```text
A---B---C main
     \   \
      D---M
```

它保留真实分叉关系，适合已经共享的分支。代价是历史可能出现更多合并节点。

## Rebase

rebase 把一组提交的补丁重新应用到新基底，产生新的 commit ID：

```text
原来：A---B---C main
          \
           D---E feature

之后：A---B---C---D''---E''
```

它让待审查历史线性、便于整理本地提交，但本质是历史改写。不要随意 rebase 已被他人基于其继续工作的共享提交；必须强推时优先 `--force-with-lease`，它仍需团队约定，不能保证没有协作风险。

## Cherry-pick

cherry-pick 把指定提交引入当前分支，也会创建新提交。适合把独立修复移植到维护分支，不适合代替日常分支同步，否则容易重复补丁和增加溯源难度。

| 需求 | 常用选择 |
|---|---|
| 保留已共享分叉历史 | merge |
| 整理尚未共享的本地提交 | interactive rebase |
| 移植少量独立提交 | cherry-pick |
| 撤销公共历史中的错误 | revert |

# 分支与交付策略

策略不是 Git 命令集合，而是团队对集成频率、评审、发布和回滚的约定。

## 短生命周期分支

从主干创建小分支，频繁同步，小批量评审，通过门禁后合并。它降低长期分叉与冲突成本，是个人项目和多数持续集成团队的稳健起点。

## Trunk-based development

开发者频繁集成到主干；未完成能力通过 feature flag、branch by abstraction 等方式保持不可见。它依赖快速测试、严格门禁和可回滚发布，不等于“所有人直接无检查推 main”。

## 长期发布分支

多版本维护或受监管发布可能需要 release/hotfix 分支。分支越多，补丁传播和版本矩阵成本越高。经典 Git Flow 是一种可选策略，不是现代项目的默认正确答案。

# 可审查提交

一个高质量提交应：

- 目的单一，构建和测试状态明确；
- 消息说明“为什么”，正文补充约束与影响；
- 不混入格式化、生成物或无关重命名；
- 不包含密码、密钥和个人数据；
- 大重构与行为修改尽量分开，降低 diff 噪声。

示例格式可采用：

```text
fix(parser): reject truncated frame header

The old path read payload length before proving that the full header
was available, which allowed an out-of-bounds read on partial input.
```

# 检查理解

1. 为什么 `git diff` 可能为空，但 `git diff --staged` 有内容？
2. rebase 后 commit ID 为什么变化？
3. `origin/main` 为什么不是服务器上的实时 main？
4. feature flag 如何减少长期分支，又引入哪些运行期治理成本？

> [!summary] 本篇结论
> Git 用对象图和引用管理快照；index 让提交内容可被精确组织。merge 保留拓扑，rebase 重写提交，cherry-pick 移植补丁。协作策略应服务小批量集成、可审查证据和安全回滚。

## 权威依据

- [Git reference manual](https://git-scm.com/docs/git)
- [Pro Git: Git Internals](https://git-scm.com/book/en/v2/Git-Internals-Git-Objects)

下一步：[02-Conflicts and Recovery (冲突与恢复)](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/02-Conflicts%20and%20Recovery%20(冲突与恢复).md)
