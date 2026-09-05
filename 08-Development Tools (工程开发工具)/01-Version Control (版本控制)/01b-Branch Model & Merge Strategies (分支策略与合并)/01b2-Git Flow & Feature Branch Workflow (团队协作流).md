---
tags:
  - devtools/git
status: 🌱
---

> [!important] **核心考点**：Git Flow 五分支模型、Feature Branch Workflow、CI/CD 集成中的分支策略

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

### Trunk-Based Development（主干开发，现代替代）

Google、Meta 等大厂采用的模式：

- 所有人直接提交到 main（或通过极短命 feature branch，<1天）
- 用 Feature Flag 控制功能开关，而非靠分支隔离
- 配合完善的 CI/CD 保证主干质量
- 适合：高频发布、DevOps 成熟的团队

---

## 关联笔记

- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [Conflict Resolution (冲突解决实操)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01c-Conflict%20Resolution%20(冲突解决实操)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [stash, tag, reflog (实用命令)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
