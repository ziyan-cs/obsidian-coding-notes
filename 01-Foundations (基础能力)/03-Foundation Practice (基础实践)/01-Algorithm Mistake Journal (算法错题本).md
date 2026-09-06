---
tags:
  - cs/algorithm
status: 🌱
---

> [!abstract] 核心考点：> 高频易错题整理、边界条件与 corner case 总结、常见陷阱复盘

# 30 秒回答

**核心结论**：核心考点：> 高频易错题整理、边界条件与 corner case 总结、常见陷阱复盘

# 使用说明

按题型分类记录刷题中反复出错的题目和典型的易错点。定期回顾可有效避免重复踩坑。

# 边界条件 Checklist

- [ ] 输入为空（数组长度为 0、字符串为 ""）
- [ ] 输入只有一个元素
- [ ] 数组有序/无序的不同处理
- [ ] 整数溢出（`l + r` 可能溢出，用 `l + (r-l)/2`）
- [ ] 指针/迭代器为空或为 end()
- [ ] 索引越界（`nums[i-1]` 当 i=0 时）
- [ ] 除零错误
- [ ] 递归栈溢出（n 太大时）

# 常见陷阱速查

## 数组/字符串
- 循环内修改数组要小心索引偏移（`erase` 后 i 应回退）
- 两数之和用哈希表时注意重复元素
- 子数组连续 vs 子序列不连续
- `size()` 返回 `size_t`，与负数比较会出错（`s.size() > -1` 永远为 false）

## 链表
- 涉及 `next` 操作前先判断节点是否非空
- 反转链表要保存 `next` 临时节点
- 快慢指针找中点的循环条件
- 递归/迭代选择：递归直观但可能栈溢出

## 树/图
- 二叉树递归时要明确空节点的返回值
- BFS 的 visited 在**入队时标记**而不是出队时（否则重复入队）
- 拓扑排序用 Kahn 算法（BFS）
- union-find 记得路径压缩

## 二分查找
- `l <= r` vs `l < r` 决定搜索空间的开闭
- 求右边界时 `mid = (lo + hi + 1) / 2` 避免死循环
- 二分答案需验证单调性

## 动态规划
- 初始化 `dp[0]` 或 `dp[0][0]` 要仔细
- 滚动数组注意每一轮要重置
- 背包问题 0-1 是逆序，完全背包是正序
- 区间 DP 长度递增枚举

## 回溯
- 回溯完要撤销选择（`pop_back`）
- 剪枝条件写对能大幅提速
- 去重通常先排序 + 同层跳过

# 易错记录模板

```
## [日期] 题目名称
- 错误类型：索引越界 / 边界漏考虑 / 思路错误
- 错误原因：
- 正确解法要点：
- 下次注意：
```

---

# 关联笔记

- [Problem Template Library (常用模板总库)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/05-Problem-Solving%20Practice%20(刷题与复盘)/00-Problem%20Template%20Library%20(常用模板总库)%20⭐.md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)

# 零基础阅读路径

先阅读问题与最小例子，再理解机制与边界，最后做验证或练习。无法复述因果关系时，先回到输入、状态和输出。

# 常见误区

- 把 **01-Algorithm Mistake Journal (算法错题本)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Algorithm Mistake Journal (算法错题本)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
