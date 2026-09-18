---
status: learning
confidence: high
verified: 2026-09-18
tags: [learning/execution, learning/dashboard]
---

> [!abstract] 唯一任务面板
> 本页把课程、项目和自由学习时间统一为一套可执行节奏。知识模块只保存脚本需要的学习属性，不再各自维护周计划。

# 每日入口

```powershell
.\List-ReviewDue.ps1
.\List-ReviewDue.ps1 -All
```

默认显示逾期、当天和明天；`-All` 查看完整首月队列。开始新内容前，先处理当天到期复习。

# 一周固定节奏

| 星期 | 可用时段 | 主任务 | 负荷边界 |
| --- | --- | --- | --- |
| 周一 | 20:45–22:30；下午另有项目推进 | C++ 中等篇，晚上理解、次日项目中应用 | 一篇，不追加系统新课 |
| 周二 | 课程密集，另有 Linux/Shell Lab | 20～30 分钟到期复习 | 不开新篇 |
| 周三 | 21:00–22:30；下午项目推进 | Python 基础 | 一篇加最小可运行示例 |
| 周四 | 09:00–12:00 | C++ 重篇与调试实验 | 一篇重文，预留至少 60 分钟编码 |
| 周五 | 12:30–13:50；14:00 Python Lab | Python 基础与实验衔接 | 一篇，直接带入 Lab |
| 周六 | 13:30–17:00 | 计算机组成/系统基础与实验 | 一篇重文，必须有观察证据 |
| 周日 | 13:30–17:00；20:30 周复盘 | 复习、短实践笔记、项目输出 | 最多一篇短文；先复习再新增 |

三条线不是互相争抢时间：C++ 训练资源与工程模型，Python 提供自动化和验证能力，计算机组成与系统基础解释程序为何这样运行。强化版首月共 28 篇；周日先复习，再用一篇短实践笔记把三者连接到同一个产物。

# 2026-09-23 至 2026-10-22

## 第一周：建立三条线的共同坐标

- [ ] **09-23 周三 21:00–22:30 · Python**：学习 [Runtime Modules Packages and uv (运行环境、模块与包管理)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/01-Runtime%20Modules%20Packages%20and%20uv%20(运行环境、模块与包管理).md)，建立隔离环境并运行一个模块化脚本。
- [ ] **09-24 周四 09:00–12:00 · C++**：先学习 [Headers Namespaces and Separate Compilation (头文件命名空间与分离编译)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/01-Language%20Basics%20(语言基础)/03-Headers%20Namespaces%20and%20Separate%20Compilation%20(头文件命名空间与分离编译).md)，再学习 [Types Pointers and References (类型指针与引用)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/01-Types%20Pointers%20and%20References%20(类型指针与引用).md)；用多文件程序验证声明、定义、const、引用与悬空风险。
- [ ] **09-25 周五 12:30–13:50 · Python**：学习 [Core Syntax Collections and Comprehensions (核心语法、容器与推导式)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/03-Core%20Syntax%20Collections%20and%20Comprehensions%20(核心语法、容器与推导式).md)，在 Python Lab 中完成一次文本统计。
- [ ] **09-26 周六 13:30–17:00 · 计算机基础**：学习 [Computer System Overview (计算机系统总览)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/01-Computer%20System%20Overview%20(计算机系统总览).md)，画出源代码到 CPU、内存、I/O 的全链路。
- [ ] **09-27 周日 13:30–17:00 · 综合实践**：前 60 分钟闭卷复习；学习 [Python Project Practice Workflow (Python 项目实践流程)](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/01-Python%20Project%20Practice%20Workflow%20(Python%20项目实践流程).md)，随后用 Python 读取 C++ 程序输出并保存环境、命令和结果；20:30 周复盘。

## 第二周：名称、内存与数据表示

