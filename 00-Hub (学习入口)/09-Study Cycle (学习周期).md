---
study_stage: reference
tags: [learning/execution, learning/dashboard]
---

> [!abstract] 学习顺序
> C++ 打底，Python 辅助工程，Go 承接后端服务。按阶段与学习块向下推进，笔记是主线，实验帮助理解。

每条 Todo 按一个学习日的目标设计：读懂机制，再完成一项小验证；同条链接从左到右学习。内容吃力时可续学，也可空出一天自主复习。勾选表示能解释要点、验证示例并说明边界，不以读完或当天做完为准。

复习由 `study_stage` 与 `review_due` 独立维护；本页只安排新学与应用深化，不重复列复习日。项目条目中的“阅读设计”和“实现功能”分别推进，完整项目允许跨多个学习日。

# 阶段一 · 程序、语言与资源基础

从能运行程序开始，交错理解计算机、C++ 与 Python；工具随实际需要引入。

**程序起点与运行环境**

- [x] [计算机系统总览](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/01-Computer%20System%20Overview%20(计算机系统总览).md)：串起源代码、可执行文件与进程的关系。
- [x] [核心语法与函数](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/01-Core%20Syntax%20and%20Functions%20(核心语法与函数).md)：用小程序区分值传递、引用传递与作用域。
- [x] [运行环境、模块与包管理](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/01-Runtime%20Modules%20Packages%20and%20uv%20(运行环境、模块与包管理).md)：确认解释器、模块入口与 uv 项目，运行最小程序。
- [x] [核心语法、容器与推导式](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/03-Core%20Syntax%20Collections%20and%20Comprehensions%20(核心语法、容器与推导式).md)：用容器与推导式处理一组数据，检查控制流。
- [ ] [数组、字符串与输入输出](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/02-Arrays%20Strings%20and%20IO%20(数组字符串与输入输出).md)：完成输入与字符串处理，检查索引和长度边界。
- [ ] [对象、名称、可变性与复制](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/02-Objects%20Names%20Mutability%20and%20Copying%20(对象、名称、可变性与复制).md)：用短实验验证引用与复制。
- [ ] [数据表示](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/02-Data%20Representation%20(数据表示).md)：用整数溢出和浮点误差理解数据表示。
- [ ] [Shell 命令与管道](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/01-Shell%20Commands%20and%20Pipelines%20(Shell%20命令与管道).md)：组合文件查找与文本管道，检查路径和退出码。
- [ ] [文件权限与环境](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/02-Files%20Permissions%20and%20Environment%20(文件权限与环境).md)：掌握权限、环境变量与配置来源，定位环境差异。

**类型、函数与构建过程**

- [ ] [类型、指针与引用](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/01-Types%20Pointers%20and%20References%20(类型指针与引用).md)：区分对象、指针与引用，识别空指针和悬垂引用。
- [ ] [头文件、命名空间与分离编译](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/03-Headers%20Namespaces%20and%20Separate%20Compilation%20(头文件命名空间与分离编译).md)：拆分声明与定义，编译链接一个多文件程序。
- [ ] [程序构建与执行](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Program%20Build%20and%20Execution%20(程序构建与执行).md)：观察预处理、编译、链接与装载各自完成什么。
- [ ] [CMake 项目与目标](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md)：用 target 组织库和程序，完成一次干净构建。
- [ ] [函数、作用域、闭包与装饰器](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/04-Functions%20Scope%20Closures%20and%20Decorators%20(函数、作用域、闭包与装饰器).md)：完成最小闭包和装饰器示例。
- [ ] [迭代、生成器与协议](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/05-Iteration%20Generators%20and%20Protocols%20(迭代、生成器与协议).md)：实现一个惰性数据流水线。
- [ ] [内存布局与分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)：观察栈、堆与对象地址，区分存储期和生命周期。
- [ ] [存储层级与输入输出](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/04-Memory%20Hierarchy%20and%20IO%20(存储层级与输入输出).md)：解释缓存局部性，并比较顺序与随机访问。

**对象与资源管理**

- [ ] [对象生命周期与拷贝控制](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)：跟踪构造、拷贝、赋值与析构，验证资源所有权。
- [ ] [值类别与移动语义](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)：观察复制与移动调用，说明 std::move 的作用。
- [ ] [RAII 与自定义分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)：先实现 RAII 资源封装，再理解自定义分配的用途。
- [ ] [智能指针](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Smart%20Pointers%20(智能指针).md)：把资源封装迁移为明确的所有权模型。
- [ ] [类、数据类与对象模型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/06-Classes%20Dataclasses%20and%20Object%20Model%20(类、数据类与对象模型).md)：用类与数据类组织数据，理解属性和方法查找。
- [ ] [异常、上下文管理与类型](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/08-Exceptions%20Context%20Managers%20and%20Typing%20(异常、上下文与类型).md)：处理失败与资源释放，对照 C++ RAII 的作用。
- [ ] [模板](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/05-Templates%20(模板).md)：写一个简单函数模板，理解推导与实例化。
- [ ] [现代 C++ 基础](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Modern%20C++%20Foundations%20(现代%20C++%20基础).md)：掌握类型推导、初始化与 Lambda 的常见用法。

**文件处理与最小交付**

- [ ] [路径、文件、序列化与时间](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/07-Paths%20Files%20Serialization%20and%20Time%20(路径、文件、序列化与时间).md)：读写路径与结构化数据，处理编码、时间和异常。
- [ ] [命令行、配置与子进程](/04-Python%20Engineering%20(Python%20工程)/01-Python%20Foundations%20(Python%20基础)/09-CLI%20Configuration%20and%20Subprocesses%20(命令行、配置与子进程).md)：为已有脚本添加命令行参数，并检查子进程退出码。
- [ ] [问题求解与调试](/01-Foundations%20(基础能力)/04-Foundation%20Practice%20(基础实践)/01-Problem%20Solving%20and%20Debugging%20(问题求解与调试).md)：从最小复现定位问题，区分现象、假设与证据。
- [ ] [测试与验证](/01-Foundations%20(基础能力)/04-Foundation%20Practice%20(基础实践)/02-Testing%20and%20Verification%20(测试与验证).md)：为正常、边界和失败输入建立一组可重复测试。
- [ ] [pytest 夹具与测试替身](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/01-pytest%20Fixtures%20and%20Test%20Doubles%20(测试夹具与替身).md)：给已有函数补测试，用夹具隔离文件或外部依赖。
- [ ] [类型标注、日志与静态检查](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/02-Type%20Hints%20Logging%20and%20Linting%20(类型日志与静态检查).md)：为已有 Python 代码接入类型检查、日志与 lint。
- [ ] [Python 项目实践流程](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/01-Python%20Project%20Practice%20Workflow%20(Python%20项目实践流程).md)：确认入口、依赖与运行方式，整理一个可复现小项目。
- [ ] [刻意练习与知识维护](/01-Foundations%20(基础能力)/04-Foundation%20Practice%20(基础实践)/03-Deliberate%20Practice%20and%20Knowledge%20Maintenance%20(刻意练习与知识维护).md)：用一道已学问题练习独立解释、验证和纠错。

