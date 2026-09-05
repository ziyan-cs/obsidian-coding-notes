---
tags:
  - devtools/git
status: 🌱
---

> [!important] **核心考点**：冲突何时产生、冲突标记含义、解决流程、高级合并策略

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
# 编辑冲突文件，解决所有 <<<< ==== >>>> 标记
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
# 相同冲突再次出现时自动应用上次方案
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

- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
- [reset vs revert vs restore (撤销三兄弟)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01d-reset%20vs%20revert%20vs%20restore%20(撤销三兄弟)%20⭐.md)
- [stash, tag, reflog (实用命令)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01e-stash,%20tag,%20reflog%20(实用命令).md)
- [CI⧸CD for C++：GitHub Actions, Static Analysis, Automation (CI⧸CD流水线)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
- [01b1-merge vs rebase vs cherry-pick (三种合并对比)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01b-Branch%20Model%20&%20Merge%20Strategies%20(分支策略与合并)/01b1-merge%20vs%20rebase%20vs%20cherry-pick%20(三种合并对比)%20⭐.md)
