#cpp #control-flow #if #loop #switch

## ⚡ TL;DR（快速决策）

- 流程控制本质是：**决定代码按什么顺序执行**
- 核心内容只有几类：
    - 条件判断：`if / else`
    - 循环：`for / while`
    - 分支：`switch`
    - 跳转：`break / continue / return`
- 一看到“分情况处理”“重复执行”，就进入流程控制思维
- 基础控制流写稳，后面所有算法代码才稳

## 🧩 Core Idea（核心本质）

- 默认代码是顺序执行
- 流程控制负责让程序：
    - 按条件走不同分支
    - 重复执行某段代码
    - 提前结束或跳过某些步骤
- 一句话理解：
    - **流程控制就是程序的行进路线图。**

## 🔧 Usage Patterns（可复用代码模板）

1. `if / else`

```cpp
if (x > 0) {
    cout << "positive\\n";
} else if (x == 0) {
    cout << "zero\\n";
} else {
    cout << "negative\\n";
}
```

1. `for` 循环

```cpp
for (int i = 0; i < 5; ++i) {
    cout << i << '\\n';
}
```

1. `while` 循环

```cpp
int x = 3;
while (x > 0) {
    cout << x << '\\n';
    --x;
}
```

1. `break` 和 `continue`

```cpp
for (int i = 0; i < 10; ++i) {
    if (i == 3) continue;
    if (i == 7) break;
}
```

## ⚠️ Pitfalls（高频错误）

- 条件写错成赋值：`=` 和 `==` 混淆
- 循环边界写错导致少一轮 / 多一轮
- `while` 忘记更新变量，造成死循环
- `break` 和 `continue` 作用别混
- 大括号省略后容易读错逻辑

## 🚀 Performance / Tips（性能优化）

- 先保证逻辑清晰，再谈简写
- 边界条件一定要单独检查
- 嵌套层数太深时，考虑拆函数
- 循环题里最常见 bug 基本都在边界

## 🧪 Common Scenarios（常见使用场景）

- 分类讨论
- 枚举遍历
- 多组输入处理
- 循环统计
- 提前终止搜索

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    for (int i = 1; i <= 5; ++i) {
        if (i % 2 == 0) cout << i << '\\n';
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **流程控制就是：通过判断、循环和跳转来决定程序执行路径。**