# 阶段二 · 数据结构与推理方法

先理解结构与不变量，再学习搜索和动态规划；已有基础用于加快推演，刷题量自行调节。

**基本结构与算法选择**

- [ ] [数组与链表](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Arrays%20and%20Linked%20Lists%20(数组与链表).md)：复现双指针与链表反转，检查空输入和复杂度。
- [ ] [栈队列与哈希](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Stack%20Queue%20and%20Hashing%20(栈队列与哈希).md)：比较栈、队列与哈希表，为典型操作选择结构。
- [ ] [二分查找](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/07-Binary%20Search%20(二分查找).md)：写出统一区间模板，覆盖空数组、单元素和边界命中测试。
- [ ] [堆与 Top K](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Heap%20and%20Top%20K%20(堆与%20Top%20K).md) + [排序算法](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/08-Sorting%20Algorithms%20(排序算法).md)：比较 Top K 的堆与排序方案，分析复杂度和适用条件。
- [ ] [树遍历与路径](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/04-Tree%20Traversal%20and%20Paths%20(树遍历与路径).md) + [二叉搜索树](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/05-Binary%20Search%20Trees%20(二叉搜索树).md)：实现遍历与查找，解释路径状态和搜索树不变量。
- [ ] [贪心与滑动窗口](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/14-Greedy%20and%20Sliding%20Window%20(贪心与滑动窗口).md)：分别验证贪心选择依据与滑动窗口不变量。

**搜索、状态与动态规划**

- [ ] [离散逻辑、集合与图](/01-Foundations%20(基础能力)/03-Mathematics%20and%20Reasoning%20(数学与推理)/01-Discrete%20Logic%20Sets%20and%20Graphs%20(离散逻辑、集合与图).md)：用命题、集合与图描述问题，写出一个不变量。
- [ ] [回溯与状态空间搜索](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/09-Backtracking%20and%20State%20Space%20Search%20(回溯与状态空间搜索).md)：明确选择、撤销与剪枝，比较排列和组合去重。
- [ ] [图并查集与最短路](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/06-Graphs%20Union%20Find%20and%20Shortest%20Paths%20(图并查集与最短路).md)：区分连通、依赖与最短路问题，手推代表算法。
- [ ] [一维动态规划](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/10-One%20Dimensional%20Dynamic%20Programming%20(一维动态规划).md)：从状态含义推导转移、初始化与空间压缩。
- [ ] [背包动态规划](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/11-Knapsack%20Dynamic%20Programming%20(背包动态规划).md)：手推 0-1 和完全背包，解释循环方向。
- [ ] [序列动态规划](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/13-Sequence%20Dynamic%20Programming%20(序列动态规划).md)：比较序列 DP 与 tails 方法，解释优化条件。
- [ ] [区间动态规划](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/12-Interval%20Dynamic%20Programming%20(区间动态规划).md)：推导区间状态与枚举顺序，验证最小规模。
- [ ] [字符串与位运算](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/15-String%20and%20Bit%20Algorithms%20(字符串与位运算).md)：推演字符串匹配，并检查位运算的整数宽度。

**系统模型与数量推理**

- [ ] [操作系统总览与启动](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/05-Operating%20System%20Overview%20and%20Boot%20(操作系统总览与启动).md)：串起固件、引导程序、内核与用户进程。
- [ ] [进程与线程](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/06-Processes%20and%20Threads%20(进程与线程).md)：比较进程与线程的地址空间和共享资源。
- [ ] [上下文切换与调度](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/07-Context%20Switching%20and%20Scheduling%20(上下文切换与调度).md)：解释时间片与上下文切换，比较计算和 I/O 任务。
- [ ] [虚拟内存、分页与分配](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)：手推地址转换，区分缺页、分页和内存分配。
- [ ] [文件系统与存储](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/09-File%20Systems%20and%20Storage%20(文件系统与存储).md)：观察文件描述符、偏移与权限，理解持久化边界。
- [ ] [概率、统计与排队](/01-Foundations%20(基础能力)/03-Mathematics%20and%20Reasoning%20(数学与推理)/02-Probability%20Statistics%20and%20Queueing%20(概率、统计与排队).md)：理解条件概率、抽样误差与排队，分析一个过载场景。
- [ ] [性能与系统思维](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/12-Performance%20and%20System%20Thinking%20(性能与系统思维).md)：用 Amdahl 与 Little 定律解释性能上限和排队。
- [ ] [线性代数与数值直觉](/01-Foundations%20(基础能力)/03-Mathematics%20and%20Reasoning%20(数学与推理)/03-Linear%20Algebra%20and%20Numerical%20Intuition%20(线性代数与数值直觉).md)：用向量与矩阵表达计算，观察误差与数量级。

# 阶段三 · 构建、测试与工程习惯

把前面的短程序整理为可构建、可测试、可定位故障的小工程。

**版本管理与可复现构建**

- [ ] [Git 模型与分支](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/01-Git%20Model%20and%20Branches%20(Git%20模型与分支).md)：理解提交、分支与合并，在练习仓库完成一次变更。
- [ ] [冲突与恢复](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/02-Conflicts%20and%20Recovery%20(冲突与恢复).md)：演练冲突处理与误操作恢复，区分恢复命令的作用。
- [ ] [CMake 依赖](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/02-CMake%20Dependencies%20(CMake%20依赖).md)：引入一个依赖，验证头文件与链接依赖的传递。
- [ ] [包管理与可复现构建](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/03-Package%20Management%20and%20Reproducible%20Builds%20(包管理与可复现构建).md)：固定依赖并重建，解释锁文件和环境的作用。
- [ ] [Make 与 Shell 自动化](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/04-Make%20and%20Shell%20Automation%20(Make%20与%20Shell%20自动化).md)：把已有构建步骤整理为脚本，检查参数与退出码。
- [ ] [构建组织与预处理](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/01-Build%20Organization%20and%20Preprocessing%20(构建组织与预处理).md)：解释符号、链接与重定位，排查一个构建错误。
- [ ] [CMake 与依赖](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/02-CMake%20and%20Dependencies%20(CMake%20与依赖).md)：组织库与测试目标，验证依赖和构建配置。

**测试、诊断与 Python 工程质量**

