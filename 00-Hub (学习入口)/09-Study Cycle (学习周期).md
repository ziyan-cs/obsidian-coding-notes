---
study_stage: reference
tags: [learning/execution, learning/dashboard]
---

> [!abstract] 学习路线
> 按知识依赖从基础走向工程与后端系统。前两个月逐日安排；之后按概念知识块顺序推进。C++、Python 与计算机基础交错学习，笔记是主线，刷题为辅。

# 使用方式

电脑查看到期复习；平板按本页顺序学习。勾选表示能复述机制、完成验证并说明一个适用边界。

```powershell
.\List-ReviewDue.ps1
.\List-ReviewDue.ps1 -All
```

复习日期由笔记属性和脚本管理；新学任务按本页顺序执行。遇到课程、项目冲突时顺延未学任务，不跳过前置。

# 两年路线

基础能力 → 工程实践与 C++ 深化 → 系统与网络 → 数据系统与 Go → 后端项目 → 云原生与安全 → AI 系统 → 系统设计与求职。阶段切换以能解释、能验证、能交付为准，不以日期赶进度。

# 阶段一 · 基础启动

日期表示首次学习安排；复习日用于主动回忆。刷题不占 Notes 主计划。

**程序环境、语言表达与运行模型**

- [x] 10/13 周二 · [运行环境、模块与包管理](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/01-Runtime%20Modules%20Packages%20and%20uv%20(运行环境、模块与包管理).md)：只完成环境、运行入口和最小模块，控制在 45 分钟内。
- [x] 10/14 周三 · [核心语法、容器与推导式](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/03-Core%20Syntax%20Collections%20and%20Comprehensions%20(核心语法、容器与推导式).md)：写出可运行示例。
- [x] 10/15 周四 · [类型、指针与引用](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/01-Types%20Pointers%20and%20References%20(类型指针与引用).md) + [头文件、命名空间与分离编译](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/03-Headers%20Namespaces%20and%20Separate%20Compilation%20(头文件命名空间与分离编译).md)：共用一个多文件实验。
- [x] 10/16 周五 · [对象、名称、可变性与复制](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/02-Objects%20Names%20Mutability%20and%20Copying%20(对象、名称、可变性与复制).md)：用短实验验证引用与复制。
- [ ] 10/17 周六 · [计算机系统总览](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/01-Computer%20System%20Overview%20(计算机系统总览).md)：画出程序从源代码到运行的链路。
- [ ] 10/18 周日 · [Python 项目实践流程](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/01-Python%20Project%20Practice%20Workflow%20(Python%20项目实践流程).md)：用本周代码走一遍流程并完成周总结。

