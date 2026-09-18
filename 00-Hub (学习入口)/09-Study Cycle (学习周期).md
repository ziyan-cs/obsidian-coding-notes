---
status: learning
confidence: high
content_verified: 2026-09-18
tags: [learning/execution, learning/dashboard]
---

> [!abstract] 唯一任务面板
> 全库的具体日期、当前阶段和临时任务只在本页统筹。知识模块只保存机器可读的 `verified`、`review_stage` 与 `review_due`，不再散落周计划。

# 每日入口

```powershell
.\List-ReviewDue.ps1
.\List-ReviewDue.ps1 -All
```

默认视图显示逾期、当天和明天；`-All` 查看完整首月队列。当天先处理到期项，再开始新笔记。

- [ ] 不看正文主动回忆到期笔记；一次不超过 3 篇。
- [ ] 完成当天新学主题的最小代码或图示。
- [ ] 保存一项证据：测试输出、调试记录、sanitizer 报告或口述提纲。
- [ ] 按表现更新 `review_stage` 与 `review_due`。

# 2026-09-23 至 2026-10-22

首月只有一条主线：**C++ 对象与资源模型 → 现代 C++ → 构建与诊断 → 系统基础**。算法由既有 LeetCode / Codeforces 时段维护；Python、Go、数据库、云原生、网安和 AI 暂不并开。

## 第一周：建立资源模型

- [ ] **09-23 周三 21:00–22:30**：先用 C++ 模块标准做 15 分钟诊断；学习 [Headers Namespaces and Separate Compilation (头文件命名空间与分离编译)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/01-Language%20Basics%20(语言基础)/03-Headers%20Namespaces%20and%20Separate%20Compilation%20(头文件命名空间与分离编译).md)，画出声明、定义、编译单元与链接关系。
- [ ] **09-24 周四 09:00–12:00**：学习 [Types Pointers and References (类型指针与引用)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/01-Types%20Pointers%20and%20References%20(类型指针与引用).md)，用小程序验证 const、引用绑定和悬空风险。
- [ ] **09-25 周五 12:30–13:50**：只做 D1/D2 主动回忆和错题修正，不开新篇。
- [ ] **09-26 周六 13:30–17:00**：学习 [Memory Layout and Allocation (内存布局与分配)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)，观察栈、堆、对齐与分配行为。
- [ ] **09-27 周日 13:30–17:00**：学习 [Object Lifetime and Copy Control (对象生命周期与拷贝控制)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)，写一个可打印构造、拷贝、移动、析构次序的类型；20:30 做周复盘。

## 第二周：完成所有权闭环

- [ ] **09-28 周一 20:45–22:30**：学习 [Polymorphism and Inheritance (多态与继承)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/04-Polymorphism%20and%20Inheritance%20(多态与继承).md)，重点解释虚析构、对象切片与组合优先。
- [ ] **09-29 周二**：不学新篇，只处理脚本列出的到期复习；课程与 Linux/Shell Lab 已占满主要精力。
- [ ] **09-30 周三 21:00–22:30**：学习 [Templates (模板)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)，写一个有约束意识的泛型函数。
- [ ] **10-01 周四 09:00–12:00**：学习 [RAII and Custom Allocation (RAII 与自定义分配)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)，开始实现 RAII 资源封装。
- [ ] **10-02 周五 12:30–13:50**：复述所有权链条，补失败路径测试，不开新篇。
- [ ] **10-03 周六 13:30–17:00**：学习 [Modern C++ Foundations (现代 C++ 基础)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C%2B%2B%20(现代%20C%2B%2B)/01-Modern%20C%2B%2B%20Foundations%20(现代%20C%2B%2B%20基础).md)，把 `auto`、lambda、range-for 和初始化规则用于小组件。
- [ ] **10-04 周日 13:30–17:00**：不学新篇；完成 RAII 组件第一版、测试与运行说明，20:30 做周复盘。

## 第三周：现代 C++ 与构建

- [ ] **10-05 周一 20:45–22:30**：学习 [Value Categories and Move (值类别与移动语义)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C%2B%2B%20(现代%20C%2B%2B)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)，用日志观察 copy 与 move。
- [ ] **10-06 周二**：只复习，不开新篇。
- [ ] **10-07 周三 21:00–22:30**：学习 [Smart Pointers (智能指针)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C%2B%2B%20(现代%20C%2B%2B)/03-Smart%20Pointers%20(智能指针).md)，画出所有权图并构造一次循环引用。
- [ ] **10-08 周四 09:00–12:00**：先学习短篇 [Perfect Forwarding and Universal References (完美转发与万能引用)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C%2B%2B%20(现代%20C%2B%2B)/04-Perfect%20Forwarding%20and%20Universal%20References%20(完美转发与万能引用).md)，再学习 [CMake Project and Targets (CMake 项目与目标)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md)，把组件改为 target-based 构建。
- [ ] **10-09 周五 12:30–13:50**：复查编译命令、include 传播和链接错误，不开新篇。
- [ ] **10-10 周六 13:30–17:00**：学习 [CMake Dependencies (CMake 依赖)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/02-CMake%20Dependencies%20(CMake%20依赖).md)，加入测试依赖并验证全新目录可构建。
- [ ] **10-11 周日 13:30–17:00**：学习 [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)，为正常路径、边界和失败路径补测试；20:30 周复盘。