- [ ] [测试策略与证据](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)：划分单元和集成测试，明确替身与真实依赖的边界。
- [ ] [调试与故障定位](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/02-Debugging%20and%20Failure%20Localization%20(调试与故障定位).md)：用断点、调用栈和变量定位一个可复现故障。
- [ ] [内存安全与动态分析](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/03-Memory%20Safety%20and%20Dynamic%20Analysis%20(内存安全与动态分析).md)：用 Sanitizer 定位内存错误，说明检测范围。
- [ ] [异常与调试](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/03-Exceptions%20and%20Debugging%20(异常与调试).md)：比较异常安全保证，验证 RAII 的回滚行为。
- [ ] [测试与可观测性](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/04-Testing%20and%20Observability%20(测试与可观测性).md)：为现有 C++ 模块接入测试与必要日志。
- [ ] [持续集成与交付](/02-Engineering%20Fundamentals%20(工程基础)/01-Git%20and%20Delivery%20(Git%20与交付)/03-CI%20CD%20(持续集成与交付).md)：把已有构建和测试接入 CI，检查失败反馈与制品。
- [ ] [自动化项目模板](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/02-Automation%20Project%20Template%20(自动化项目模板).md) + [项目质量基线](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/03-Project%20Quality%20Baseline%20(项目质量基线).md)：为已有脚本建立项目骨架，接入检查与测试。
- [ ] [测试打包与交付](/04-Python%20Engineering%20(Python%20工程)/04-Python%20Practice%20(Python%20实践)/03-Testing%20Packaging%20and%20Delivery%20(测试打包与交付).md)：在干净目录验证安装、启动和使用说明。
- [ ] [数据处理边界](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/02-Data%20Processing%20Boundaries%20(数据处理边界).md)：定义数据输入范围，验证解析失败与异常输入。
- [ ] [性质、模糊与集成测试](/04-Python%20Engineering%20(Python%20工程)/02-Quality%20and%20Automation%20(质量与自动化)/04-Property%20Fuzz%20and%20Integration%20Testing%20(性质、模糊与集成测试).md)：选择一个解析器，补性质测试或模糊测试。

# 阶段四 · C++ 泛型、并发与性能

沿容器、泛型和并发深入 C++，重专题各用一条 Todo 集中推演。

**容器、对象与泛型接口**

