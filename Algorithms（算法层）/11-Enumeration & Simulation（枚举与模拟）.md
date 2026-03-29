#algorithm #enumeration #simulation #bruteforce #implementation

## ⚡ TL;DR（快速决策）

- **枚举**：把可能情况按某种顺序一个个试出来
- **模拟**：按照题目给定规则，把过程老老实实写出来
- 这两类题往往不靠“高深算法”，而靠：
    - 分类是否完整
    - 过程是否写对
    - 细节是否处理干净
- 一看到这些特征，要优先想到枚举 / 模拟：
    - 数据范围不大
    - 题目规则很多、描述长
    - 问你“是否存在 / 最优值 / 合法方案”但可以直接尝试
    - 让你按步骤更新状态
    - 涉及日期、时钟、棋盘、字符串变换、回合制过程
- 枚举的核心是：**不重不漏**
- 模拟的核心是：**按题意还原过程，不自作聪明改规则**
- 很多题其实是：**枚举候选答案 + 模拟验证**

## 🧩 Core Idea（核心本质）

- 枚举：
    - 本质是把“所有可能情况”系统地遍历一遍
    - 关键不是暴力，而是**怎么枚举更合理**
- 模拟：
    - 本质是把题目叙述中的操作流程，翻译成程序执行流程
    - 关键不是技巧，而是**状态设计是否清楚**
- 两者关系：
    - 枚举负责“试哪些情况”
    - 模拟负责“每种情况怎么判断 / 怎么跑过程”
- 常见思维模型：
    - 枚举一个位置 / 一个答案 / 一个分界点
    - 枚举两个端点 / 两个元素 / 两个状态
    - 枚举操作起点，然后模拟后续过程
    - 按时间顺序 / 规则顺序一步步更新状态
- 一句话理解：
    - **枚举是在遍历可能，模拟是在执行规则。**

## 🔧 Usage Patterns（可复用代码模板）

1. 单层枚举

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums = {3, 1, 4, 1, 5};

    for (int i = 0; i < (int)nums.size(); ++i) {
        cout << "下标 " << i << ", 值 = " << nums[i] << '\\n';
    }

    return 0;
}
```

要点：

- 枚举对象通常是：位置、元素、答案、起点、长度
- 先想清楚“到底在枚举什么”

1. 双层枚举（区间 / 配对类高频）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums = {1, 2, 3, 4};

    for (int i = 0; i < (int)nums.size(); ++i) {
        for (int j = i + 1; j < (int)nums.size(); ++j) {
            cout << "配对: " << nums[i] << ", " << nums[j] << '\\n';
        }
    }

    return 0;
}
```

要点：

- 常用于：两数关系、区间枚举、子串枚举
- 重点是避免重复和漏掉情况

1. 枚举答案 + 检查函数

```cpp
#include <iostream>
using namespace std;

bool check(int x) {
    return x % 7 == 0 && x % 9 == 0;
}

int main() {
    for (int ans = 1; ans <= 1000; ++ans) {
        if (check(ans)) {
            cout << ans << '\\n';
            break;
        }
    }
    return 0;
}
```

要点：

- 这是非常经典的写法：
    - 主函数负责枚举
    - `check()` 负责验证
- 好处是结构清晰，特别适合考试和面试

1. 字符串模拟

```cpp
#include <iostream>
#include <string>
using namespace std;

string toUpperCase(string s) {
    for (char& c : s) {
        if ('a' <= c && c <= 'z') {
            c = c - 'a' + 'A';
        }
    }
    return s;
}

int main() {
    cout << toUpperCase("abCde") << '\\n';
    return 0;
}
```

要点：

- 字符串题常是模拟题重灾区
- 每次只改一个字符 / 一个位置，最稳

1. 网格模拟

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<string> grid = {
        "...#.",
        "..#..",
        "....."
    };

    int x = 0, y = 0;
    string ops = "RRDD";

    for (char op : ops) {
        int nx = x, ny = y;
        if (op == 'U') --nx;
        if (op == 'D') ++nx;
        if (op == 'L') --ny;
        if (op == 'R') ++ny;

        if (0 <= nx && nx < (int)grid.size() &&
            0 <= ny && ny < (int)grid[0].size() &&
            grid[nx][ny] != '#') {
            x = nx;
            y = ny;
        }
    }

    cout << x << ' ' << y << '\\n';
    return 0;
}
```

要点：

- 网格模拟的关键：
    - 方向数组 / 方向判断
    - 越界判断
    - 障碍判断
    - 状态更新顺序

1. 日期 / 时钟模拟

```cpp
#include <iostream>
using namespace std;

int main() {
    int hour = 23, minute = 58;

    for (int step = 0; step < 5; ++step) {
        ++minute;
        if (minute == 60) {
            minute = 0;
            ++hour;
        }
        if (hour == 24) hour = 0;

        cout << hour << ':';
        if (minute < 10) cout << '0';
        cout << minute << '\\n';
    }

    return 0;
}
```

要点：

- 这种题本质不是难，而是容易在进位 / 边界上出错
- 遇到日期题要特别注意闰年、每月天数

1. 枚举子数组 / 子串

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums = {1, 2, 3};

    for (int l = 0; l < (int)nums.size(); ++l) {
        for (int r = l; r < (int)nums.size(); ++r) {
            cout << '[' << l << ", " << r << "]\\n";
        }
    }

    return 0;
}
```

