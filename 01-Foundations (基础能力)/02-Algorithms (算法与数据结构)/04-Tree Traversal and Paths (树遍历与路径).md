---
study_stage: backlog
tags: [algorithm/tree, algorithm/traversal, learning/foundation]
---

> [!abstract] 学习目标
> 用“节点语义、递归返回值、进入/退出时机”统一二叉树 DFS、BFS、路径与最近公共祖先问题，并能把递归可靠地改写为迭代。

# 先定义节点与空树语义

```cpp
struct TreeNode {
    int value;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};
```

树算法首先约定：空树的高度、节点数、路径和、是否允许空路径分别是什么。边界定义不清会让递归基例与最终答案互相矛盾。

# 深度优先遍历

前序、中序、后序的区别是**处理当前节点的时机**：

```text
前序：节点 → 左 → 右    适合自顶向下传状态、序列化
中序：左 → 节点 → 右    BST 中得到有序键
后序：左 → 右 → 节点    适合汇总子树结果、释放节点
```

## 递归的两种设计

1. **参数承载路径状态**：当前深度、前缀和、路径内容；进入节点时更新，返回时回溯。
2. **返回值承载子树答案**：高度、节点数、是否平衡、可向父节点延伸的最佳路径。

例如树高：

```cpp
int height(const TreeNode* node) {
    if (node == nullptr) return 0;
    return 1 + std::max(height(node->left), height(node->right));
}
```

时间 `O(n)`；递归空间是树高 `O(h)`，退化链上可达 `O(n)` 并触发栈溢出。

## 迭代遍历

前序可直接用栈，先压右再压左。中序需要一路压入左链，弹出后转向右子树。后序可使用“节点 + 是否展开”状态，避免依赖易错的上次访问指针技巧：

```cpp
std::vector<int> postorder(TreeNode* root) {
    std::vector<int> result;
    if (root == nullptr) return result;
    std::vector<std::pair<TreeNode*, bool>> stack{{root, false}};

    while (!stack.empty()) {
        auto [node, expanded] = stack.back();
        stack.pop_back();
        if (expanded) {
            result.push_back(node->value);
            continue;
        }
        stack.push_back({node, true});
        if (node->right) stack.push_back({node->right, false});
        if (node->left) stack.push_back({node->left, false});
    }
    return result;
}
```

# 广度优先与层序

BFS 用队列按层处理。若需要层边界，在每轮开始保存 `level_size = q.size()`，只处理这一批节点；循环过程中新增节点属于下一层。

层序适合最短层数、右视图、每层聚合和完全二叉树问题。普通树上若只做完整遍历，DFS/BFS 都是 `O(n)`；区别主要在访问顺序和辅助空间形态。

# 路径问题

“路径”必须先问清：

- 起点/终点是否必须是根或叶子？
- 能否从子节点回到父节点再向另一分支？
- 求是否存在、条数、最大值，还是恢复具体路径？
- 节点值是否可能为负？

根到叶路径常用回溯；任意两点最大路径和通常用后序：返回“可向父节点延伸的单边最大贡献”，同时用全局/外部状态更新“经过当前节点的双边答案”。负贡献应按题意舍弃。

# 最近公共祖先

在普通二叉树中，若保证 `p`、`q` 都存在：

```cpp
TreeNode* lca(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (root == nullptr || root == p || root == q) return root;
    TreeNode* left = lca(root->left, p, q);
    TreeNode* right = lca(root->right, p, q);
    if (left && right) return root;
    return left ? left : right;
}
```

返回值表示“当前子树中找到的目标或其 LCA”。若目标可能不存在，需要同时返回找到目标的数量，不能把找到一个目标误报为 LCA。BST 可利用键的有序性缩小方向，但重复键会使按值定位失去唯一性。

# 工程与测试

- 明确节点所有权。示例裸指针只表达算法结构，生产代码应由容器、arena 或智能指针管理生命周期。
- 不要默认递归总安全；不受控深度的数据应考虑显式栈或深度限制。
- 测试空树、单节点、只有左/右链、完全树、重复值、极深树和负权路径。
- 恢复路径时保存父指针或在回溯中复制当前路径，注意复杂度差异。

# 检查理解

1. 为什么后序遍历适合计算高度和平衡性？
2. 递归复杂度中的 `O(h)` 指什么，何时退化为 `O(n)`？
3. LCA 在目标可能缺失时为何需要额外状态？
4. 任意路径最大和为何不能直接把左右子树最优值都返回给父节点？

> [!summary] 本篇结论
> 树题的统一模型是：进入节点时获得自顶向下状态，退出节点时汇总子树返回值。先定义路径和空树语义，再选择 DFS 的处理时机或 BFS 的层次顺序。

下一步：[05-Binary Search Trees (二叉搜索树)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/05-Binary%20Search%20Trees%20(二叉搜索树).md)