- [ ] [顺序容器](/03-C++%20Backend%20(C++%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/01-Sequence%20Containers%20(顺序容器).md)：比较顺序容器的成本，验证迭代器失效。
- [ ] [关联容器](/03-C++%20Backend%20(C++%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/02-Associative%20Containers%20(关联容器).md)：比较有序与哈希容器，验证比较器和哈希约束。
- [ ] [迭代器算法与适配器](/03-C++%20Backend%20(C++%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/03-Iterators%20Algorithms%20and%20Adapters%20(迭代器算法与适配器).md)：用标准算法处理容器，检查迭代器类别与失效。
- [ ] [多态与继承](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/04-Polymorphism%20and%20Inheritance%20(多态与继承).md)：理解虚调用、对象切片与多态析构。
- [ ] [完美转发与万能引用](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/04-Perfect%20Forwarding%20and%20Universal%20References%20(完美转发与万能引用).md)：手推引用折叠，实现保留值类别的转发包装。
- [ ] [词汇类型与视图](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/06-Vocabulary%20Types%20and%20Views%20(词汇类型与视图).md)：用 optional、variant 和视图表达接口意图。
- [ ] [编译期编程](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/05-Compile%20Time%20Programming%20(编译期编程).md)：区分 const、constexpr 与 consteval，验证编译期计算。
- [ ] [Concepts 与 SFINAE](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/07-Concepts%20and%20SFINAE%20(Concepts%20与%20SFINAE).md)：为模板添加约束，比较约束失败与替换失败。

**并发机制、性能与工程检验**

- [ ] [同步原语](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/10-Synchronization%20Primitives%20(同步原语).md)：区分锁、条件变量与信号量，明确共享状态和等待条件。
- [ ] [死锁分析与恢复](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/11-Deadlock%20Analysis%20and%20Recovery%20(死锁分析与恢复).md)：画出等待图，用统一锁顺序消除一个死锁。
- [ ] [线程锁与协作](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/01-Threads%20Locks%20and%20Coordination%20(线程锁与协作).md)：实现互斥和条件变量协作，解释等待谓词。
- [ ] [原子与内存序](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/02-Atomics%20and%20Memory%20Order%20(原子与内存序).md)：推导发布与获取的同步关系，说明 relaxed 的边界。
- [ ] [线程池](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/03-Thread%20Pool%20(线程池).md)：实现最小线程池，验证提交、等待与停止。
- [ ] [无锁与性能](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/04-Lock%20Free%20and%20Performance%20(无锁与性能).md)：理解 ABA、回收与进展保证，分析一个无锁示例。
- [ ] [协程](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/08-Coroutines%20(协程).md)：跟踪协程帧和句柄生命周期，运行最小示例。
- [ ] [性能基准与剖析](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/04-Performance%20Benchmarking%20and%20Profiling%20(性能基准与剖析).md)：建立基线并读取剖析结果，区分相关性与因果。
- [ ] [性能分析与优化](/03-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/05-Profiling%20and%20Optimization%20(性能分析与优化).md)：针对一个已测瓶颈改动，比较收益与代价。
- [ ] [C++ 后端标准与自测](/03-C++%20Backend%20(C++%20后端)/00-C++%20Backend%20Standard%20(C++%20后端标准与自测).md)：抽查对象、并发与工程能力，定位仍需专攻的主题。
- [ ] [基础能力标准与自测](/01-Foundations%20(基础能力)/00-Foundations%20Standard%20(基础能力标准与自测).md)：抽查基础概念与算法推导，把缺口定位到原笔记。

# 阶段五 · Linux 与网络协议

把进程和内存模型落到 Linux，再理解端到端通信和应用协议。

**进程、线程与系统接口**

- [ ] [系统管理](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/03-System%20Administration%20(系统管理).md)：检查进程、用户与磁盘，理解服务运行环境。
- [ ] [系统调用与动态库](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/12-System%20Calls%20and%20Libraries%20(系统调用与动态库).md)：用 strace 观察调用，区分库函数与系统调用。
- [ ] [进程与进程间通信](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/04-Processes%20and%20IPC%20(进程与进程间通信).md)：用 fork/exec/wait 观察进程，比较常见 IPC。
- [ ] [POSIX 线程生命周期](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/05-POSIX%20Thread%20Lifecycle%20(POSIX%20线程生命周期).md)：创建与回收线程，检查参数和返回值的生命周期。
- [ ] [线程同步](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/06-Thread%20Synchronization%20(线程同步).md)：实现共享队列，解释条件变量的循环等待。
- [ ] [信号与处理](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/07-Signals%20and%20Signal%20Handling%20(信号与处理).md)：验证信号控制流程，理解处理器的安全限制。
- [ ] [调试与追踪](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/13-Debugging%20and%20Tracing%20(调试与追踪).md)：用追踪与调试工具定位一次进程或 I/O 故障。

**传输机制与套接字**

- [ ] [TCP 与 IP 基础](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/01-TCP%20and%20IP%20Fundamentals%20(TCP%20与%20IP%20基础).md)：串起分层、寻址与端口分用，理解数据如何到达进程。
- [ ] [TCP 握手与状态机](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/02-TCP%20Handshake%20and%20State%20Machine%20(TCP%20握手与状态机).md)：手推握手与关闭状态，解释重传和连接队列。
- [ ] [TCP 可靠传输与拥塞控制](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/03-TCP%20Reliability%20Flow%20and%20Congestion%20(TCP%20可靠传输与拥塞控制).md)：区分可靠传输、流量控制与拥塞控制。
- [ ] [UDP 协议](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/05-UDP%20(UDP).md)：运行数据报收发，解释消息边界、丢失与乱序。
- [ ] [TCP 流式分帧](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/04-TCP%20Stream%20Framing%20(TCP%20流式分帧).md)：设计长度前缀解析，覆盖分段和合并到达。
- [ ] [IO 模型与多路复用](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/08-IO%20Models%20and%20Multiplexing%20(IO%20模型与多路复用).md)：比较阻塞、非阻塞与多路复用的等待流程。
- [ ] [Socket 与事件循环](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/01-Sockets%20and%20Event%20Loop%20(Socket%20与事件循环).md)：先运行阻塞 TCP 收发，再改为非阻塞事件循环。

**I/O 多路复用与应用协议**

- [ ] [epoll API 与触发模式](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/09-epoll%20API%20and%20Trigger%20Modes%20(epoll%20API%20与触发模式).md)：运行 LT/ET 示例，验证非阻塞读写与 EAGAIN。
- [ ] [epoll 内部实现](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/10-epoll%20Internals%20(epoll%20内部实现).md)：区分兴趣集合与就绪队列，说明复杂度和拷贝边界。
- [ ] [零拷贝](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/11-Zero%20Copy%20(零拷贝).md)：画出 sendfile 等数据路径，比较适用限制。
- [ ] [HTTP 协议](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/06-HTTP%20Protocol%20(HTTP%20协议).md)：观察请求与响应，理解方法、状态码和消息边界。
- [ ] [TLS 与 HTTPS](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/07-TLS%20and%20HTTPS%20(TLS%20与%20HTTPS).md)：串起证书校验与密钥协商，解释认证和前向安全。
- [ ] [HTTP/2 协议](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/08-HTTP%202%20(HTTP%202).md)：理解多路复用与 HPACK，区分两层队头阻塞。
- [ ] [gRPC 与 Protobuf](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/09-gRPC%20and%20Protobuf%20(gRPC%20与%20Protobuf).md)：调用最小 RPC，验证超时、错误与字段兼容。

# 阶段六 · 服务组织与接口实践

用已学机制组织小型服务，同时让 Python 承担接口检查与自动化。

**事件驱动与服务生命周期**

- [ ] [Reactor 架构](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/02-Reactor%20Architecture%20(Reactor%20架构).md)：划分接入、事件分发与业务处理职责。
- [ ] [连接缓冲与分帧](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/04-Connections%20Buffers%20and%20Framing%20(连接缓冲与分帧).md)：实现增量解析与输入输出缓冲，处理半包和部分写。
- [ ] [定时器](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/03-Timers%20(定时器).md)：实现堆式定时器，验证取消、到期与精度。
- [ ] [优雅关闭与请求排空](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/06-Graceful%20Shutdown%20and%20Drain%20(优雅关闭与请求排空).md)：实现停止接入、排空请求和超时退出。
- [ ] [压测](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/05-Benchmarking%20(压测).md)：压测已有服务，解释吞吐、延迟分位与资源瓶颈。
- [ ] [系统与网络标准与自测](/06-Systems%20and%20Networking%20(系统与网络)/00-Systems%20and%20Networking%20Standard%20(系统与网络标准与自测).md)：串起连接、处理、定位与关闭流程，检查理解缺口。

**接口、异步与可观测性**

- [ ] [HTTP自动化与接口检查](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/01-HTTP%20Automation%20and%20API%20Checks%20(HTTP自动化与接口检查).md)：为已理解的 HTTP 接口检查正常、错误和超时响应。
- [ ] [异步 I-O 与并发](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/04-Async%20IO%20and%20Concurrency%20(异步%20I-O%20与并发).md)：实现有界异步请求，验证取消、超时与任务退出。
- [ ] [可观测性、日志、指标与追踪](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)：为请求关联日志、指标与追踪，选择必要字段。
- [ ] [HTTP 服务设计](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/04-HTTP%20Service%20Design%20(HTTP%20服务设计).md) + [可观测性与性能](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/05-Observability%20and%20Performance%20(可观测性与性能).md)：为小型 HTTP 服务定义接口、错误语义与延迟指标。
- [ ] [后端架构模式](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/07-Backend%20Architecture%20Patterns%20(后端架构模式).md)：画出小服务的模块与部署关系，说明故障边界。

**项目设计与质量判断**

- [ ] [资源生命周期](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/01-Resource%20Lifetime%20(资源生命周期).md) + [并发模型](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/02-Concurrency%20Models%20(并发模型).md) + [错误处理](/11-Projects%20(项目实践)/03-Engineering%20Decision%20Records%20(工程决策记录)/03-Error%20Handling%20(错误处理).md)：梳理一个请求的资源、并发与错误传播边界。
- [ ] [项目设计模板](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/01-Project%20Design%20Template%20(项目设计模板).md) + [需求架构与里程碑](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/03-Requirements%20Architecture%20and%20Milestones%20(需求架构与里程碑).md)：为一个小服务明确需求、模块、非目标与里程碑。
- [ ] [测试压测与证据](/11-Projects%20(项目实践)/01-Project%20Method%20(项目方法)/02-Testing%20Benchmark%20and%20Evidence%20(测试压测与证据).md)：设计功能测试与基准负载，解释指标和环境影响。
- [ ] [发布验证与质量门禁](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/07-Release%20Verification%20and%20Quality%20Gates%20(发布验证与质量门禁).md)：为已有服务设置发布检查、失败判定与回滚条件。
- [ ] [工程基础标准与自测](/02-Engineering%20Fundamentals%20(工程基础)/00-Engineering%20Fundamentals%20Standard%20(工程基础标准与自测).md)：检查构建、测试与诊断流程，补齐一个实际缺口。

# 阶段七 · 数据存储与分布式机制

先掌握单机数据正确性，再进入缓存、消息与多节点一致性。

**SQL、索引与事务**

- [ ] [MySQL 环境与元数据](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/03-MySQL%20Environment%20and%20Metadata%20(MySQL%20环境与元数据).md)：启动实例，确认版本、字符集、权限与元数据。
- [ ] [SQL 定义与数据操作](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/01-SQL%20Definition%20and%20Manipulation%20(SQL%20定义与数据操作).md)：创建数据库和表，执行增删改并验证约束、字符集与事务影响。
- [ ] [SQL 查询连接与聚合](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/02-SQL%20Queries%20Joins%20and%20Aggregation%20(SQL%20查询连接与聚合).md)：用连接、过滤、分组和分页完成查询，检查重复行与 NULL 语义。
- [ ] [InnoDB 存储与索引](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/04-InnoDB%20Storage%20and%20Indexes%20(InnoDB%20存储与索引).md)：串起页、缓冲池与 B+ 树，理解聚簇索引。
- [ ] [事务与隔离级别](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/05-Transactions%20and%20Isolation%20(事务与隔离级别).md)：用两个会话观察隔离差异，解释并发读写现象。
- [ ] [MVCC 与 Read View](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/06-MVCC%20and%20Read%20Views%20(MVCC%20与%20Read%20View).md)：手推版本可见性，比较 RC 与 RR 的快照时机。

**锁、恢复与查询效率**

- [ ] [锁与死锁](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/07-Locks%20and%20Deadlocks%20(锁与死锁).md)：复现锁等待或死锁，观察索引与隔离级别的影响。
- [ ] [日志与恢复](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/08-Logging%20and%20Recovery%20(日志与恢复).md)：区分 redo、undo 与 binlog，推演提交和恢复。
- [ ] [查询分析与优化](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/09-Query%20Analysis%20and%20Optimization%20(查询分析与优化).md)：分析一个执行计划，验证索引或 SQL 改动。
- [ ] [连接池](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/10-Connection%20Pool%20(连接池).md)：配置池大小与超时，验证连接耗尽和释放。
- [ ] [复制](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/11-Replication%20(复制).md)：比较复制确认与延迟，解释故障切换代价。
- [ ] [分库分表](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/12-Sharding%20and%20Partitioning%20(分库分表).md)：选择分片键，推演跨分片查询与扩容成本。

**缓存、客户端与流量入口**

- [ ] [Redis 数据结构](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/01-Redis%20Data%20Structures%20(Redis%20数据结构).md)：按访问模式选择 Redis 结构，比较操作成本。
- [ ] [Redis 运行时与客户端](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/04-Redis%20Runtime%20and%20Client%20(Redis%20运行时与客户端).md)：接入客户端，观察事件循环与请求延迟。
- [ ] [Redis 持久化与淘汰](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/02-Redis%20Persistence%20and%20Eviction%20(Redis%20持久化与淘汰).md)：比较 RDB/AOF，验证过期与淘汰行为。
- [ ] [缓存一致性问题](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/03-Cache%20Consistency%20Problems%20(缓存一致性问题).md)：区分穿透、击穿与雪崩，设计对应防护。
- [ ] [MySQL 与 Redis 缓存](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/13-MySQL%20and%20Redis%20Caching%20(MySQL%20与%20Redis%20缓存).md)：实现 Cache-Aside，推演失效与旧值回填。
- [ ] [Nginx 代理与负载均衡](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/05-Nginx%20Proxy%20and%20Load%20Balancing%20(Nginx%20代理与负载均衡).md)：配置反向代理，验证路由、超时与负载分配。

**消息、共识与事务协调**

- [ ] [消息队列](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/03-Messaging%20and%20Streaming%20(消息与流)/01-Message%20Queues%20(消息队列).md) + [RabbitMQ 与 Kafka 选型](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/03-Messaging%20and%20Streaming%20(消息与流)/03-RabbitMQ%20and%20Kafka%20Selection%20(RabbitMQ%20与%20Kafka%20选型).md)：理解消息生产与消费，再按路由和顺序需求比较选型。
- [ ] [Kafka 投递与幂等](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/03-Messaging%20and%20Streaming%20(消息与流)/02-Kafka%20Delivery%20and%20Idempotency%20(Kafka%20投递与幂等).md)：推演重复投递与重平衡，验证消费幂等。
- [ ] [一致性与 CAP](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/01-Consistency%20and%20CAP%20(一致性与%20CAP).md)：用分区场景讨论一致性与可用性的取舍。
- [ ] [Raft 共识](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/02-Raft%20Consensus%20(Raft%20共识).md)：手推选举与日志复制，解释提交条件。
- [ ] [Paxos 共识](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/03-Paxos%20(Paxos).md)：手推一次提案，解释多数派交集与活锁。
- [ ] [分布式事务](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/04-Distributed%20Transactions%20(分布式事务).md)：推演两阶段提交与故障，比较补偿方案。
- [ ] [数据系统与分布式标准与自测](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/00-Data%20Systems%20and%20Distributed%20Computing%20Standard%20(数据系统与分布式标准与自测).md)：用事务、缓存和消息场景检查一致性与恢复理解。

# 阶段八 · Go 服务与多语言协作

已有系统、网络与数据库基础后，再学习 Go 的类型、并发和服务交付。

**语言、模块与资源安全**

- [ ] [模块包与工具链](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/04-Modules%20Packages%20and%20Tooling%20(模块包与工具链).md)：创建 module，运行、测试并整理包依赖。
- [ ] [语法、值、结构体与方法](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/01-Syntax%20Values%20Structs%20and%20Methods%20(语法、值、结构体与方法).md)：运行小程序，掌握值、结构体与方法接收者。
- [ ] [切片、映射、字符串与内存](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/02-Slices%20Maps%20Strings%20and%20Memory%20(切片、映射、字符串与内存).md)：验证切片共享、映射行为与字符串编码。
- [ ] [接口、嵌入与泛型](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/03-Interfaces%20Embedding%20and%20Generics%20(接口、嵌入与泛型).md)：用接口解耦实现，理解嵌入与泛型约束。
- [ ] [错误、panic、defer 与资源安全](/05-Go%20Backend%20(Go%20后端)/01-Go%20Foundations%20(Go%20基础)/05-Errors%20Panic%20Defer%20and%20Resource%20Safety%20(错误、panic、defer%20与资源安全).md)：用错误返回和 defer 管理失败路径，理解 panic 边界。

**并发与请求生命周期**

- [ ] [并发原语](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/01-Goroutines%20Channels%20and%20sync%20(并发原语).md)：实现有界并发，比较 channel 与锁的用途。
- [ ] [取消传播与错误](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/02-Context%20Cancellation%20and%20Errors%20(取消传播与错误).md)：让取消沿请求树传播，检查任务退出路径。
- [ ] [调度 GC 与内存](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/03-Scheduler%20GC%20and%20Memory%20(调度%20GC%20与内存).md)：观察分配与 GC，理解调度和内存成本。
- [ ] [HTTP JSON与中间件](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/01-HTTP%20JSON%20and%20Middleware%20(HTTP%20JSON与中间件).md)：实现 JSON 接口与中间件，处理上下文和错误。
- [ ] [路由校验与错误](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/02-Routing%20Validation%20and%20Errors%20(路由校验与错误).md) + [服务生命周期](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/03-API%20Lifecycle%20and%20Graceful%20Shutdown%20(服务生命周期).md)：补输入校验与服务关闭，验证错误响应和排空。
- [ ] [测试与质量](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/04-Testing%20and%20Quality%20(测试与质量).md)：编写表驱动与集成测试，运行竞态检查。

**运行诊断与数据集成**

- [ ] [可观测性与运行](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/05-Observability%20and%20Operations%20(可观测性与运行).md)：为已有服务接入日志和指标，定位一次请求故障。
- [ ] [RPC、Protobuf 与接口演进](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/06-RPC%20Protobuf%20and%20API%20Evolution%20(RPC、Protobuf%20与接口演进).md)：实现最小 RPC，检查契约演进和超时传播。
- [ ] [SQL 访问与事务](/05-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/01-SQL%20Access%20and%20Transactions%20(SQL%20访问与事务).md)：实现事务写入，验证回滚和连接释放。
- [ ] [Redis 缓存模式](/05-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/02-Redis%20Cache%20Patterns%20(Redis%20缓存模式).md)：接入缓存，处理过期、空值与连接失败。
- [ ] [消息与幂等](/05-Go%20Backend%20(Go%20后端)/04-Data%20Integration%20(数据集成)/03-Messaging%20and%20Idempotency%20(消息与幂等).md)：实现幂等消费，验证重复投递和失败重试。

**实践交付与语言分工**

- [ ] [Go 服务实践流程](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/01-Go%20Service%20Practice%20Workflow%20(Go%20服务实践流程).md)：检查服务入口、测试与运行配置，串起交付流程。
- [ ] [首个服务交付](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/03-First%20Service%20Delivery%20(首个服务交付).md)：交付已有小服务，补齐启动、测试与限制说明。
- [ ] [C++ 到 Go 迁移决策](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/02-C++%20to%20Go%20Decision%20Notes%20(C++%20到%20Go%20迁移决策).md) + [多语言协作](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/03-C++%20Go%20Python%20Collaboration%20(多语言协作).md)：比较三种语言的职责，明确接口、所有权与错误契约。
- [ ] [Python 工程标准与自测](/04-Python%20Engineering%20(Python%20工程)/00-Python%20Engineering%20Standard%20(Python%20工程标准与自测).md)：检查 Python 环境、测试与自动化能力。
- [ ] [Go 后端标准与自测](/05-Go%20Backend%20(Go%20后端)/00-Go%20Backend%20Standard%20(Go%20后端标准与自测).md)：检查 Go 接口、并发退出与数据访问能力。

# 阶段九 · 安全边界与主项目实现

案例先用于理解设计；选一个主项目逐步实现，其余案例保留为方案比较。

**应用安全与项目边界**

- [ ] [威胁建模、认证与会话](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/01-Threat%20Modeling%20Authentication%20and%20Sessions%20(威胁建模、认证与会话).md)：为接口画信任边界，理解认证与会话生命周期。
- [ ] [授权与 API 安全](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/02-Authorization%20and%20API%20Security%20(授权与%20API%20安全).md)：为资源访问设置授权，验证越权与输入攻击。
- [ ] [安全编码与内存安全](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/03-Secure%20Coding%20and%20Memory%20Safety%20(安全编码与内存安全).md)：审查输入和资源操作，定位一类安全缺陷。
- [ ] [密钥、密码学与供应链](/09-Security%20Engineering%20(安全工程)/02-Identity%20Cloud%20and%20Supply%20Chain%20(身份、云与供应链)/01-Secrets%20Cryptography%20and%20Supply%20Chain%20(密钥、密码学与供应链).md)：理解密钥与依赖来源管理，演练一次轮换。
- [ ] [短链接服务](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/01-URL%20Shortener%20(短链接服务).md)：读懂短码与跳转设计，验证一种短码冲突处理。
- [ ] [即时通讯与通知](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/02-IM%20and%20Notification%20(即时通讯与通知).md)：梳理在线与离线投递，推演重复消息处理。
- [ ] [秒杀项目](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/03-Flash%20Sale%20Project%20(秒杀项目).md)：推演库存扣减与异步下单，验证一个并发边界。
- [ ] [配置中心项目](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/04-Config%20Center%20Project%20(配置中心项目).md)：读懂配置版本与推送流程，推演一次灰度回滚。

**需求、接口与纵向实现**

- [ ] 选择主项目：写清核心场景、非目标和验收条件。
- [ ] 拆分实现路径：确定最小可演示功能，列出依赖和风险。
- [ ] 定义数据与接口：标出事务、幂等和权限边界。
- [ ] 建立项目骨架：配置启动、测试与本地依赖。
- [ ] 打通纵向切片：实现请求、业务、持久化与响应。
- [ ] 补齐异常路径：处理输入错误、资源释放与失败响应。

**并发、可靠性与交付**

- [ ] 验证数据正确性：实现必要的索引、事务与并发控制。
- [ ] 处理重复与部分失败：验证幂等、超时和重试边界。
- [ ] 审查组件与资源：确认缓存或队列的用途，设置资源上限。
- [ ] 落实安全约束：检查身份授权、秘密管理和日志脱敏。
- [ ] 验证故障恢复：演练依赖失败、任务退出与优雅关闭。
- [ ] 补齐关键测试：覆盖正常、边界和失败路径。
- [ ] 建立性能基线：记录负载、环境、吞吐和延迟分位。
- [ ] 分析一个瓶颈：验证改动收益，说明代价和容量边界。
- [ ] 整理交付材料：让他人按 README 启动、测试和演示。
- [ ] 记录设计取舍：保留一个故障案例、已知限制与后续方向。
- [ ] [项目标准与自测](/11-Projects%20(项目实践)/00-Project%20Standard%20(项目标准与自测).md)：按清单检查主项目的运行、测试与演示证据。

# 阶段十 · 云原生运行与安全治理

将已有服务放入容器与集群，再补齐可靠性、平台与安全响应能力。

**容器、集群与服务可靠性**

- [ ] [容器镜像与运行时](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/01-Container%20Images%20and%20Runtime%20(容器镜像与运行时).md)：构建并运行镜像，检查用户、信号和资源限制。
- [ ] [Kubernetes 工作负载与网络](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/02-Kubernetes%20Workloads%20and%20Networking%20(Kubernetes%20工作负载与网络).md)：部署工作负载与服务，观察副本和服务发现。
- [ ] [配置、探针与发布](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/03-Configuration%20Probes%20and%20Rollouts%20(配置、探针与发布).md)：配置探针与滚动发布，演练失败和回滚。
- [ ] [集群调度、资源与存储](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/04-Cluster%20Scheduling%20Resources%20and%20Storage%20(集群调度、资源与存储).md)：设置资源与存储，观察调度约束。
- [ ] [Kubernetes 排障与生产运维](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/01-Containers%20and%20Kubernetes%20(容器与%20Kubernetes)/05-Kubernetes%20Troubleshooting%20and%20Production%20Operations%20(Kubernetes%20排障与生产运维).md)：从事件和日志定位 Pod 故障，验证恢复步骤。
- [ ] [SLO、错误预算与告警](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/03-Reliability%20Engineering%20(可靠性工程)/01-SLO%20Error%20Budgets%20and%20Alerting%20(SLO、错误预算与告警).md) + [容量、自动扩缩与成本](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/03-Reliability%20Engineering%20(可靠性工程)/02-Capacity%20Autoscaling%20and%20Cost%20(容量、自动扩缩与成本).md)：定义服务目标，再估算容量与扩缩成本。
- [ ] [事故响应、混沌与恢复](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/03-Reliability%20Engineering%20(可靠性工程)/03-Incident%20Response%20Chaos%20and%20Recovery%20(事故响应、混沌与恢复).md)：为已有服务设计一次可控故障演练与恢复。

**平台交付与环境治理**

- [ ] [基础设施即代码与 GitOps](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/02-Platform%20Engineering%20(平台工程)/01-Infrastructure%20as%20Code%20and%20GitOps%20(基础设施即代码与%20GitOps).md)：用声明式配置描述环境，观察同步与回滚。
- [ ] [策略、配置与多环境](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/02-Platform%20Engineering%20(平台工程)/03-Policy%20Configuration%20and%20Environments%20(策略、配置与多环境).md)：区分环境配置与密钥，验证变更和漂移检查。
- [ ] [开发者平台与黄金路径](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/02-Platform%20Engineering%20(平台工程)/02-Developer%20Platforms%20and%20Golden%20Paths%20(开发者平台与黄金路径).md)：设计可复用的服务模板，明确默认配置与限制。
- [ ] [云身份与容器安全](/09-Security%20Engineering%20(安全工程)/02-Identity%20Cloud%20and%20Supply%20Chain%20(身份、云与供应链)/02-Cloud%20Identity%20and%20Container%20Security%20(云身份与容器安全).md)：限制服务身份权限，检查容器与凭据暴露面。
- [ ] [云原生与可靠性标准与自测](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/00-Cloud%20Native%20and%20Reliability%20Standard%20(云原生与可靠性标准与自测).md)：检查部署、容量、监控与恢复链路。

**安全验证、隔离与响应**

- [ ] [安全需求与验证](/09-Security%20Engineering%20(安全工程)/01-Application%20Security%20(应用安全)/04-Security%20Requirements%20and%20Verification%20(安全需求与验证).md) + [安全测试与漏洞管理](/09-Security%20Engineering%20(安全工程)/02-Identity%20Cloud%20and%20Supply%20Chain%20(身份、云与供应链)/03-Security%20Testing%20and%20Vulnerability%20Management%20(安全测试与漏洞管理).md)：把安全需求转成测试，检查依赖并验证修复。
- [ ] [沙箱与不可信执行](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/01-Sandboxing%20and%20Untrusted%20Execution%20(沙箱与不可信执行).md)：设计不可信执行的权限和资源边界。
- [ ] [安全日志与检测](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/02-Security%20Logging%20and%20Detection%20(安全日志与检测).md)：定义关键安全事件，串起告警与调查。
- [ ] [安全检测与事件证据](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/06-Security%20Detection%20and%20Incident%20Evidence%20(安全检测与事件证据).md)：从安全告警追到原始事件，保留调查依据。
- [ ] [安全事件响应与恢复](/09-Security%20Engineering%20(安全工程)/03-Isolation%20Detection%20and%20Response%20(隔离、检测与响应)/03-Incident%20Response%20and%20Recovery%20(安全事件响应与恢复).md)：推演事件遏制、恢复与证据保留。
- [ ] [安全工程标准与自测](/09-Security%20Engineering%20(安全工程)/00-Security%20Engineering%20Standard%20(安全工程标准与自测).md)：检查认证、授权、编码与响应能力。

# 阶段十一 · AI 应用与运行工程

从模型调用和效果评估进入检索与工具工作流，再理解推理服务的资源与安全边界。

**模型、评估与检索应用**

- [ ] [Token、向量与模型行为](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/01-Tokens%20Embeddings%20and%20Model%20Behavior%20(Token、向量与模型行为).md)：理解 Token、向量与上下文，观察输入变化。
- [ ] [训练、推理与模型生命周期](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/02-Training%20Inference%20and%20Model%20Lifecycle%20(训练、推理与模型生命周期).md)：区分训练、评估与推理，串起模型生命周期。
- [ ] [模型 API 与结构化输出](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/03-Model%20APIs%20and%20Structured%20Outputs%20(模型%20API%20与结构化输出).md)：调用模型并校验结构，处理限额、超时与错误。
- [ ] [评估数据与实验设计](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/01-AI%20Foundations%20(AI%20基础)/04-Evaluation%20Data%20and%20Experiment%20Design%20(评估数据与实验设计).md)：建立小型评估集，对比配置并分析偏差。
- [ ] [提示、上下文与工具调用](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/02-LLM%20Applications%20and%20Agents%20(大模型应用与智能体)/01-Prompt%20Context%20and%20Tool%20Calling%20(提示、上下文与工具调用).md)：构造上下文并调用受限工具，校验参数和返回值。
- [ ] [RAG 与向量检索](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/02-LLM%20Applications%20and%20Agents%20(大模型应用与智能体)/02-RAG%20and%20Vector%20Retrieval%20(RAG%20与向量检索).md)：搭建最小检索问答，分别检查召回与答案依据。

**工具工作流与推理服务**

- [ ] [Agent 工作流、评估与安全](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/02-LLM%20Applications%20and%20Agents%20(大模型应用与智能体)/03-Agent%20Workflow%20Evaluation%20and%20Safety%20(Agent%20工作流、评估与安全).md)：运行有限步骤的工具流程，检查权限与停止条件。
- [ ] [推理服务、批处理与缓存](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/03-AI%20Infrastructure%20(AI%20基础设施)/01-Inference%20Serving%20Batching%20and%20Caching%20(推理服务、批处理与缓存).md)：比较批处理和缓存对延迟、吞吐的影响。
- [ ] [GPU 调度、容器与多租户](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/03-AI%20Infrastructure%20(AI%20基础设施)/02-GPU%20Scheduling%20Containers%20and%20Multi%20Tenancy%20(GPU%20调度、容器与多租户).md)：估算显存与并发，理解隔离和调度限制。
- [ ] [AI 可观测性、成本与安全](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/03-AI%20Infrastructure%20(AI%20基础设施)/03-AI%20Observability%20Cost%20and%20Security%20(AI%20可观测性、成本与安全).md)：记录请求质量、延迟与成本，检查安全事件。
- [ ] [安全 AI Agent 平台](/11-Projects%20(项目实践)/02-Backend%20Projects%20(后端项目)/05-Secure%20AI%20Agent%20Platform%20(安全%20AI%20Agent%20平台).md)：梳理平台设计，在现有原型中验证一个隔离或审计环节。
- [ ] [AI 系统工程标准与自测](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/00-AI%20Systems%20Engineering%20Standard%20(AI%20系统工程标准与自测).md)：检查模型调用、检索、评估与运行边界。

# 阶段十二 · 系统设计与技术表达

以已完成的代码和实验支撑设计与表达，逐步形成可核验的能力证据。

**系统设计与方案比较**

- [ ] [短链接系统](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/01-URL%20Shortener%20(短链接系统).md)：推导短码、缓存与容量，比较实现取舍。
- [ ] [即时通信系统](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/02-IM%20System%20(即时通信系统).md)：推演消息顺序、离线补偿与重复投递。
- [ ] [秒杀系统设计](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/03-Flash%20Sale%20System%20Design%20(秒杀系统设计).md)：从流量推导限流、库存与异步化方案。
- [ ] [配置中心系统设计](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/04-Config%20Center%20System%20Design%20(配置中心系统设计).md)：推演配置发布、灰度与回滚的一致性。
- [ ] [API 网关](/12-Career%20(求职与成长)/02-System%20Design%20(系统设计)/05-API%20Gateway%20(API%20网关).md)：设计路由与过滤链，解释限流和故障隔离。

**基础表达与项目证据**

- [ ] [C++ 后端面试高频 100 题](/12-Career%20(求职与成长)/01-Interview%20Expression%20(面试表达)/01-C++%20Backend%20Interview%20Top%20100%20(C++%20后端面试高频%20100%20题).md)：先选对象与并发主题口述，按机制和边界定位缺口。
- [ ] [数据速记](/12-Career%20(求职与成长)/01-Interview%20Expression%20(面试表达)/02-Data%20Quick%20Reference%20(数据速记).md)：推导常用复杂度与数量级，标明成立条件。
- [ ] [项目难点表达](/12-Career%20(求职与成长)/01-Interview%20Expression%20(面试表达)/03-Project%20Deep%20Dive%20(项目难点表达).md)：选一个真实难点，组织取舍、证据与限制。
- [ ] [后端能力证据](/12-Career%20(求职与成长)/03-Career%20Execution%20(求职执行)/01-Backend%20Competency%20Evidence%20(后端能力证据).md) + [简历证据与作品集](/12-Career%20(求职与成长)/03-Career%20Execution%20(求职执行)/02-Resume%20Evidence%20and%20Portfolio%20(简历证据与作品集).md)：把个人贡献关联到代码、测试与可核验结果。
- [ ] [面试训练与复盘](/12-Career%20(求职与成长)/03-Career%20Execution%20(求职执行)/03-Interview%20Practice%20and%20Retrospective%20(面试训练与复盘).md) + [求职能力标准与自测](/12-Career%20(求职与成长)/00-Career%20Standard%20(求职能力标准与自测).md)：做一次模拟问答，按岗位标准定位补强方向。

**综合演练与求职准备**

- [ ] 画出主项目全景：串起请求、数据与依赖，标出容量假设。
- [ ] 比较关键方案：说明替代设计、失败模式和回滚边界。
- [ ] 演练系统设计：依次完成需求澄清、容量估算与故障分析。
- [ ] 演练项目深挖：结合代码追问事务、并发、安全和性能。
- [ ] 核对作品与简历：复现运行结果，删去无证据的表述。
- [ ] 对照目标岗位：整理仍需补强的主题，回到对应笔记。

# 阶段十三 · 核心主题专攻

在第一轮学习和项目之后深化重点；也可在完成对应前置后提前进入。每项聚焦一个实现或故障问题。

**对象、所有权与并发正确性**

- [ ] [对象生命周期与拷贝控制](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/03-Object%20Lifetime%20and%20Copy%20Control%20(对象生命周期与拷贝控制).md)：设计资源类型，验证拷贝、移动与析构的不变量。
- [ ] [值类别与移动语义](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)：比较有无移动的调用路径，解释生命周期和异常保证。
- [ ] [RAII 与自定义分配](/03-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)：注入构造失败，验证资源恰好释放一次。
- [ ] [智能指针](/03-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Smart%20Pointers%20(智能指针).md)：画出所有权图，检查共享与循环引用。
- [ ] [原子与内存序](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/02-Atomics%20and%20Memory%20Order%20(原子与内存序).md)：推导 happens-before，判断哪些操作需要同步。
- [ ] [线程池](/03-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/03-Thread%20Pool%20(线程池).md)：测试提交与关闭竞态，验证排空和线程回收。

**服务边界与数据一致性**

- [ ] [epoll API 与触发模式](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/09-epoll%20API%20and%20Trigger%20Modes%20(epoll%20API%20与触发模式).md)：构造 LT/ET 场景，验证何时必须读写到 EAGAIN。
- [ ] [TCP 流式分帧](/06-Systems%20and%20Networking%20(系统与网络)/02-Network%20Protocols%20(网络协议)/04-TCP%20Stream%20Framing%20(TCP%20流式分帧).md)：用分段、合并和非法长度测试分帧解析器。
- [ ] [连接缓冲与分帧](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/04-Connections%20Buffers%20and%20Framing%20(连接缓冲与分帧).md)：加入缓冲上限与水位，处理慢客户端和部分写。
- [ ] [MVCC 与 Read View](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/06-MVCC%20and%20Read%20Views%20(MVCC%20与%20Read%20View).md)：推演并发事务，解释 RC/RR 的快照可见性。
- [ ] [缓存一致性问题](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/03-Cache%20Consistency%20Problems%20(缓存一致性问题).md)：分析旧值回填与删除失败，明确一致性窗口。
- [ ] [并发原语](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/01-Goroutines%20Channels%20and%20sync%20(并发原语).md)：检查并发流程全部退出路径，定位泄漏和无界排队。
- [ ] [取消传播与错误](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/02-Context%20Cancellation%20and%20Errors%20(取消传播与错误).md)：让取消贯穿服务调用，验证下游退出与错误语义。

# 后续调整

遇到前置缺口就回到对应笔记；已掌握的内容通过验证后可直接勾选。让实际理解和项目反馈决定速度，保留自主复习与深入专攻的空间。
