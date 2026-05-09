
> 核心考点：三者的作用范围、是否改写历史、适用场景

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

bash

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