- [ ] **09-28 周一 20:45–22:30 · C++**：学习 [Memory Layout and Allocation (内存布局与分配)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)，聚焦栈、堆、对齐和分配主线，实验延续到项目时段。
- [ ] **09-29 周二**：只运行近期待复习；不新增笔记。
- [ ] **09-30 周三 21:00–22:30 · Python**：学习 [Objects Names Mutability and Copying (对象、名称、可变性与复制)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/02-Objects%20Names%20Mutability%20and%20Copying%20(对象、名称、可变性与复制).md)，对照 C++ 值、引用与生命周期。
- [ ] **10-01 周四 09:00–12:00 · C++**：学习 [Object Lifetime and Copy Control (对象生命周期与拷贝控制)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)，记录构造、拷贝、移动和析构顺序。
- [ ] **10-02 周五 12:30–13:50 · Python**：学习 [Functions Scope Closures and Decorators (函数、作用域、闭包与装饰器)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/04-Functions%20Scope%20Closures%20and%20Decorators%20(函数、作用域、闭包与装饰器).md)，在 Lab 中实现计时装饰器。
- [ ] **10-03 周六 13:30–17:00 · 计算机组成**：学习 [Data Representation (数据表示)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Data%20Representation%20(数据表示).md)，用 C++ 与 Python 对照整数溢出、浮点误差、字节序和文本编码。
- [ ] **10-04 周日 13:30–17:00 · 综合实践**：先整理“名称—对象—字节”对照表；学习 [Automation Project Template (自动化项目模板)](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/02-Automation%20Project%20Template%20(自动化项目模板).md)，建立首月实验仓架构并修正错题。

## 第三周：生命周期、迭代与构建链

- [ ] **10-05 周一 20:45–22:30 · C++**：学习 [Modern C++ Foundations (现代 C++ 基础)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C%2B%2B%20(现代%20C%2B%2B)/01-Modern%20C%2B%2B%20Foundations%20(现代%20C%2B%2B%20基础).md)，把初始化、`auto`、lambda 和 range-for 用进小项目。
- [ ] **10-06 周二**：只复习，不新增笔记。
- [ ] **10-07 周三 21:00–22:30 · Python**：学习 [Iteration Generators and Protocols (迭代、生成器与协议)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/05-Iteration%20Generators%20and%20Protocols%20(迭代、生成器与协议).md)，写一个惰性读取日志的生成器。
- [ ] **10-08 周四 09:00–12:00 · C++**：学习 [Value Categories and Move (值类别与移动语义)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C%2B%2B%20(现代%20C%2B%2B)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)，用日志区分 copy、move 和移动后状态。
- [ ] **10-09 周五 12:30–13:50 · Python**：学习 [Classes Dataclasses and Object Model (类、数据类与对象模型)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/06-Classes%20Dataclasses%20and%20Object%20Model%20(类、数据类与对象模型).md)，与 C++ 对象模型做边界对照。
- [ ] **10-10 周六 13:30–17:00 · 计算机组成**：学习 [Program Build and Execution (程序构建与执行)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)，观察预处理、编译、链接、装载和运行时产物。
- [ ] **10-11 周日 13:30–17:00 · 综合实践**：先完成本周复习；学习 [Type Hints Logging and Linting (类型日志与静态检查)](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/02-Type%20Hints%20Logging%20and%20Linting%20(类型日志与静态检查).md)，再输出“Python 解释执行与 C++ 编译链接”同异图和失败案例。

## 第四周：所有权、文件与存储层级

