
> 核心考点：冲突何时产生、冲突标记含义、解决流程

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

## 实用工具

```bash
git mergetool               # 打开图形化合并工具（vimdiff / vscode 等）
git checkout --ours file    # 直接取当前分支版本
git checkout --theirs file  # 直接取对方分支版本
git diff --conflict=diff3   # 显示三方对比（共同祖先 + 两方改动）
```