- [ ] 10/19 周一 · [Python 项目实践流程](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/01-Python%20Project%20Practice%20Workflow%20(Python%20项目实践流程).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 10/20 周二 · [Python 项目实践流程](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/01-Python%20Project%20Practice%20Workflow%20(Python%20项目实践流程).md)：轻量回忆：只复述一条知识链，不开新篇。

**函数、内存、对象生命周期与构建**

- [ ] 10/21 周三 · [函数、作用域、闭包与装饰器](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/04-Functions%20Scope%20Closures%20and%20Decorators%20(函数、作用域、闭包与装饰器).md)：完成最小闭包和装饰器示例。
- [ ] 10/22 周四 · [核心语法与函数](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/01-Core%20Syntax%20and%20Functions%20(核心语法与函数).md)：把值传递、引用传递、作用域与函数接口写进一个可编译的小程序。
- [ ] 10/22 周四 · [内存布局与分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)：配合调试器观察地址与生命周期。
- [ ] 10/23 周五 · [函数、作用域、闭包与装饰器](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/04-Functions%20Scope%20Closures%20and%20Decorators%20(函数、作用域、闭包与装饰器).md)、[核心语法与函数](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/01-Core%20Syntax%20and%20Functions%20(核心语法与函数).md)：Quick Review：回想本周新学内容，写下一个没讲清的问题。
- [ ] 10/24 周六 · [数据表示](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Data%20Representation%20(数据表示).md)：完成溢出、补码或浮点实验。
- [ ] 10/25 周日 · [函数、作用域、闭包与装饰器](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/04-Functions%20Scope%20Closures%20and%20Decorators%20(函数、作用域、闭包与装饰器).md)、[核心语法与函数](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/01-Core%20Syntax%20and%20Functions%20(核心语法与函数).md)、[内存布局与分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)：Active Recall 与周总结：用一页纸串起 C++、Python、计算机基础。

- [ ] 10/26 周一 · [数据表示](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Data%20Representation%20(数据表示).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 10/27 周二 · [数据表示](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Data%20Representation%20(数据表示).md)：轻量回忆：只复述一条知识链，不开新篇。
- [ ] 10/28 周三 · [迭代、生成器与协议](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/05-Iteration%20Generators%20and%20Protocols%20(迭代、生成器与协议).md)：实现一个惰性数据流水线。
- [ ] 10/29 周四 · [对象生命周期与拷贝控制](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)：重篇，只做这一篇并验证特殊成员函数调用。
- [ ] 10/30 周五 · [迭代、生成器与协议](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/05-Iteration%20Generators%20and%20Protocols%20(迭代、生成器与协议).md)、[对象生命周期与拷贝控制](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)：Quick Review：回想本周新学内容，写下一个没讲清的问题。
- [ ] 10/31 周六 · [程序构建与执行](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)：保存预处理、编译、链接和装载证据。

**对象模型、数据表示与资源管理**

- [ ] 11/01 周日 · [迭代、生成器与协议](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/05-Iteration%20Generators%20and%20Protocols%20(迭代、生成器与协议).md)、[对象生命周期与拷贝控制](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)、[程序构建与执行](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)：Active Recall 与周总结：用一页纸串起 C++、Python、计算机基础。

- [ ] 11/02 周一 · [程序构建与执行](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 11/03 周二 · [程序构建与执行](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)：轻量回忆：只复述一条知识链，不开新篇。
- [ ] 11/04 周三 · [类、数据类与对象模型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/06-Classes%20Dataclasses%20and%20Object%20Model%20(类、数据类与对象模型).md)：把前一周脚本整理成对象模型。
- [ ] 11/05 周四 · [现代 C++ 基础](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Modern%20C++%20Foundations%20(现代%20C++%20基础).md)：整理一组能直接用于项目的语言特性。
- [ ] 11/06 周五 · [类、数据类与对象模型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/06-Classes%20Dataclasses%20and%20Object%20Model%20(类、数据类与对象模型).md)、[现代 C++ 基础](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Modern%20C++%20Foundations%20(现代%20C++%20基础).md)：Quick Review：回想本周新学内容，写下一个没讲清的问题。
- [ ] 11/07 周六 · [存储层级与输入输出](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)：完成一次缓存或 I/O 观察实验。
- [ ] 11/08 周日 · [类、数据类与对象模型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/06-Classes%20Dataclasses%20and%20Object%20Model%20(类、数据类与对象模型).md)、[现代 C++ 基础](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Modern%20C++%20Foundations%20(现代%20C++%20基础).md)、[存储层级与输入输出](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)：Active Recall 与周总结：用一页纸串起 C++、Python、计算机基础。

- [ ] 11/09 周一 · [存储层级与输入输出](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 11/10 周二 · [存储层级与输入输出](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)：轻量回忆：只复述一条知识链，不开新篇。
- [ ] 11/11 周三 · [异常、上下文管理与类型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/08-Exceptions%20Context%20Managers%20and%20Typing%20(异常、上下文与类型).md)：与 RAII 做机制对照。
- [ ] 11/12 周四 · [值类别与移动语义](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)：用构造日志观察复制与移动。
- [ ] 11/13 周五 · [异常、上下文管理与类型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/08-Exceptions%20Context%20Managers%20and%20Typing%20(异常、上下文与类型).md)、[值类别与移动语义](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)：Quick Review：回想本周新学内容，写下一个没讲清的问题。
- [ ] 11/14 周六 · [操作系统总览与启动](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/05-Operating%20System%20Overview%20and%20Boot%20(操作系统总览与启动).md)：从上电到用户程序运行，画出固件、引导程序、内核和进程的关系。
- [ ] 11/15 周日 · [异常、上下文管理与类型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/08-Exceptions%20Context%20Managers%20and%20Typing%20(异常、上下文与类型).md)、[值类别与移动语义](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)、[操作系统总览与启动](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/05-Operating%20System%20Overview%20and%20Boot%20(操作系统总览与启动).md)：Active Recall 与周总结：用一页纸串起 C++、Python、计算机基础。

**文件处理、并发模型与工程边界**

- [ ] 11/16 周一 · [操作系统总览与启动](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/05-Operating%20System%20Overview%20and%20Boot%20(操作系统总览与启动).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 11/17 周二 · [操作系统总览与启动](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/05-Operating%20System%20Overview%20and%20Boot%20(操作系统总览与启动).md)：轻量回忆：只复述一条知识链，不开新篇。
- [ ] 11/18 周三 · [路径、文件、序列化与时间](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/07-Paths%20Files%20Serialization%20and%20Time%20(路径、文件、序列化与时间).md)：读取并汇总 C++ 程序输出。
- [ ] 11/19 周四 · [RAII 与自定义分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)：实现资源封装并测试异常路径。
- [ ] 11/20 周五 · [路径、文件、序列化与时间](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/07-Paths%20Files%20Serialization%20and%20Time%20(路径、文件、序列化与时间).md)、[RAII 与自定义分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)：Quick Review：回想本周新学内容，写下一个没讲清的问题。
- [ ] 11/21 周六 · [进程与线程](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/06-Processes%20and%20Threads%20(进程与线程).md)：区分地址空间、执行流和共享资源，画出进程创建到线程并发的模型。
- [ ] 11/22 周日 · [路径、文件、序列化与时间](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/07-Paths%20Files%20Serialization%20and%20Time%20(路径、文件、序列化与时间).md)、[RAII 与自定义分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)、[进程与线程](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/06-Processes%20and%20Threads%20(进程与线程).md)：Active Recall 与周总结：用一页纸串起 C++、Python、计算机基础。

- [ ] 11/23 周一 · [进程与线程](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/06-Processes%20and%20Threads%20(进程与线程).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 11/24 周二 · [进程与线程](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/06-Processes%20and%20Threads%20(进程与线程).md)：轻量回忆：只复述一条知识链，不开新篇。
- [ ] 11/25 周三 · [命令行、配置与子进程](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/09-CLI%20Configuration%20and%20Subprocesses%20(命令行、配置与子进程).md)：完成驱动 C++ 程序的 CLI。
- [ ] 11/26 周四 · [数组、字符串与输入输出](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/02-Arrays%20Strings%20and%20IO%20(数组字符串与输入输出).md)：完成一个读取输入、处理字符串并输出结果的程序，明确数组和 `std::string` 的边界行为。
- [ ] 11/27 周五 · [命令行、配置与子进程](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/09-CLI%20Configuration%20and%20Subprocesses%20(命令行、配置与子进程).md)、[数组、字符串与输入输出](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/02-Arrays%20Strings%20and%20IO%20(数组字符串与输入输出).md)：Quick Review：回想本周新学内容，写下一个没讲清的问题。
- [ ] 11/28 周六 · [上下文切换与调度](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/07-Context%20Switching%20and%20Scheduling%20(上下文切换与调度).md)：把调度目标、时间片和上下文切换成本联系起来，分析一个 I/O 密集与 CPU 密集混合场景。
- [ ] 11/29 周日 · [命令行、配置与子进程](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/09-CLI%20Configuration%20and%20Subprocesses%20(命令行、配置与子进程).md)、[数组、字符串与输入输出](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/02-Arrays%20Strings%20and%20IO%20(数组字符串与输入输出).md)、[上下文切换与调度](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/07-Context%20Switching%20and%20Scheduling%20(上下文切换与调度).md)：Active Recall 与周总结：用一页纸串起 C++、Python、计算机基础。

**测试验证、泛型编程与交付**

- [ ] 11/30 周一 · [上下文切换与调度](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/07-Context%20Switching%20and%20Scheduling%20(上下文切换与调度).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 12/01 周二 · [上下文切换与调度](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/07-Context%20Switching%20and%20Scheduling%20(上下文切换与调度).md)：轻量回忆：只复述一条知识链，不开新篇。
- [ ] 12/02 周三 · [pytest 夹具与测试替身](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/01-pytest%20Fixtures%20and%20Test%20Doubles%20(测试夹具与替身).md)：只给已有模块补测试，不另开新项目。
- [ ] 12/03 周四 · [模板](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)：完成一个泛型容器或算法小例子。
- [ ] 12/04 周五 · [pytest 夹具与测试替身](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/01-pytest%20Fixtures%20and%20Test%20Doubles%20(测试夹具与替身).md)、[模板](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)：Quick Review：回想本周新学内容，写下一个没讲清的问题。
- [ ] 12/05 周六 · [虚拟内存、分页与分配](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)：从虚拟地址到物理页框画出转换链路，解释缺页异常与页面置换的区别。
- [ ] 12/06 周日 · [pytest 夹具与测试替身](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/01-pytest%20Fixtures%20and%20Test%20Doubles%20(测试夹具与替身).md)、[模板](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)、[虚拟内存、分页与分配](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)：Active Recall 与周总结：用一页纸串起 C++、Python、计算机基础。

- [ ] 12/07 周一 · [虚拟内存、分页与分配](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)：到期复习：从复习脚本清单中挑一篇，合上笔记复述机制与边界。
- [ ] 12/08 周二 · [虚拟内存、分页与分配](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)：轻量回忆：只复述一条知识链，不开新篇。
- [ ] 12/09 周三 · [类型标注、日志与静态检查](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/02-Type%20Hints%20Logging%20and%20Linting%20(类型日志与静态检查).md)：把工具接入本周 Python 代码。
- [ ] 12/10 周四 · [CMake 项目与目标](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md) + [CMake 依赖](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/02-CMake%20Dependencies%20(CMake%20依赖).md)：完成阶段综合交付与复现说明。
- [ ] 12/11 周五 · [模板](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)、[类型标注、日志与静态检查](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/02-Type%20Hints%20Logging%20and%20Linting%20(类型日志与静态检查).md)、[虚拟内存、分页与分配](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)：两个月复盘：分别口述三条主线，记录各自最薄弱的一点并据此排下一阶段。
- [ ] 12/12 周六 · [智能指针](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Smart%20Pointers%20(智能指针).md)：把资源封装迁移为明确的所有权模型。

# 阶段二 · 夯实基础、工程工具与主语言

按下列顺序继续；每完成一组知识块，再用其 `00-... Standard` 自测。未进入近期计划的笔记保持 `backlog`，属性规则见 [Note Standard (笔记规范)](/00-Hub%20(学习入口)/03-Note%20Standard%20(笔记规范).md)。

**数组与链表、栈队列与哈希、二分查找**
- [ ] [01-Arrays and Linked Lists (数组与链表)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Arrays%20and%20Linked%20Lists%20(数组与链表).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Stack Queue and Hashing (栈队列与哈希)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Stack%20Queue%20and%20Hashing%20(栈队列与哈希).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [07-Binary Search (二分查找)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/07-Binary%20Search%20(二分查找).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**堆与 Top K、排序算法、树遍历与路径**
- [ ] [03-Heap and Top K (堆与 Top K)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Heap%20and%20Top%20K%20(堆与%20Top%20K).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [08-Sorting Algorithms (排序算法)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/08-Sorting%20Algorithms%20(排序算法).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Tree Traversal and Paths (树遍历与路径)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/04-Tree%20Traversal%20and%20Paths%20(树遍历与路径).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**顺序容器、多态与继承、自动化项目模板**
- [ ] [01-Sequence Containers (顺序容器)](/03-C++%20Backend%20(C++%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/01-Sequence%20Containers%20(顺序容器).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Polymorphism and Inheritance (多态与继承)](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/04-Polymorphism%20and%20Inheritance%20(多态与继承).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Automation Project Template (自动化项目模板)](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/02-Automation%20Project%20Template%20(自动化项目模板).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**测试打包与交付、项目质量基线、数据处理边界**
- [ ] [03-Testing Packaging and Delivery (测试打包与交付)](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/03-Testing%20Packaging%20and%20Delivery%20(测试打包与交付).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Project Quality Baseline (项目质量基线)](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/03-Project%20Quality%20Baseline%20(项目质量基线).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Data Processing Boundaries (数据处理边界)](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/02-Data%20Processing%20Boundaries%20(数据处理边界).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**文件系统与存储、同步原语、死锁分析与恢复**
- [ ] [09-File Systems and Storage (文件系统与存储)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/09-File%20Systems%20and%20Storage%20(文件系统与存储).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [10-Synchronization Primitives (同步原语)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/10-Synchronization%20Primitives%20(同步原语).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [11-Deadlock Analysis and Recovery (死锁分析与恢复)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/11-Deadlock%20Analysis%20and%20Recovery%20(死锁分析与恢复).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**性能与系统思维、离散逻辑、集合与图、二叉搜索树**
- [ ] [12-Performance and System Thinking (性能与系统思维)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/12-Performance%20and%20System%20Thinking%20(性能与系统思维).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Discrete Logic Sets and Graphs (离散逻辑、集合与图)](/01-Foundations%20(基础能力)/03-Mathematics%20and%20Reasoning%20(数学与推理)/01-Discrete%20Logic%20Sets%20and%20Graphs%20(离散逻辑、集合与图).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Binary Search Trees (二叉搜索树)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/05-Binary%20Search%20Trees%20(二叉搜索树).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**图并查集与最短路、回溯与状态空间搜索、一维动态规划**
- [ ] [06-Graphs Union Find and Shortest Paths (图并查集与最短路)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/06-Graphs%20Union%20Find%20and%20Shortest%20Paths%20(图并查集与最短路).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [09-Backtracking and State Space Search (回溯与状态空间搜索)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/09-Backtracking%20and%20State%20Space%20Search%20(回溯与状态空间搜索).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [10-One Dimensional Dynamic Programming (一维动态规划)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/10-One%20Dimensional%20Dynamic%20Programming%20(一维动态规划).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**背包动态规划、区间动态规划、序列动态规划**
- [ ] [11-Knapsack Dynamic Programming (背包动态规划)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/11-Knapsack%20Dynamic%20Programming%20(背包动态规划).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [12-Interval Dynamic Programming (区间动态规划)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/12-Interval%20Dynamic%20Programming%20(区间动态规划).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [13-Sequence Dynamic Programming (序列动态规划)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/13-Sequence%20Dynamic%20Programming%20(序列动态规划).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**贪心与滑动窗口、字符串与位运算、概率、统计与排队**
- [ ] [14-Greedy and Sliding Window (贪心与滑动窗口)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/14-Greedy%20and%20Sliding%20Window%20(贪心与滑动窗口).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [15-String and Bit Algorithms (字符串与位运算)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/15-String%20and%20Bit%20Algorithms%20(字符串与位运算).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Probability Statistics and Queueing (概率、统计与排队)](/01-Foundations%20(基础能力)/03-Mathematics%20and%20Reasoning%20(数学与推理)/02-Probability%20Statistics%20and%20Queueing%20(概率、统计与排队).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**线性代数与数值直觉、问题求解与调试、测试与验证**
- [ ] [03-Linear Algebra and Numerical Intuition (线性代数与数值直觉)](/01-Foundations%20(基础能力)/03-Mathematics%20and%20Reasoning%20(数学与推理)/03-Linear%20Algebra%20and%20Numerical%20Intuition%20(线性代数与数值直觉).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Problem Solving and Debugging (问题求解与调试)](/01-Foundations%20(基础能力)/04-Foundation%20Practice%20(基础实践)/01-Problem%20Solving%20and%20Debugging%20(问题求解与调试).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Testing and Verification (测试与验证)](/01-Foundations%20(基础能力)/04-Foundation%20Practice%20(基础实践)/02-Testing%20and%20Verification%20(测试与验证).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**刻意练习与知识维护、基础能力标准与自测**
- [ ] [03-Deliberate Practice and Knowledge Maintenance (刻意练习与知识维护)](/01-Foundations%20(基础能力)/04-Foundation%20Practice%20(基础实践)/03-Deliberate%20Practice%20and%20Knowledge%20Maintenance%20(刻意练习与知识维护).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Foundations Standard (基础能力标准与自测)](/01-Foundations%20(基础能力)/00-Foundations%20Standard%20(基础能力标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**构建、交付与质量控制**

**Git 模型与分支、冲突与恢复、持续集成与交付**
- [ ] [01-Git Model and Branches (Git 模型与分支)](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/01-Git%20Model%20and%20Branches%20(Git%20模型与分支).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Conflicts and Recovery (冲突与恢复)](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/02-Conflicts%20and%20Recovery%20(冲突与恢复).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-CI CD (持续集成与交付)](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/03-CI%20CD%20(持续集成与交付).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**包管理与可复现构建、Make 与 Shell 自动化、测试策略与证据**
- [ ] [03-Package Management and Reproducible Builds (包管理与可复现构建)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/03-Package%20Management%20and%20Reproducible%20Builds%20(包管理与可复现构建).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Make and Shell Automation (Make 与 Shell 自动化)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/04-Make%20and%20Shell%20Automation%20(Make%20与%20Shell%20自动化).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**调试与故障定位、内存安全与动态分析、性能基准与剖析**
- [ ] [02-Debugging and Failure Localization (调试与故障定位)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/02-Debugging%20and%20Failure%20Localization%20(调试与故障定位).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Memory Safety and Dynamic Analysis (内存安全与动态分析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/03-Memory%20Safety%20and%20Dynamic%20Analysis%20(内存安全与动态分析).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Performance Benchmarking and Profiling (性能基准与剖析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/04-Performance%20Benchmarking%20and%20Profiling%20(性能基准与剖析).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**可观测性、日志、指标与追踪、安全检测与事件证据、发布验证与质量门禁**
- [ ] [05-Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [06-Security Detection and Incident Evidence (安全检测与事件证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/06-Security%20Detection%20and%20Incident%20Evidence%20(安全检测与事件证据).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [07-Release Verification and Quality Gates (发布验证与质量门禁)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/07-Release%20Verification%20and%20Quality%20Gates%20(发布验证与质量门禁).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**工程基础标准与自测**
- [ ] [00-Engineering Fundamentals Standard (工程基础标准与自测)](/02-Engineering%20Fundamentals%20(工程基础)/00-Engineering%20Fundamentals%20Standard%20(工程基础标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**C++ 后端**

**关联容器、迭代器算法与适配器、完美转发与万能引用**
- [ ] [02-Associative Containers (关联容器)](/03-C++%20Backend%20(C++%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/02-Associative%20Containers%20(关联容器).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Iterators Algorithms and Adapters (迭代器算法与适配器)](/03-C++%20Backend%20(C++%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/03-Iterators%20Algorithms%20and%20Adapters%20(迭代器算法与适配器).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Perfect Forwarding and Universal References (完美转发与万能引用)](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/04-Perfect%20Forwarding%20and%20Universal%20References%20(完美转发与万能引用).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**词汇类型与视图、编译期编程、Concepts 与 SFINAE**
- [ ] [06-Vocabulary Types and Views (词汇类型与视图)](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/06-Vocabulary%20Types%20and%20Views%20(词汇类型与视图).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Compile Time Programming (编译期编程)](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/05-Compile%20Time%20Programming%20(编译期编程).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [07-Concepts and SFINAE (Concepts 与 SFINAE)](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/07-Concepts%20and%20SFINAE%20(Concepts%20与%20SFINAE).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**线程锁与协作、原子与内存序、线程池**
- [ ] [01-Threads Locks and Coordination (线程锁与协作)](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/01-Threads%20Locks%20and%20Coordination%20(线程锁与协作).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Atomics and Memory Order (原子与内存序)](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/02-Atomics%20and%20Memory%20Order%20(原子与内存序).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Thread Pool (线程池)](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/03-Thread%20Pool%20(线程池).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**无锁与性能、协程、构建组织与预处理**
- [ ] [04-Lock Free and Performance (无锁与性能)](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/04-Lock%20Free%20and%20Performance%20(无锁与性能).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [08-Coroutines (协程)](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/08-Coroutines%20(协程).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Build Organization and Preprocessing (构建组织与预处理)](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/01-Build%20Organization%20and%20Preprocessing%20(构建组织与预处理).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**CMake 与依赖、异常与调试、测试与可观测性**
- [ ] [02-CMake and Dependencies (CMake 与依赖)](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/02-CMake%20and%20Dependencies%20(CMake%20与依赖).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Exceptions and Debugging (异常与调试)](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/03-Exceptions%20and%20Debugging%20(异常与调试).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Testing and Observability (测试与可观测性)](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/04-Testing%20and%20Observability%20(测试与可观测性).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**性能分析与优化、C++ 后端标准与自测**
- [ ] [05-Profiling and Optimization (性能分析与优化)](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/05-Profiling%20and%20Optimization%20(性能分析与优化).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-C++ Backend Standard (C++ 后端标准与自测)](/03-C++%20Backend%20(C++%20后端)/00-C++%20Backend%20Standard%20(C++%20后端标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**Python 工程**

**性质、模糊与集成测试、HTTP自动化与接口检查、异步 I-O 与并发**
- [ ] [04-Property Fuzz and Integration Testing (性质、模糊与集成测试)](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/04-Property%20Fuzz%20and%20Integration%20Testing%20(性质、模糊与集成测试).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-HTTP Automation and API Checks (HTTP自动化与接口检查)](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/01-HTTP%20Automation%20and%20API%20Checks%20(HTTP自动化与接口检查).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Async IO and Concurrency (异步 I-O 与并发)](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/04-Async%20IO%20and%20Concurrency%20(异步%20I-O%20与并发).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

# 阶段三 · 建立项目方法

**工程决策复盘**

**资源生命周期、并发模型、错误处理**
- [ ] [01-Resource Lifetime (资源生命周期)](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/01-Resource%20Lifetime%20(资源生命周期).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Concurrency Models (并发模型)](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/02-Concurrency%20Models%20(并发模型).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Error Handling (错误处理)](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/03-Error%20Handling%20(错误处理).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**HTTP 服务设计、可观测性与性能**
- [ ] [04-HTTP Service Design (HTTP 服务设计)](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/04-HTTP%20Service%20Design%20(HTTP%20服务设计).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Observability and Performance (可观测性与性能)](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/05-Observability%20and%20Performance%20(可观测性与性能).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**项目方法**

**项目设计模板、测试压测与证据、需求架构与里程碑**
- [ ] [01-Project Design Template (项目设计模板)](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/01-Project%20Design%20Template%20(项目设计模板).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Testing Benchmark and Evidence (测试压测与证据)](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/02-Testing%20Benchmark%20and%20Evidence%20(测试压测与证据).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Requirements Architecture and Milestones (需求架构与里程碑)](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/03-Requirements%20Architecture%20and%20Milestones%20(需求架构与里程碑).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

# 阶段四 · 打通系统与网络

**Shell 命令与管道、文件权限与环境、系统管理**
- [ ] [01-Shell Commands and Pipelines (Shell 命令与管道)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/01-Shell%20Commands%20and%20Pipelines%20(Shell%20命令与管道).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Files Permissions and Environment (文件权限与环境)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/02-Files%20Permissions%20and%20Environment%20(文件权限与环境).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-System Administration (系统管理)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/03-System%20Administration%20(系统管理).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**进程与进程间通信、POSIX 线程生命周期、线程同步**
- [ ] [04-Processes and IPC (进程与进程间通信)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/04-Processes%20and%20IPC%20(进程与进程间通信).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-POSIX Thread Lifecycle (POSIX 线程生命周期)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/05-POSIX%20Thread%20Lifecycle%20(POSIX%20线程生命周期).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [06-Thread Synchronization (线程同步)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/06-Thread%20Synchronization%20(线程同步).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**信号与处理、IO 模型与多路复用、epoll API 与触发模式**
- [ ] [07-Signals and Signal Handling (信号与处理)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/07-Signals%20and%20Signal%20Handling%20(信号与处理).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [08-IO Models and Multiplexing (IO 模型与多路复用)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/08-IO%20Models%20and%20Multiplexing%20(IO%20模型与多路复用).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [09-epoll API and Trigger Modes (epoll API 与触发模式)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/09-epoll%20API%20and%20Trigger%20Modes%20(epoll%20API%20与触发模式).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**epoll 内部实现、零拷贝、系统调用与动态库**
- [ ] [10-epoll Internals (epoll 内部实现)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/10-epoll%20Internals%20(epoll%20内部实现).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [11-Zero Copy (零拷贝)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/11-Zero%20Copy%20(零拷贝).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [12-System Calls and Libraries (系统调用与动态库)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/12-System%20Calls%20and%20Libraries%20(系统调用与动态库).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**调试与追踪、TCP 与 IP 基础、TCP 握手与状态机**
- [ ] [13-Debugging and Tracing (调试与追踪)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/13-Debugging%20and%20Tracing%20(调试与追踪).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-TCP and IP Fundamentals (TCP 与 IP 基础)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/01-TCP%20and%20IP%20Fundamentals%20(TCP%20与%20IP%20基础).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-TCP Handshake and State Machine (TCP 握手与状态机)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/02-TCP%20Handshake%20and%20State%20Machine%20(TCP%20握手与状态机).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**TCP 可靠传输与拥塞控制、TCP 流式分帧、UDP**
- [ ] [03-TCP Reliability Flow and Congestion (TCP 可靠传输与拥塞控制)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/03-TCP%20Reliability%20Flow%20and%20Congestion%20(TCP%20可靠传输与拥塞控制).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-TCP Stream Framing (TCP 流式分帧)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/04-TCP%20Stream%20Framing%20(TCP%20流式分帧).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-UDP (UDP)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/05-UDP%20(UDP).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**HTTP 协议、TLS 与 HTTPS、HTTP 2**
- [ ] [06-HTTP Protocol (HTTP 协议)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/06-HTTP%20Protocol%20(HTTP%20协议).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [07-TLS and HTTPS (TLS 与 HTTPS)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/07-TLS%20and%20HTTPS%20(TLS%20与%20HTTPS).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [08-HTTP 2 (HTTP 2)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/08-HTTP%202%20(HTTP%202).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**gRPC 与 Protobuf、Socket 与事件循环、Reactor 架构**
- [ ] [09-gRPC and Protobuf (gRPC 与 Protobuf)](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/09-gRPC%20and%20Protobuf%20(gRPC%20与%20Protobuf).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Sockets and Event Loop (Socket 与事件循环)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/01-Sockets%20and%20Event%20Loop%20(Socket%20与事件循环).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Reactor Architecture (Reactor 架构)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/02-Reactor%20Architecture%20(Reactor%20架构).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**定时器、连接缓冲与分帧、压测**
- [ ] [03-Timers (定时器)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/03-Timers%20(定时器).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Connections Buffers and Framing (连接缓冲与分帧)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/04-Connections%20Buffers%20and%20Framing%20(连接缓冲与分帧).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Benchmarking (压测)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/05-Benchmarking%20(压测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**优雅关闭与请求排空、后端架构模式、系统与网络标准与自测**
- [ ] [06-Graceful Shutdown and Drain (优雅关闭与请求排空)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/06-Graceful%20Shutdown%20and%20Drain%20(优雅关闭与请求排空).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [07-Backend Architecture Patterns (后端架构模式)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/07-Backend%20Architecture%20Patterns%20(后端架构模式).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Systems and Networking Standard (系统与网络标准与自测)](/06-Systems%20and%20Networking%20(系统与网络)/00-Systems%20and%20Networking%20Standard%20(系统与网络标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

# 阶段五 · 数据系统与 Go 服务

**SQL 定义与数据操作、SQL 查询连接与聚合、MySQL 环境与元数据**
- [ ] [01-SQL Definition and Manipulation (SQL 定义与数据操作)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/01-SQL%20Definition%20and%20Manipulation%20(SQL%20定义与数据操作).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-SQL Queries Joins and Aggregation (SQL 查询连接与聚合)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/02-SQL%20Queries%20Joins%20and%20Aggregation%20(SQL%20查询连接与聚合).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-MySQL Environment and Metadata (MySQL 环境与元数据)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/03-MySQL%20Environment%20and%20Metadata%20(MySQL%20环境与元数据).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**InnoDB 存储与索引、事务与隔离级别、MVCC 与 Read View**
- [ ] [04-InnoDB Storage and Indexes (InnoDB 存储与索引)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/04-InnoDB%20Storage%20and%20Indexes%20(InnoDB%20存储与索引).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Transactions and Isolation (事务与隔离级别)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/05-Transactions%20and%20Isolation%20(事务与隔离级别).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [06-MVCC and Read Views (MVCC 与 Read View)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/06-MVCC%20and%20Read%20Views%20(MVCC%20与%20Read%20View).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**锁与死锁、日志与恢复、查询分析与优化**
- [ ] [07-Locks and Deadlocks (锁与死锁)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/07-Locks%20and%20Deadlocks%20(锁与死锁).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [08-Logging and Recovery (日志与恢复)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/08-Logging%20and%20Recovery%20(日志与恢复).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [09-Query Analysis and Optimization (查询分析与优化)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/09-Query%20Analysis%20and%20Optimization%20(查询分析与优化).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**连接池、复制、分库分表**
- [ ] [10-Connection Pool (连接池)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/10-Connection%20Pool%20(连接池).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [11-Replication (复制)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/11-Replication%20(复制).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [12-Sharding and Partitioning (分库分表)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/12-Sharding%20and%20Partitioning%20(分库分表).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**Redis 数据结构、Redis 持久化与淘汰、缓存一致性问题**
- [ ] [01-Redis Data Structures (Redis 数据结构)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/01-Redis%20Data%20Structures%20(Redis%20数据结构).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Redis Persistence and Eviction (Redis 持久化与淘汰)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/02-Redis%20Persistence%20and%20Eviction%20(Redis%20持久化与淘汰).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Cache Consistency Problems (缓存一致性问题)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/03-Cache%20Consistency%20Problems%20(缓存一致性问题).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**Redis 运行时与客户端、MySQL 与 Redis 缓存、Nginx 代理与负载均衡**
- [ ] [04-Redis Runtime and Client (Redis 运行时与客户端)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/04-Redis%20Runtime%20and%20Client%20(Redis%20运行时与客户端).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [13-MySQL and Redis Caching (MySQL 与 Redis 缓存)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/13-MySQL%20and%20Redis%20Caching%20(MySQL%20与%20Redis%20缓存).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Nginx Proxy and Load Balancing (Nginx 代理与负载均衡)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/05-Nginx%20Proxy%20and%20Load%20Balancing%20(Nginx%20代理与负载均衡).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**消息队列、Kafka 投递与幂等、RabbitMQ 与 Kafka 选型**
- [ ] [01-Message Queues (消息队列)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/03-Messaging%20and%20Streaming%20(消息与流)/01-Message%20Queues%20(消息队列).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Kafka Delivery and Idempotency (Kafka 投递与幂等)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/03-Messaging%20and%20Streaming%20(消息与流)/02-Kafka%20Delivery%20and%20Idempotency%20(Kafka%20投递与幂等).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-RabbitMQ and Kafka Selection (RabbitMQ 与 Kafka 选型)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/03-Messaging%20and%20Streaming%20(消息与流)/03-RabbitMQ%20and%20Kafka%20Selection%20(RabbitMQ%20与%20Kafka%20选型).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**一致性与 CAP、Raft 共识、Paxos**
- [ ] [01-Consistency and CAP (一致性与 CAP)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/01-Consistency%20and%20CAP%20(一致性与%20CAP).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Raft Consensus (Raft 共识)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/02-Raft%20Consensus%20(Raft%20共识).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Paxos (Paxos)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/03-Paxos%20(Paxos).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**分布式事务、数据系统与分布式标准与自测**
- [ ] [04-Distributed Transactions (分布式事务)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/04-Distributed%20Transactions%20(分布式事务).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Data Systems and Distributed Computing Standard (数据系统与分布式标准与自测)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/00-Data%20Systems%20and%20Distributed%20Computing%20Standard%20(数据系统与分布式标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**Go 后端**

**语法、值、结构体与方法、切片、映射、字符串与内存、接口、嵌入与泛型**
- [ ] [01-Syntax Values Structs and Methods (语法、值、结构体与方法)](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/01-Syntax%20Values%20Structs%20and%20Methods%20(语法、值、结构体与方法).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Slices Maps Strings and Memory (切片、映射、字符串与内存)](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/02-Slices%20Maps%20Strings%20and%20Memory%20(切片、映射、字符串与内存).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Interfaces Embedding and Generics (接口、嵌入与泛型)](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/03-Interfaces%20Embedding%20and%20Generics%20(接口、嵌入与泛型).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**模块包与工具链、错误、panic、defer 与资源安全、并发原语**
- [ ] [04-Modules Packages and Tooling (模块包与工具链)](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/04-Modules%20Packages%20and%20Tooling%20(模块包与工具链).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Errors Panic Defer and Resource Safety (错误、panic、defer 与资源安全)](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/05-Errors%20Panic%20Defer%20and%20Resource%20Safety%20(错误、panic、defer%20与资源安全).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Goroutines Channels and sync (并发原语)](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/01-Goroutines%20Channels%20and%20sync%20(并发原语).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**取消传播与错误、调度 GC 与内存、HTTP JSON与中间件**
- [ ] [02-Context Cancellation and Errors (取消传播与错误)](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/02-Context%20Cancellation%20and%20Errors%20(取消传播与错误).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Scheduler GC and Memory (调度 GC 与内存)](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/03-Scheduler%20GC%20and%20Memory%20(调度%20GC%20与内存).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-HTTP JSON and Middleware (HTTP JSON与中间件)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/01-HTTP%20JSON%20and%20Middleware%20(HTTP%20JSON与中间件).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**路由校验与错误、服务生命周期、测试与质量**
- [ ] [02-Routing Validation and Errors (路由校验与错误)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/02-Routing%20Validation%20and%20Errors%20(路由校验与错误).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-API Lifecycle and Graceful Shutdown (服务生命周期)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/03-API%20Lifecycle%20and%20Graceful%20Shutdown%20(服务生命周期).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [04-Testing and Quality (测试与质量)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/04-Testing%20and%20Quality%20(测试与质量).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**可观测性与运行、RPC、Protobuf 与接口演进、SQL 访问与事务**
- [ ] [05-Observability and Operations (可观测性与运行)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/05-Observability%20and%20Operations%20(可观测性与运行).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [06-RPC Protobuf and API Evolution (RPC、Protobuf 与接口演进)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/06-RPC%20Protobuf%20and%20API%20Evolution%20(RPC、Protobuf%20与接口演进).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-SQL Access and Transactions (SQL 访问与事务)](/05-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/01-SQL%20Access%20and%20Transactions%20(SQL%20访问与事务).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**Redis 缓存模式、消息与幂等、Go 服务实践流程**
- [ ] [02-Redis Cache Patterns (Redis 缓存模式)](/05-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/02-Redis%20Cache%20Patterns%20(Redis%20缓存模式).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Messaging and Idempotency (消息与幂等)](/05-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/03-Messaging%20and%20Idempotency%20(消息与幂等).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Go Service Practice Workflow (Go 服务实践流程)](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/01-Go%20Service%20Practice%20Workflow%20(Go%20服务实践流程).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**C++ 到 Go 迁移决策、首个服务交付**
- [ ] [02-C++ to Go Decision Notes (C++ 到 Go 迁移决策)](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/02-C++%20to%20Go%20Decision%20Notes%20(C++%20到%20Go%20迁移决策).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-First Service Delivery (首个服务交付)](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/03-First%20Service%20Delivery%20(首个服务交付).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**多语言协作与工程规范**

**多语言协作、Python 工程标准与自测、Go 后端标准与自测**
- [ ] [03-C++ Go Python Collaboration (多语言协作)](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/03-C++%20Go%20Python%20Collaboration%20(多语言协作).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Python Engineering Standard (Python 工程标准与自测)](/04-Python%20Engineering%20(Python%20工程)/00-Python%20Engineering%20Standard%20(Python%20工程标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Go Backend Standard (Go 后端标准与自测)](/05-Go%20Backend%20(Go%20后端)/00-Go%20Backend%20Standard%20(Go%20后端标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

# 阶段六 · 第一轮后端项目

**短链接服务、即时通讯与通知、秒杀项目**
- [ ] [01-URL Shortener (短链接服务)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/01-URL%20Shortener%20(短链接服务).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-IM and Notification (即时通讯与通知)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/02-IM%20and%20Notification%20(即时通讯与通知).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Flash Sale Project (秒杀项目)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/03-Flash%20Sale%20Project%20(秒杀项目).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**需求与范围**
- [ ] 选定一个主项目，写清目标用户、核心场景、非目标和验收条件。
- [ ] 拆成可演示的最小端到端路径，并记录风险与依赖。
- [ ] 定义数据模型和 API 契约，标出事务、幂等与权限边界。
- [ ] 整合本周产物并说明失败边界。

**贯通主路径**
- [ ] 搭建可重复运行的开发环境与项目骨架。
- [ ] 实现从请求、业务处理到持久化和响应的纵向切片。
- [ ] 为关键路径补输入校验、错误语义和基础测试。
- [ ] 整合本周产物并说明失败边界。

**数据与并发**
- [ ] 按项目需要实现索引、事务或并发控制。
- [ ] 处理重复请求、超时和部分失败，避免重复副作用。
- [ ] 验证缓存或队列是否真的必要，不为架构而堆组件。
- [ ] 整合本周产物并说明失败边界。

**可靠性与安全**
- [ ] 为连接、请求和后台任务设置资源上限与 timeout。
- [ ] 补身份授权、秘密管理、日志脱敏和失败降级。
- [ ] 用故障注入验证重试、恢复和优雅关闭。
- [ ] 整合本周产物并说明失败边界。

**测试与可观测**
- [ ] 补单元、集成和关键边界测试。
- [ ] 记录结构化日志、核心指标与请求关联信息。
- [ ] 让陌生人按 README 从干净环境启动并跑通测试。
- [ ] 整合本周产物并说明失败边界。

**性能与故障证据**
- [ ] 建立基准负载并记录硬件、数据集和参数。
- [ ] 定位一个实际瓶颈，完成有基线的优化或说明不优化的理由。
- [ ] 验证容量边界、超时/拒绝行为和恢复过程。
- [ ] 整合本周产物并说明失败边界。

**交付复盘**
- [ ] 完成演示路径、架构图和关键决策记录。
- [ ] 整理一份故障或性能案例，附可复核证据。
- [ ] 复盘未完成项、已知限制与下一阶段改进，不虚报完成度。
- [ ] 整合本周产物并说明失败边界。

# 阶段七 · 云原生与安全工程

**容器镜像与运行时、Kubernetes 工作负载与网络、配置、探针与发布**
- [ ] [01-Container Images and Runtime (容器镜像与运行时)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/01-Container%20Images%20and%20Runtime%20(容器镜像与运行时).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Kubernetes Workloads and Networking (Kubernetes 工作负载与网络)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/02-Kubernetes%20Workloads%20and%20Networking%20(Kubernetes%20工作负载与网络).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Configuration Probes and Rollouts (配置、探针与发布)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/03-Configuration%20Probes%20and%20Rollouts%20(配置、探针与发布).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**集群调度、资源与存储、Kubernetes 排障与生产运维、SLO、错误预算与告警**
- [ ] [04-Cluster Scheduling Resources and Storage (集群调度、资源与存储)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/04-Cluster%20Scheduling%20Resources%20and%20Storage%20(集群调度、资源与存储).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-Kubernetes Troubleshooting and Production Operations (Kubernetes 排障与生产运维)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/05-Kubernetes%20Troubleshooting%20and%20Production%20Operations%20(Kubernetes%20排障与生产运维).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-SLO Error Budgets and Alerting (SLO、错误预算与告警)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/03-Reliability%20Engineering%20(可靠性工程)/01-SLO%20Error%20Budgets%20and%20Alerting%20(SLO、错误预算与告警).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**容量、自动扩缩与成本、事故响应、混沌与恢复、基础设施即代码与 GitOps**
- [ ] [02-Capacity Autoscaling and Cost (容量、自动扩缩与成本)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/03-Reliability%20Engineering%20(可靠性工程)/02-Capacity%20Autoscaling%20and%20Cost%20(容量、自动扩缩与成本).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Incident Response Chaos and Recovery (事故响应、混沌与恢复)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/03-Reliability%20Engineering%20(可靠性工程)/03-Incident%20Response%20Chaos%20and%20Recovery%20(事故响应、混沌与恢复).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Infrastructure as Code and GitOps (基础设施即代码与 GitOps)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/02-Platform%20Engineering%20(平台工程)/01-Infrastructure%20as%20Code%20and%20GitOps%20(基础设施即代码与%20GitOps).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**开发者平台与黄金路径、策略、配置与多环境、云原生与可靠性标准与自测**
- [ ] [02-Developer Platforms and Golden Paths (开发者平台与黄金路径)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/02-Platform%20Engineering%20(平台工程)/02-Developer%20Platforms%20and%20Golden%20Paths%20(开发者平台与黄金路径).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Policy Configuration and Environments (策略、配置与多环境)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/02-Platform%20Engineering%20(平台工程)/03-Policy%20Configuration%20and%20Environments%20(策略、配置与多环境).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Cloud Native and Reliability Standard (云原生与可靠性标准与自测)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/00-Cloud%20Native%20and%20Reliability%20Standard%20(云原生与可靠性标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**威胁建模、身份与防御边界**

**威胁建模、认证与会话、授权与 API 安全、安全编码与内存安全**
- [ ] [01-Threat Modeling Authentication and Sessions (威胁建模、认证与会话)](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/01-Threat%20Modeling%20Authentication%20and%20Sessions%20(威胁建模、认证与会话).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Authorization and API Security (授权与 API 安全)](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/02-Authorization%20and%20API%20Security%20(授权与%20API%20安全).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Secure Coding and Memory Safety (安全编码与内存安全)](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/03-Secure%20Coding%20and%20Memory%20Safety%20(安全编码与内存安全).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**安全需求与验证、密钥、密码学与供应链、云身份与容器安全**
- [ ] [04-Security Requirements and Verification (安全需求与验证)](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/04-Security%20Requirements%20and%20Verification%20(安全需求与验证).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Secrets Cryptography and Supply Chain (密钥、密码学与供应链)](/09-Security%20Engineering%20(安全工程)/02-Identity%20Cloud%20and%20Supply%20Chain%20(身份、云与供应链)/01-Secrets%20Cryptography%20and%20Supply%20Chain%20(密钥、密码学与供应链).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Cloud Identity and Container Security (云身份与容器安全)](/09-Security%20Engineering%20(安全工程)/02-Identity%20Cloud%20and%20Supply%20Chain%20(身份、云与供应链)/02-Cloud%20Identity%20and%20Container%20Security%20(云身份与容器安全).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**安全测试与漏洞管理、沙箱与不可信执行、安全日志与检测**
- [ ] [03-Security Testing and Vulnerability Management (安全测试与漏洞管理)](/09-Security%20Engineering%20(安全工程)/02-Identity%20Cloud%20and%20Supply%20Chain%20(身份、云与供应链)/03-Security%20Testing%20and%20Vulnerability%20Management%20(安全测试与漏洞管理).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Sandboxing and Untrusted Execution (沙箱与不可信执行)](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/01-Sandboxing%20and%20Untrusted%20Execution%20(沙箱与不可信执行).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Security Logging and Detection (安全日志与检测)](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/02-Security%20Logging%20and%20Detection%20(安全日志与检测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**安全事件响应与恢复、安全工程标准与自测**
- [ ] [03-Incident Response and Recovery (安全事件响应与恢复)](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/03-Incident%20Response%20and%20Recovery%20(安全事件响应与恢复).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Security Engineering Standard (安全工程标准与自测)](/09-Security%20Engineering%20(安全工程)/00-Security%20Engineering%20Standard%20(安全工程标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

# 阶段八 · 配置平台、AI 系统与综合项目

**配置中心项目**
- [ ] [04-Config Center Project (配置中心项目)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/04-Config%20Center%20Project%20(配置中心项目).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**模型能力、应用设计与运行保障**

**Token、向量与模型行为、训练、推理与模型生命周期、模型 API 与结构化输出**
- [ ] [01-Tokens Embeddings and Model Behavior (Token、向量与模型行为)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/01-Tokens%20Embeddings%20and%20Model%20Behavior%20(Token、向量与模型行为).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Training Inference and Model Lifecycle (训练、推理与模型生命周期)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/02-Training%20Inference%20and%20Model%20Lifecycle%20(训练、推理与模型生命周期).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Model APIs and Structured Outputs (模型 API 与结构化输出)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/03-Model%20APIs%20and%20Structured%20Outputs%20(模型%20API%20与结构化输出).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**评估数据与实验设计、提示、上下文与工具调用、RAG 与向量检索**
- [ ] [04-Evaluation Data and Experiment Design (评估数据与实验设计)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/04-Evaluation%20Data%20and%20Experiment%20Design%20(评估数据与实验设计).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Prompt Context and Tool Calling (提示、上下文与工具调用)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/02-LLM%20Applications%20and%20Agents%20(大模型应用与智能体)/01-Prompt%20Context%20and%20Tool%20Calling%20(提示、上下文与工具调用).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-RAG and Vector Retrieval (RAG 与向量检索)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/02-LLM%20Applications%20and%20Agents%20(大模型应用与智能体)/02-RAG%20and%20Vector%20Retrieval%20(RAG%20与向量检索).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**Agent 工作流、评估与安全、推理服务、批处理与缓存、GPU 调度、容器与多租户**
- [ ] [03-Agent Workflow Evaluation and Safety (Agent 工作流、评估与安全)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/02-LLM%20Applications%20and%20Agents%20(大模型应用与智能体)/03-Agent%20Workflow%20Evaluation%20and%20Safety%20(Agent%20工作流、评估与安全).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Inference Serving Batching and Caching (推理服务、批处理与缓存)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/03-AI%20Infrastructure%20(AI%20基础设施)/01-Inference%20Serving%20Batching%20and%20Caching%20(推理服务、批处理与缓存).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-GPU Scheduling Containers and Multi Tenancy (GPU 调度、容器与多租户)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/03-AI%20Infrastructure%20(AI%20基础设施)/02-GPU%20Scheduling%20Containers%20and%20Multi%20Tenancy%20(GPU%20调度、容器与多租户).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**AI 可观测性、成本与安全、AI 系统工程标准与自测**
- [ ] [03-AI Observability Cost and Security (AI 可观测性、成本与安全)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/03-AI%20Infrastructure%20(AI%20基础设施)/03-AI%20Observability%20Cost%20and%20Security%20(AI%20可观测性、成本与安全).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-AI Systems Engineering Standard (AI 系统工程标准与自测)](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/00-AI%20Systems%20Engineering%20Standard%20(AI%20系统工程标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**受控智能体项目实践**

**安全 AI Agent 平台**
- [ ] [05-Secure AI Agent Platform (安全 AI Agent 平台)](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/05-Secure%20AI%20Agent%20Platform%20(安全%20AI%20Agent%20平台).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**项目质量与交付标准**

**项目标准与自测**
- [ ] [00-Project Standard (项目标准与自测)](/11-Projects%20(项目实践)/00-Project%20Standard%20(项目标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

# 阶段九 · 系统设计、表达与求职证据

**C++ 后端面试高频 100 题、数据速记、项目难点表达**
- [ ] [01-C++ Backend Interview Top 100 (C++ 后端面试高频 100 题)](/12-Career%20(求职与成长)/01-Interview%20Expression%20(面试表达)/01-C++%20Backend%20Interview%20Top%20100%20(C++%20后端面试高频%20100%20题).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-Data Quick Reference (数据速记)](/12-Career%20(求职与成长)/01-Interview%20Expression%20(面试表达)/02-Data%20Quick%20Reference%20(数据速记).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Project Deep Dive (项目难点表达)](/12-Career%20(求职与成长)/01-Interview%20Expression%20(面试表达)/03-Project%20Deep%20Dive%20(项目难点表达).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**短链接系统、即时通信系统、秒杀系统设计**
- [ ] [01-URL Shortener (短链接系统)](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/01-URL%20Shortener%20(短链接系统).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [02-IM System (即时通信系统)](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/02-IM%20System%20(即时通信系统).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Flash Sale System Design (秒杀系统设计)](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/03-Flash%20Sale%20System%20Design%20(秒杀系统设计).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**配置中心系统设计、API 网关、后端能力证据**
- [ ] [04-Config Center System Design (配置中心系统设计)](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/04-Config%20Center%20System%20Design%20(配置中心系统设计).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [05-API Gateway (API 网关)](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/05-API%20Gateway%20(API%20网关).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [01-Backend Competency Evidence (后端能力证据)](/12-Career%20(求职与成长)/03-Career%20Execution%20(求职执行)/01-Backend%20Competency%20Evidence%20(后端能力证据).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**简历证据与作品集、面试训练与复盘、求职能力标准与自测**
- [ ] [02-Resume Evidence and Portfolio (简历证据与作品集)](/12-Career%20(求职与成长)/03-Career%20Execution%20(求职执行)/02-Resume%20Evidence%20and%20Portfolio%20(简历证据与作品集).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [03-Interview Practice and Retrospective (面试训练与复盘)](/12-Career%20(求职与成长)/03-Career%20Execution%20(求职执行)/03-Interview%20Practice%20and%20Retrospective%20(面试训练与复盘).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。
- [ ] [00-Career Standard (求职能力标准与自测)](/12-Career%20(求职与成长)/00-Career%20Standard%20(求职能力标准与自测).md)：理解核心机制，完成笔记中的示例或推导，并说清适用边界：理解核心机制，完成笔记中的示例或推导，并说清适用边界。

**系统设计推演**
- [ ] 选一个已完成项目，画出请求、数据和依赖链路，并标注容量假设。
- [ ] 针对一个关键方案比较至少两种取舍，说明失败模式与回滚边界。
- [ ] 完成一次限时系统设计口述，检查需求澄清、容量估算和故障处理。
- [ ] 用证据复核设计中的假设，记录一项需要补学的基础。

**项目表达与证据**
- [ ] 为主项目整理问题背景、个人职责、架构和核心数据流。
- [ ] 选一个性能或故障案例，准备基线、改动、结果和限制证据。
- [ ] 做一次项目深挖演练，追问事务、并发、安全、可观测与恢复。
- [ ] 对照项目代码、测试和记录，删除没有证据支撑的表述。

**简历与岗位匹配**
- [ ] 将项目成果改写为可验证的简历条目，明确个人贡献与结果。
- [ ] 按目标岗位检查 C++、系统、数据库、Go 和工程实践的薄弱项。
- [ ] 完成一轮基础技术问答，按机制、边界、证据三项复盘。
- [ ] 将复盘问题回填错误记录，并为下一轮安排补学。

**综合模拟与收口**
- [ ] 完成一次项目讲解与追问模拟。
- [ ] 完成一次系统设计与故障排查模拟。
- [ ] 复核作品从干净环境能启动、测试和演示。
- [ ] 总结下一阶段岗位准备重点。
- [ ] 仍薄弱的知识回到对应阶段，不以勾选数代替掌握。

# 收束与调整

完成一个知识块后再进入后续依赖；若基础缺口影响理解，回到对应笔记补齐。每个阶段留下可运行结果、实验观察或项目证据。复习日期只改对应笔记属性，未学任务顺延，不把复习误作新学。