- [ ] **10-12 周一 20:45–22:30 · C++**：学习 [Templates (模板)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)，为实验工具写一个小型泛型组件并明确实例化边界。
- [ ] **10-13 周二**：只复习，不新增笔记。
- [ ] **10-14 周三 21:00–22:30 · Python**：学习 [Paths Files Serialization and Time (路径、文件、序列化与时间)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/07-Paths%20Files%20Serialization%20and%20Time%20(路径、文件、序列化与时间).md)，写一个跨平台结果采集脚本。
- [ ] **10-15 周四 09:00–12:00 · C++**：学习 [RAII and Custom Allocation (RAII 与自定义分配)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)，实现并测试一个资源封装。
- [ ] **10-16 周五 12:30–13:50 · Python**：学习 [Exceptions Context Managers and Typing (异常、上下文与类型)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/08-Exceptions%20Context%20Managers%20and%20Typing%20(异常、上下文与类型).md)，用 context manager 管理测试资源。
- [ ] **10-17 周六 13:30–17:00 · 计算机组成**：学习 [Memory Hierarchy and IO (存储层级与输入输出)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)，完成局部性与顺序/随机访问对照实验。
- [ ] **10-18 周日 13:30–17:00 · 综合实践**：先完成复习；学习 [pytest Fixtures and Test Doubles (测试夹具与替身)](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/01-pytest%20Fixtures%20and%20Test%20Doubles%20(测试夹具与替身).md)，再把 RAII 组件、Python 采集脚本和内存实验整理为可复现实验。

## 收尾：把三条线变成工程产物

- [ ] **10-19 周一 20:45–22:30 · C++**：学习 [Smart Pointers (智能指针)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C%2B%2B%20(现代%20C%2B%2B)/03-Smart%20Pointers%20(智能指针).md)，画所有权图并构造一次循环引用。
- [ ] **10-20 周二**：清理累计复习和错题，不新增笔记。
- [ ] **10-21 周三 21:00–22:30 · Python**：学习 [CLI Configuration and Subprocesses (命令行、配置与子进程)](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/09-CLI%20Configuration%20and%20Subprocesses%20(命令行、配置与子进程).md)，封装 C++ 构建和实验命令。
- [ ] **10-22 周四 09:00–12:00 · 工程整合**：学习 [CMake Project and Targets (CMake 项目与目标)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md) 与 [CMake Dependencies (CMake 依赖)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/02-CMake%20Dependencies%20(CMake%20依赖).md)，完成可复现构建、测试依赖、Python 驱动脚本和验收说明。

# 学完后的属性更新

排期状态：

```yaml
verified: 2026-09-23
review_stage: learn
review_due: 2026-09-23
```

真正学完后，将 `verified` 校正为实际完成日，`review_stage` 改为 `D1`，`review_due` 改为次日。之后按 `D1 → D3 → D7 → D14 → D30 → D60 → solid` 推进；答不出机制、边界或最小代码则退回 D1。达到 `solid` 后删除 `review_due`。

# 首月验收

- [ ] C++：能解释对象、生命周期、copy/move、RAII 与智能指针，并有可运行资源封装。
- [ ] Python：能建立环境、组织模块、处理数据和文件、编写 CLI，并驱动外部程序。
- [ ] 计算机组成：能从数据表示讲到编译执行、存储层级与 I/O，且有至少两个实验结果。
- [ ] 综合：CMake 构建 C++ 程序，Python 脚本负责执行、采集和报告，README 可让他人复现。
- [ ] 复习：没有通过单纯修改日期消除逾期；错误与薄弱点已记录。

# 调整规则

- 当天时段被课程或项目占用：只顺延该条线到下一个同类时段，不挤占另一条线。
- 周四或周六的重篇未完成：周日优先收尾，不新增内容。
- 到期项超过 5 篇：暂停新篇，优先当前项目、真实错误和最近学习内容。
- 周日先做到期复习，再学一篇短实践/质量笔记，最后完成三线串联和项目输出。
- 本页是唯一日程真相源，模块文件只保存脚本所需属性。

# 系统边界

- 长期能力顺序：[Backend Learning Roadmap (后端学习路线)](/00-Hub%20(学习入口)/01-Backend%20Learning%20Roadmap%20(后端学习路线).md)
- 复习规则：[Spaced Review System (间隔复习系统)](/00-Hub%20(学习入口)/02-Spaced%20Review%20System%20(间隔复习系统).md)
- 错误记录：[Mistake Log (错误记录)](/00-Hub%20(学习入口)/05-Mistake%20Log%20(错误记录).md)
