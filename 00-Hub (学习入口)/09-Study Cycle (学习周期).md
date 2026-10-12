---
study_stage: reference
tags: [learning/execution, learning/dashboard]
---

> [!abstract] 唯一任务面板
> 本页把课程、项目和自由学习时间统一为一套可执行节奏。知识模块只保存脚本需要的学习属性，不再各自维护周计划。

# 每日入口

```powershell
.\List-ReviewDue.ps1
.\List-ReviewDue.ps1 -All
```

Windows 端默认显示逾期、当天和明天，`-All` 查看完整首月队列；平板端直接使用下方“首月任务”，每个 Todo 都可进入对应笔记。开始新内容前，先处理当天到期复习。

# 首月一周节奏

| 星期 | 可用时段 | 主任务 | 负荷边界 |
| --- | --- | --- | --- |
| 周一 | 20:45–22:15 | 中等篇学习或上周遗留收口 | 一篇；当天已有算法练习，不叠加重实验 |
| 周二 | 课程密集至 21:25 | 到期复习或环境准备 | 原则上不开新篇；首日只安排轻量启动 |
| 周三 | 20:45–22:15 | Python/C++ 中等篇与最小示例 | 一篇，代码必须当晚运行 |
| 周四 | 08:30–11:30 | 重篇深读、调试与机制实验 | 一篇重文，或两篇强关联内容 |
| 周五 | 12:30–13:30 | 快速复习、短篇或 Python Lab 衔接 | 只放轻量内容，不追求大段推进 |
| 周六 | 13:30–16:30 | 计算机系统、C++ 重篇与综合实践 | 一篇重文加实验，保留观察证据 |
| 周日 | 08:30–11:30；13:30–15:30 | 主动回忆、补缺与周总结 | 新内容仅限短篇；优先消化本周欠账 |

三条线不是互相争抢时间：C++ 训练资源与工程模型，Python 提供自动化和验证能力，计算机组成与系统基础解释程序为何这样运行。以 2026-10-13 为重新排期的起点，首月保留 28 篇；周四和周六承接重任务，周五与周日只放短篇或复习。首月之后按实际进度滚动安排，不预填整个大学的死日期。

# 首月安排

**周期：2026-10-13 至 2026-11-12**

这是启动段，不是两年总计划。上表确定每周方向，下方 Todo 负责从平板直接进入当天笔记；`List-ReviewDue.ps1 -All` 仍从各笔记的 `review_due` 汇总同一排期。目录缩写沿用路线图的“一级/二级序号”。

| 时段 | 文件夹焦点 | 本段留下的东西 |
| --- | --- | --- |
| 10/13–10/18 | 04/01 Python 启动；03/01–03/02 C++ 基础；01/01 系统总览 | 建好 Python 环境；完成指针/头文件小程序；画出程序运行链路 |
| 10/19–10/25 | 03/02 对象与资源模型；04/01 函数；01/01 数据表示；04/04 自动化实践 | 比较 C++ 生命周期与 Python 对象；保留一个自动化脚本和数据表示实验 |
| 10/26–11/01 | 03/03 现代 C++；04/01 Python 协议与对象模型；01/01 程序构建；04/02 测试 | 记录编译链接过程；用 pytest 验证一个 Python 模块 |
| 11/02–11/08 | 03/02 模板与 RAII；04/01 文件/异常；01/01 存储层级与 I/O | 完成资源封装，对比异常退出路径，并保存 I/O 或缓存观察结果 |
| 11/09–11/12 | 03/03 智能指针；04/01 CLI；02/02 CMake | 交付 CMake 构建的 C++ 程序、Python 驱动脚本、测试命令和复现说明 |

# 首月任务

**第 1 周 · 10/13–10/18**

- [ ] 10/13 周二 · [运行环境、模块与包管理](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/01-Runtime%20Modules%20Packages%20and%20uv%20(运行环境、模块与包管理).md)：只完成环境、运行入口和最小模块，控制在 45 分钟内。
- [ ] 10/14 周三 · [核心语法、容器与推导式](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/03-Core%20Syntax%20Collections%20and%20Comprehensions%20(核心语法、容器与推导式).md)：写出可运行示例。
- [ ] 10/15 周四 · [类型、指针与引用](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/01-Types%20Pointers%20and%20References%20(类型指针与引用).md) + [头文件、命名空间与分离编译](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/03-Headers%20Namespaces%20and%20Separate%20Compilation%20(头文件命名空间与分离编译).md)：共用一个多文件实验。
- [ ] 10/16 周五 · [对象、名称、可变性与复制](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/02-Objects%20Names%20Mutability%20and%20Copying%20(对象、名称、可变性与复制).md)：用短实验验证引用与复制。
- [ ] 10/17 周六 · [计算机系统总览](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/01-Computer%20System%20Overview%20(计算机系统总览).md)：画出程序从源代码到运行的链路。
- [ ] 10/18 周日 · [Python 项目实践流程](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/01-Python%20Project%20Practice%20Workflow%20(Python%20项目实践流程).md)：用本周代码走一遍流程并完成周总结。