## 第四周：诊断与程序执行

- [ ] **10-12 周一 20:45–22:30**：学习 [Debugging and Failure Localization (调试与故障定位)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/02-Debugging%20and%20Failure%20Localization%20(调试与故障定位).md)，保留一次从症状到根因的调试记录。
- [ ] **10-13 周二**：只复习，不开新篇。
- [ ] **10-14 周三 21:00–22:30**：学习 [Memory Safety and Dynamic Analysis (内存安全与动态分析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/03-Memory%20Safety%20and%20Dynamic%20Analysis%20(内存安全与动态分析).md)，用 ASan/UBSan 主动制造并定位错误。
- [ ] **10-15 周四 09:00–12:00**：学习 [Program Build and Execution (程序构建与执行)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)，串起预处理、编译、链接、装载与运行时。
- [ ] **10-16 周五 12:30–13:50**：复盘构建与诊断链，不开新篇。
- [ ] **10-17 周六 13:30–17:00**：学习 [Processes and Threads (进程与线程)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/06-Processes%20and%20Threads%20(进程与线程).md)，用进程地址空间和线程共享关系解释程序行为。
- [ ] **10-18 周日 13:30–17:00**：不学新篇；完成 RAII 组件第二版、sanitizer 与测试报告，20:30 做周复盘。

## 收尾：并发与内存桥接

- [ ] **10-19 周一 20:45–22:30**：学习 [Synchronization Primitives (同步原语)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/10-Synchronization%20Primitives%20(同步原语).md)，重点掌握不变量、临界区、条件等待和锁顺序。
- [ ] **10-20 周二**：只做累计复习与错题清理，不开新篇。
- [ ] **10-21 周三 21:00–22:30**：做首月闭卷串讲：对象生命周期 → RAII → move → 构建 → 运行时 → 线程同步；补齐项目 README 和运行证据。
- [ ] **10-22 周四 09:00–12:00**：学习 [Virtual Memory Paging and Allocation (虚拟内存、分页与分配)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)，完成首月验收并决定下一周期。

# 学完后的属性更新

首次排期时：

```yaml
verified: 2026-09-23
review_stage: learn
review_due: 2026-09-23
```

当天真正学完后，`verified` 保留首次完成日，`review_stage` 改为 `D1`，`review_due` 改为次日。若延期，先把 `verified` 改成真实完成日，再计算后续间隔；不得为了消除逾期而假装完成。

通过复习后依次推进 `D1 → D3 → D7 → D14 → D30 → D60 → solid`。答不出机制、边界或写不出最小代码时，退回 `D1`，次日再测。进入 `solid` 后移除 `review_due`，保留 `verified` 和 `review_stage: solid`。

# 首月验收

- [ ] 能不看笔记解释对象生命周期、copy/move、RAII、智能指针和异常安全。
- [ ] 能从预处理讲到进程运行，并解释地址空间、线程共享与同步不变量。
- [ ] RAII 小组件可在干净目录构建，具备测试、失败用例和 sanitizer 证据。
- [ ] 到期复习没有通过“改日期”伪完成；错误已进入 Mistake Log。
- [ ] 能用 5～8 分钟完整讲述首月知识链，而不是逐篇背标题。

# 调整规则

- 学习日不足 90 分钟：只做复习加一个最小实验；新篇顺延，后续日期整体移动，不叠加补课。
- 连续两次实践失败：缩小产物，不增加主题。
- 到期项超过 5 篇：优先当前主线、真实错误和项目阻塞项，其余顺延。
- 周二、周五默认不学新篇；周日优先输出，避免把阅读量当进度。
- 本页是唯一日程真相源；模块文件只保存脚本所需属性。

# 系统边界

- 长期能力顺序：[Backend Learning Roadmap (后端学习路线)](/00-Hub%20(学习入口)/01-Backend%20Learning%20Roadmap%20(后端学习路线).md)
- 复习间隔与更新规则：[Spaced Review System (间隔复习系统)](/00-Hub%20(学习入口)/02-Spaced%20Review%20System%20(间隔复习系统).md)
- 模块验收：各一级目录的 `00-... Standard`
- 错误记录：[Mistake Log (错误记录)](/00-Hub%20(学习入口)/05-Mistake%20Log%20(错误记录).md)