要点：

- 区间题的经典起手式
- 后面可以再在区间内部统计或模拟

1. 回合制 / 事件流模拟

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int hp = 10;
    vector<int> damage = {2, 3, 4, 1};

    for (int i = 0; i < (int)damage.size(); ++i) {
        hp -= damage[i];
        cout << "第 " << i + 1 << " 回合后 hp = " << hp << '\\n';
        if (hp <= 0) {
            cout << "角色被击败\\n";
            break;
        }
    }

    return 0;
}
```

要点：

- 按顺序处理事件
- 每一步都更新状态
- 很适合拆成“输入 → 状态 → 操作 → 输出”

## ⚠️ Pitfalls（高频错误）

- 枚举题最常见错误：
    - 漏情况
    - 重复枚举
    - 循环边界写错
- 模拟题最常见错误：
    - 没完全按题意写
    - 自己脑补了简化规则
    - 多个状态更新顺序写反
- 写枚举时一定先问自己：
    - 我枚举的对象是什么？
    - 是否已经覆盖所有情况？
    - 是否有重复？
- 写模拟时一定先问自己：
    - 当前有哪些状态量？
    - 每一步谁先变、谁后变？
    - 边界条件是什么？
- 网格题高频错误：
    - 行列搞反
    - 越界判断漏掉 `=`
    - 修改当前位置前没先判断新位置是否合法
- 字符串题高频错误：
    - 下标越界
    - 字符和字符串混用
    - 忘记恢复现场（如果有试填 / 回退）
- 日期题高频错误：
    - 闰年判断写错
    - 2 月天数忘记特殊处理
    - 月末 / 年末进位出错
- 当数据范围很大时，纯枚举可能超时
    - 这时要考虑剪枝、预处理、数学规律、前缀和、双指针、哈希等优化

## 🚀 Performance / Tips（性能优化）

- 复杂度要先估：
    - 单层枚举常是 $O(n)$
    - 双层枚举常是 $O(n^2)$
    - 三层枚举常是 $O(n^3)$
- 判断能不能直接枚举，先看数据范围：
    - $n le 10^2$：很多 $O(n^2)$、$O(n^3)$ 都有机会
    - $n le 10^5$：一般不能乱双层 / 三层枚举
- 优化枚举的常见方式：
    - 固定一个量，快速计算另一个量
    - 把重复计算提出来做前缀和 / 预处理
    - 用 `check()` 封装逻辑，便于调试
    - 找到明显不可能的情况后尽早 `continue` / `break`
- 模拟题优化重点常不在算法，而在代码结构：
    - 用清晰变量名
    - 拆函数
    - 统一边界处理
- 经验判断：
    - 题意长、规则细、范围不大：大概率是模拟
    - 要求“找所有可能 / 枚举所有位置”：大概率是枚举
    - 有时正解不是高级算法，而是**认真实现**

## 🧪 Common Scenarios（常见使用场景）

- **枚举答案**：最小满足条件值、合法数字、分界点
- **枚举位置**：枚举删除点、交换点、起点、终点
- **枚举区间**：子数组、子串、区间统计
- **枚举配对**：两元素关系、两位置关系
- **过程模拟**：
    - 日期推进
    - 时钟变化
    - 回合战斗
    - 机器人移动
    - 队列 / 栈操作过程
- **字符串模拟**：替换、压缩、展开、转换
- **棋盘 / 网格模拟**：走迷宫、落子、扩散、移动规则

## 🆚 Enumeration & Simulation vs 其他常见思想

- **vs Brute Force（暴力）**
    - 枚举通常是暴力的一种具体实现方式
    - 但“会枚举”不等于“乱暴力”，关键是有序、不重不漏
- **vs Greedy（贪心）**
    - 枚举 / 模拟：把情况试出来 / 把过程跑出来
    - 贪心：每步直接做局部最优选择
- **vs Search / Backtracking（搜索 / 回溯）**
    - 枚举更偏直接遍历固定范围
    - 回溯更偏决策树式尝试与撤销
- **vs Implementation（实现题）**
    - 模拟题很多本质上就是实现题
    - 区别只是模拟更强调“按规则推进状态”

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

bool check(const vector<int>& nums, int l, int r) {
    int sum = 0;
    for (int i = l; i <= r; ++i) sum += nums[i];
    return sum % 2 == 0;
}

int main() {
    vector<int> nums = {1, 2, 3, 4};

    for (int l = 0; l < (int)nums.size(); ++l) {
        for (int r = l; r < (int)nums.size(); ++r) {
            if (check(nums, l, r)) {
                cout << "找到一个合法区间: [" << l << ", " << r << "]\\n";
            }
        }
    }

    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **枚举是把可能情况系统地试出来，模拟是把题目规则一步步执行出来。**