**第 2 周 · 10/19–10/25**

- [ ] 10/19 周一 · [内存布局与分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)：配合调试器观察地址与生命周期。
- [ ] 10/21 周三 · [函数、作用域、闭包与装饰器](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/04-Functions%20Scope%20Closures%20and%20Decorators%20(函数、作用域、闭包与装饰器).md)：完成最小闭包和装饰器示例。
- [ ] 10/22 周四 · [对象生命周期与拷贝控制](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)：重篇，只做这一篇并验证特殊成员函数调用。
- [ ] 10/23 周五 · [自动化项目模板](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/02-Automation%20Project%20Template%20(自动化项目模板).md)：整理为可重复运行的目录。
- [ ] 10/24 周六 · [数据表示](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Data%20Representation%20(数据表示).md)：完成溢出、补码或浮点实验。
- [ ] 10/25 周日 · [类型标注、日志与静态检查](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/02-Type%20Hints%20Logging%20and%20Linting%20(类型日志与静态检查).md)：把工具接入本周 Python 代码。

**第 3 周 · 10/26–11/01**

- [ ] 10/26 周一 · [现代 C++ 基础](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Modern%20C++%20Foundations%20(现代%20C++%20基础).md)：整理一组能直接用于项目的语言特性。
- [ ] 10/28 周三 · [迭代、生成器与协议](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/05-Iteration%20Generators%20and%20Protocols%20(迭代、生成器与协议).md)：实现一个惰性数据流水线。
- [ ] 10/29 周四 · [值类别与移动语义](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)：用构造日志观察复制与移动。
- [ ] 10/30 周五 · [类、数据类与对象模型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/06-Classes%20Dataclasses%20and%20Object%20Model%20(类、数据类与对象模型).md)：把前一周脚本整理成对象模型。
- [ ] 10/31 周六 · [程序构建与执行](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)：保存预处理、编译、链接和装载证据。
- [ ] 11/01 周日 · [pytest 夹具与测试替身](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/01-pytest%20Fixtures%20and%20Test%20Doubles%20(测试夹具与替身).md)：只给已有模块补测试，不另开新项目。

**第 4 周 · 11/02–11/08**

- [ ] 11/02 周一 · [模板](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)：完成一个泛型容器或算法小例子。
- [ ] 11/04 周三 · [路径、文件、序列化与时间](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/07-Paths%20Files%20Serialization%20and%20Time%20(路径、文件、序列化与时间).md)：读取并汇总 C++ 程序输出。
- [ ] 11/05 周四 · [RAII 与自定义分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)：实现资源封装并测试异常路径。
- [ ] 11/06 周五 · [异常、上下文管理与类型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/08-Exceptions%20Context%20Managers%20and%20Typing%20(异常、上下文与类型).md)：与 RAII 做机制对照。
- [ ] 11/07 周六 · [存储层级与输入输出](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)：完成一次缓存或 I/O 观察实验。
- [ ] 11/08 周日 · 本周只做主动回忆、补缺和综合实践，不新增笔记。

**收口段 · 11/09–11/12**

- [ ] 11/09 周一 · [智能指针](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Smart%20Pointers%20(智能指针).md)：把资源封装迁移为明确的所有权模型。
- [ ] 11/11 周三 · [命令行、配置与子进程](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/09-CLI%20Configuration%20and%20Subprocesses%20(命令行、配置与子进程).md)：完成驱动 C++ 程序的 CLI。
- [ ] 11/12 周四 · [CMake 项目与目标](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md) + [CMake 依赖](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/02-CMake%20Dependencies%20(CMake%20依赖).md)：完成首月综合交付与复现说明。

首月的优先级是让 C++、Python、计算机核心三条线在**同一份可运行产物**里相遇。若某篇重文未完成，不为赶表格而跳过实验；把该主题顺延到下一个同类学习时段，并同步修改对应笔记的 `review_due`。周日以补缺和整合作为第一任务，不强制新增笔记。

# 长期推进

首月之后按 6～8 周推进块滚动。每轮只设一个**主文件夹群**、一个**支撑文件夹群**和一个可演示成果；具体读到哪篇，根据前一轮的错误与项目瓶颈再排进 `review_due`。按照 [后端学习路线](/00-Hub%20(学习入口)/01-Backend%20Learning%20Roadmap%20(后端学习路线).md) 依次推进：

