---
tags:
  - devtools/git
status: learning
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Git Flow & Feature Branch Workflow — 团队协作流

> [!important] **核心考点**：Git Flow 五分支模型、Feature Branch Workflow、CI/CD 集成中的分支策略

> [!tip] 先选最小能保障质量的流程
> 对个人项目和多数小团队，短生命周期 feature branch + review + CI 往往足够。只有确有固定发版、并行维护等约束时，再承担 Git Flow 的额外分支成本。

### Feature Branch Workflow（功能分支工作流）

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

### Git Flow（经典发版模型）

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

### Trunk-Based Development（主干开发）

以短分支或直接向主干集成为特征的模式：

- 通过直接提交或极短生命周期 feature branch 保持与主干同步
- 用 Feature Flag 控制功能开关，而非靠分支隔离
- 配合完善的 CI/CD 保证主干质量
- 适合：高频发布、DevOps 成熟的团队

## 30 秒回答

分支策略的目的不是制造流程，而是让主干始终可集成、变更可审查、发布可追溯。个人项目可用短 feature branch；有固定发版节奏时 Git Flow 可能更清晰；高频交付团队可采用 trunk-based。选择依据是发布节奏、CI 可靠度和团队协作成本。

## 自测

1. Git Flow 为什么可能不适合一个快速迭代的小项目？
2. feature branch 合并前，最少要有哪些质量信号？
3. 什么时候 feature flag 比长期分支更合适？

---

## 关联笔记

- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [stash, tag, reflog (实用命令)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
