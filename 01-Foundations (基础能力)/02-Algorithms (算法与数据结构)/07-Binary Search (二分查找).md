---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [algorithm/search, algorithm/invariant, learning/foundation]
---

> [!abstract] 学习目标
> 把二分理解为在单调谓词上寻找边界；通过统一半开区间不变量处理精确查找、lower/upper bound 和答案二分。

# 二分的本质

二分不要求元素一定是数字，真正要求是搜索空间上的谓词具有单调分界：

```text
false false false | true true true
                  ↑ 第一个 true
```

每轮必须缩小搜索区间，并保持目标边界仍在其中。与其背多套 `+1/-1`，不如固定一种区间语义。

# 半开区间模板

在升序数组中找第一个 `>= target` 的位置，即 `lower_bound`：

```cpp
std::size_t lower_bound_index(const std::vector<int>& a, int target) {
    std::size_t left = 0;
    std::size_t right = a.size(); // 答案位于 [left, right]
    while (left < right) {
        const std::size_t mid = left + (right - left) / 2;
        if (a[mid] < target) left = mid + 1;
        else right = mid;
    }
    return left; // 可能等于 a.size()
}
```

循环中待搜索区间是 `[left, right)`；结束时区间为空，`left` 是分界点。由它可以推导：

- 第一个 `> target`：谓词改为 `a[mid] > target`（即 `upper_bound`）。
- 最后一个 `<= target`：`upper_bound(target) - 1`，但先检查结果是否为 0，避免无符号下溢。
- 精确查找：求 `lower_bound` 后检查位置未越界且值相等。
- 目标出现次数：`upper_bound - lower_bound`。

生产代码优先使用 `std::lower_bound`、`std::upper_bound` 和 ranges 版本；手写用于理解不变量或自定义搜索空间。

# 二分答案

当候选答案 `x` 可通过 `feasible(x)` 判断，并且可行性单调，就能二分第一个可行值：

```cpp
long long first_feasible(long long low, long long high) {
    // 约定答案存在于 [low, high]，high 必须可行
    while (low < high) {
        long long mid = low + (high - low) / 2;
        if (feasible(mid)) high = mid;
        else low = mid + 1;
    }
    return low;
}
```

关键不是模板，而是证明：

1. 搜索上下界覆盖答案；
2. `feasible` 单调；
3. 每轮严格缩区间；
4. 整数运算不会溢出；
5. 不存在答案时接口如何表示。

最大化最小值通常搜索“最后一个可行”，最小化最大值搜索“第一个可行”。装载能力、加工速度、分割阈值等题都属于这一模型。

# 旋转数组

无重复元素的旋转升序数组中，每轮至少有一半有序。先判断哪一半有序，再看目标是否位于其值域。存在大量重复值时，`a[left] == a[mid] == a[right]` 可能无法判断方向，只能收缩边界，最坏复杂度退化到 `O(n)`。

# 常见失败模式

- 混用闭区间与半开区间，导致越界或漏掉最后一个候选；
- `mid = (left + right) / 2` 在有符号大整数上溢出；
- 用无符号下标执行 `mid - 1`，在 0 处下溢；
- 浮点二分只按固定轮数却不分析精度，或用 `==` 判断结果；
- 谓词计算本身溢出，例如 `speed * time >= jobs`；应改写或使用更宽类型；
- 没有证明单调性，只因“看起来可二分”就套模板。

# 检查理解

1. `lower_bound` 返回 `size()` 表示什么？
2. 为什么“最后一个 `<= x`”可由 `upper_bound(x) - 1` 推导？
3. 答案二分中哪个函数必须单调，如何写出反例验证？
4. 旋转数组含重复值时为什么可能退化为线性？

> [!summary] 本篇结论
> 二分是对单调谓词分界的搜索。统一半开区间、明确循环不变量和不存在答案的语义，比记忆若干看似相似的模板可靠得多。

下一步：[08-Sorting Algorithms (排序算法)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/08-Sorting%20Algorithms%20(排序算法).md)