1. **C++ 与系统块**：03/03–03/06 配合 06/01–06/03、02/03；做网络或并发组件，拿到 sanitizer、日志与压测证据。
2. **Python、Go 与数据块**：先补 04/02–04/04，再走 05/01–05/05 与 07/01–07/02；做带 MySQL、缓存、测试与优雅关闭的服务。
3. **分布式、云与安全块**：07/03–07/04 → 08/01、08/03 → 09/01–09/03；在 11/02 中深做一个多组件项目，演练故障、权限和恢复。08/02 平台工程仅在真正需要平台化时深入。
4. **AI 与求职块**：10/01 → 10/02，容量需求出现后再进 10/03；用 11/02 验证评估、权限与成本。12/04 决策记录全程积累，12/01–12/03 在有项目证据后集中训练。

每轮前半段建立模型与最小实现，中段补反例、测试和故障注入，后段整理 README、原始指标与取舍记录。**结束条件**不是“读完文件夹”，而是能演示产物、解释两个失败路径，并能指出下一轮最重要的知识缺口。若未达到，继续当前块而不是机械切换主题。

首月后的时段仍沿用上表的可用时间，但主任务替换为当前推进块；例如进入 Go 与数据块后，周一/周四的 C++ 时段可用于 Go 服务或数据库实验，C++ 改为每周一次维护性实践。不要把“首月星期表”当成未来两年的固定科目表。

# 学完后的属性更新

排期状态：

```yaml
study_stage: learn
review_due: 2026-10-13
```

真正学完后才写 `studied_on`（实际完成日），把 `study_stage` 改为 `D1`，`review_due` 改为次日。之后按 `D1 → D3 → D7 → D14 → D30 → D60 → solid` 推进；答不出机制、边界或最小代码则退回 D1。达到 `solid` 后删除 `review_due`。

# 首月验收

- [ ] **C++**：串联[对象生命周期与拷贝控制](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)、[值类别与移动语义](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)、[RAII 与自定义分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)和[智能指针](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Smart%20Pointers%20(智能指针).md)，留下可运行的资源封装。
- [ ] **Python**：完成[环境、模块与包管理](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/01-Runtime%20Modules%20Packages%20and%20uv%20(运行环境、模块与包管理).md)、[核心语法与容器](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/03-Core%20Syntax%20Collections%20and%20Comprehensions%20(核心语法、容器与推导式).md)、[文件处理](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/07-Paths%20Files%20Serialization%20and%20Time%20(路径、文件、序列化与时间).md)、[异常与上下文管理](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/08-Exceptions%20Context%20Managers%20and%20Typing%20(异常、上下文与类型).md)和[命令行与子进程](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/09-CLI%20Configuration%20and%20Subprocesses%20(命令行、配置与子进程).md)，能用脚本驱动外部程序。
- [ ] **计算机组成**：依次完成[数据表示](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Data%20Representation%20(数据表示).md)、[程序构建与执行](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)和[存储层级与输入输出](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)，保留至少两个实验结果。
- [ ] **综合实践**：用[CMake 项目与目标](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md)和[CMake 依赖](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/02-CMake%20Dependencies%20(CMake%20依赖).md)构建 C++ 程序，Python 脚本负责执行、采集和报告，README 可让他人复现。
- [ ] **复习收口**：按[间隔复习系统](/00-Hub%20(学习入口)/02-Spaced%20Review%20System%20(间隔复习系统).md)处理到期项，并把薄弱点写入[错误记录](/00-Hub%20(学习入口)/05-Mistake%20Log%20(错误记录).md)，不靠单纯修改日期清除逾期。

# 调整规则

- 当天时段被课程或项目占用：只顺延该条线到下一个同类时段，不挤占另一条线。
- 周二原则上不开新篇；2026-10-13 仅用轻量环境笔记启动本轮。
- 周四或周六的重篇未完成：周日优先收尾，不新增内容。
- 到期项超过 5 篇：暂停新篇，优先当前项目、真实错误和最近学习内容。
- 周日先做到期复习和补缺，有余力再学短实践笔记；必须留下本周项目或实验增量。
- 本页只保存时段、阶段和调整规则；具体到期日以笔记属性为准，由脚本统一展示。

# 系统边界

- 长期能力顺序：[Backend Learning Roadmap (后端学习路线)](/00-Hub%20(学习入口)/01-Backend%20Learning%20Roadmap%20(后端学习路线).md)
- 复习规则：[Spaced Review System (间隔复习系统)](/00-Hub%20(学习入口)/02-Spaced%20Review%20System%20(间隔复习系统).md)
- 错误记录：[Mistake Log (错误记录)](/00-Hub%20(学习入口)/05-Mistake%20Log%20(错误记录).md)
