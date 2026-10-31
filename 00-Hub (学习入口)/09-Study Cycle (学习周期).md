---
study_stage: reference
tags: [topic/learning]
---

> [!abstract] 学习计划
> C++ 建立资源与系统能力，Python 用于自动化验证，Go 用于服务交付。按任务产物验收。

# 执行规则

- 先按[间隔复习系统](<./02-Spaced Review System (间隔复习系统).md>)处理到期复习。学习笔记后先独立完成结尾的动手题，再口述机制追问；用代码、测试或可复现记录核对答案。任务可跨多天，完成验证后勾选。
- 按 1–5 顺序推进，算法穿插练习；6 按项目或岗位选择，7 从首个服务起积累证据。笔记状态仍由 `study_stage` 和 `review_due` 记录。

# 1. 基础程序与测试

验收：C++ 和 Python 程序均可在干净目录运行、测试和排障。

- [ ] [计算机系统总览](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/01-Computer System Overview (计算机系统总览).md>) + [数据表示](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/02-Data Representation (数据表示).md>)：画出源码到进程的路径，用整数或浮点边界验证一次表示差异。
- [ ] [核心语法与函数](<../03-C++ Backend (C++ 后端)/01-Language Basics (语言基础)/01-Core Syntax and Functions (核心语法与函数).md>) + [数组、字符串与输入输出](<../03-C++ Backend (C++ 后端)/01-Language Basics (语言基础)/02-Arrays Strings and IO (数组字符串与输入输出).md>)：做一个有输入、输出和错误分支的 C++ 命令行程序。
- [ ] [运行环境、模块与包管理](<../04-Python Engineering (Python 工程)/01-Python Foundations (Python 基础)/01-Runtime Modules Packages and uv (运行环境、模块与包管理).md>) + [核心语法、容器与推导式](<../04-Python Engineering (Python 工程)/01-Python Foundations (Python 基础)/03-Core Syntax Collections and Comprehensions (核心语法、容器与推导式).md>)：建立 Python 项目，用脚本处理一组结构化数据。
- [ ] [对象、名称、可变性与复制](<../04-Python Engineering (Python 工程)/01-Python Foundations (Python 基础)/02-Objects Names Mutability and Copying (对象、名称、可变性与复制).md>) + [函数、作用域、闭包与装饰器](<../04-Python Engineering (Python 工程)/01-Python Foundations (Python 基础)/04-Functions Scope Closures and Decorators (函数、作用域、闭包与装饰器).md>)：用短实验解释参数与对象别名；装饰器只做一个可测试示例。
- [ ] [Shell 命令与管道](<../06-Systems and Networking (系统与网络)/01-Linux Runtime (Linux 运行时)/01-Shell Commands and Pipelines (Shell 命令与管道).md>) + [文件权限与环境](<../06-Systems and Networking (系统与网络)/01-Linux Runtime (Linux 运行时)/02-Files Permissions and Environment (文件权限与环境).md>)：在另一目录重跑程序，记录路径、权限和环境差异。
- [ ] [Git 模型与分支](<../02-Engineering Fundamentals (工程基础)/01-Git and Delivery (Git 与交付)/01-Git Model and Branches (Git 模型与分支).md>) + [问题求解与调试](<../01-Foundations (基础能力)/04-Foundation Practice (基础实践)/01-Problem Solving and Debugging (问题求解与调试).md>)：提交小程序，制造并定位一个可复现错误。
- [ ] [测试与验证](<../01-Foundations (基础能力)/04-Foundation Practice (基础实践)/02-Testing and Verification (测试与验证).md>) + [pytest 夹具与测试替身](<../04-Python Engineering (Python 工程)/02-Quality and Automation (质量与自动化)/01-pytest Fixtures and Test Doubles (测试夹具与替身).md>)：给两个小程序各覆盖正常、边界和失败输入。
- [ ] **交付检查**：留下运行说明、测试命令与一条故障记录；让自己从空目录按说明复现。

# 2. C++ 资源与构建

验收：组件可构建、可测试，所有权和失败路径清楚。

- [ ] [类型、指针与引用](<../03-C++ Backend (C++ 后端)/02-Object and Resource Model (对象与资源模型)/01-Types Pointers and References (类型指针与引用).md>) + [内存布局与分配](<../03-C++ Backend (C++ 后端)/02-Object and Resource Model (对象与资源模型)/02-Memory Layout and Allocation (内存布局与分配).md>)：画出对象、存储期和引用关系，构造一个悬垂或越界反例。
- [ ] [对象生命周期与拷贝控制](<../03-C++ Backend (C++ 后端)/02-Object and Resource Model (对象与资源模型)/03-Object Lifetime and Copy Control (对象生命周期与拷贝控制).md>) + [RAII 与自定义分配](<../03-C++ Backend (C++ 后端)/02-Object and Resource Model (对象与资源模型)/06-RAII and Custom Allocation (RAII 与自定义分配).md>)：封装一种资源，验证构造、复制限制与析构责任；自定义分配先理解用途。
- [ ] [值类别与移动语义](<../03-C++ Backend (C++ 后端)/03-Modern C++ (现代 C++)/02-Value Categories and Move (值类别与移动语义).md>) + [智能指针](<../03-C++ Backend (C++ 后端)/03-Modern C++ (现代 C++)/03-Smart Pointers (智能指针).md>)：记录复制和移动调用，画出独占、共享和弱观察的所有权图。
- [ ] [头文件、命名空间与分离编译](<../03-C++ Backend (C++ 后端)/01-Language Basics (语言基础)/03-Headers Namespaces and Separate Compilation (头文件命名空间与分离编译).md>) + [程序构建与执行](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/03-Program Build and Execution (程序构建与执行).md>)：把组件拆成多文件，说明编译、链接和装载边界。
- [ ] [CMake 项目与目标](<../02-Engineering Fundamentals (工程基础)/02-Build and Dependencies (构建与依赖)/01-CMake Project and Targets (CMake 项目与目标).md>) + [CMake 依赖](<../02-Engineering Fundamentals (工程基础)/02-Build and Dependencies (构建与依赖)/02-CMake Dependencies (CMake 依赖).md>)：组织库、程序和测试目标，做一次干净构建。
- [ ] [现代 C++ 基础](<../03-C++ Backend (C++ 后端)/03-Modern C++ (现代 C++)/01-Modern C++ Foundations (现代 C++ 基础).md>) + [顺序容器](<../03-C++ Backend (C++ 后端)/04-STL and Data Structures (STL 与数据结构)/01-Sequence Containers (顺序容器).md>) + [关联容器](<../03-C++ Backend (C++ 后端)/04-STL and Data Structures (STL 与数据结构)/02-Associative Containers (关联容器).md>)：按接口和数据布局选容器，检查迭代器失效。
- [ ] [异常与调试](<../03-C++ Backend (C++ 后端)/06-Engineering Practice (工程实践)/03-Exceptions and Debugging (异常与调试).md>) + [内存安全与动态分析](<../02-Engineering Fundamentals (工程基础)/03-Verification and Diagnostics (验证与诊断)/03-Memory Safety and Dynamic Analysis (内存安全与动态分析).md>)：注入一次失败，用 Sanitizer 或调试器证明资源与内存行为。
- [ ] **交付检查**：发布可构建组件、测试和所有权说明；能指出一种被测试拦截的错误。

# 3. HTTP 小服务

验收：服务能响应请求，并通过接口、错误和关闭测试。

- [ ] [操作系统总览与启动](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/05-Operating System Overview and Boot (操作系统总览与启动).md>) + [进程与线程](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/06-Processes and Threads (进程与线程).md>)：观察程序如何启动、阻塞和退出，区分进程与线程的共享边界。
- [ ] [虚拟内存、分页与分配](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/08-Virtual Memory Paging and Allocation (虚拟内存、分页与分配).md>) + [文件系统与存储](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/09-File Systems and Storage (文件系统与存储).md>)：观察地址空间、文件描述符和持久化边界。
- [ ] [TCP 与 IP 基础](<../06-Systems and Networking (系统与网络)/02-Network Protocols (网络协议)/01-TCP and IP Fundamentals (TCP 与 IP 基础).md>) + [TCP 握手与状态机](<../06-Systems and Networking (系统与网络)/02-Network Protocols (网络协议)/02-TCP Handshake and State Machine (TCP 握手与状态机).md>)：用抓包或命令观察连接建立、关闭和端口分用。
- [ ] [HTTP 协议](<../06-Systems and Networking (系统与网络)/02-Network Protocols (网络协议)/06-HTTP Protocol (HTTP 协议).md>) + [Socket 与事件循环](<../06-Systems and Networking (系统与网络)/03-Server Networking (服务器网络编程)/01-Sockets and Event Loop (Socket 与事件循环).md>)：先完成阻塞式请求响应，再解释非阻塞实现需要解决的问题。
- [ ] [TCP 流式分帧](<../06-Systems and Networking (系统与网络)/02-Network Protocols (网络协议)/04-TCP Stream Framing (TCP 流式分帧).md>) + [连接缓冲与分帧](<../06-Systems and Networking (系统与网络)/03-Server Networking (服务器网络编程)/04-Connections Buffers and Framing (连接缓冲与分帧).md>)：为服务或独立解析器测试分段、合并到达和非法长度。
- [ ] [HTTP自动化与接口检查](<../04-Python Engineering (Python 工程)/03-Backend Assistance (后端辅助)/01-HTTP Automation and API Checks (HTTP自动化与接口检查).md>) + [测试策略与证据](<../02-Engineering Fundamentals (工程基础)/03-Verification and Diagnostics (验证与诊断)/01-Testing Strategy and Evidence (测试策略与证据).md>)：用 Python 检查成功、错误和超时响应。
- [ ] [可观测性、日志、指标与追踪](<../02-Engineering Fundamentals (工程基础)/03-Verification and Diagnostics (验证与诊断)/05-Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪).md>) + [优雅关闭与请求排空](<../06-Systems and Networking (系统与网络)/03-Server Networking (服务器网络编程)/06-Graceful Shutdown and Drain (优雅关闭与请求排空).md>)：关联一次请求的日志，验证停止接入和退出。
- [ ] **交付检查**：小服务有启动命令、接口契约、自动化测试和一次可复现的关闭实验。

# 4. Go 数据服务

验收：Go API 能持久化数据、验证事务，并可靠关闭；首版只用一种存储。

- [ ] [MySQL 环境与元数据](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/03-MySQL Environment and Metadata (MySQL 环境与元数据).md>) + [SQL 定义与数据操作](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/01-SQL Definition and Manipulation (SQL 定义与数据操作).md>) + [SQL 查询连接与聚合](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/02-SQL Queries Joins and Aggregation (SQL 查询连接与聚合).md>)：建表、写查询并验证约束与空值。
- [ ] [InnoDB 存储与索引](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/04-InnoDB Storage and Indexes (InnoDB 存储与索引).md>) + [事务与隔离级别](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/05-Transactions and Isolation (事务与隔离级别).md>)：用两会话复现一个并发读写现象，说明索引与事务边界。
- [ ] [模块包与工具链](<../05-Go Backend (Go 后端)/01-Go Foundations (Go 基础)/04-Modules Packages and Tooling (模块包与工具链).md>) + [语法、值、结构体与方法](<../05-Go Backend (Go 后端)/01-Go Foundations (Go 基础)/01-Syntax Values Structs and Methods (语法、值、结构体与方法).md>)：创建 Go module，完成最小业务模型。
- [ ] [切片、映射、字符串与内存](<../05-Go Backend (Go 后端)/01-Go Foundations (Go 基础)/02-Slices Maps Strings and Memory (切片、映射、字符串与内存).md>) + [错误、panic、defer 与资源安全](<../05-Go Backend (Go 后端)/01-Go Foundations (Go 基础)/05-Errors Panic Defer and Resource Safety (错误、panic、defer 与资源安全).md>)：验证共享与错误传播，确保资源关闭。
- [ ] [接口、嵌入与泛型](<../05-Go Backend (Go 后端)/01-Go Foundations (Go 基础)/03-Interfaces Embedding and Generics (接口、嵌入与泛型).md>)：为真实依赖定义最小接口；泛型只验证一个必要用途。
- [ ] [HTTP JSON与中间件](<../05-Go Backend (Go 后端)/03-Web Services (Web 服务)/01-HTTP JSON and Middleware (HTTP JSON与中间件).md>) + [路由校验与错误](<../05-Go Backend (Go 后端)/03-Web Services (Web 服务)/02-Routing Validation and Errors (路由校验与错误).md>)：实现一个完整请求路径，覆盖非法输入与统一错误响应。
- [ ] [SQL 访问与事务](<../05-Go Backend (Go 后端)/04-Data Integration (数据集成)/01-SQL Access and Transactions (SQL 访问与事务).md>)：完成一次事务写入，验证回滚、超时和连接释放。
- [ ] [测试与质量](<../05-Go Backend (Go 后端)/03-Web Services (Web 服务)/04-Testing and Quality (测试与质量).md>) + [服务生命周期](<../05-Go Backend (Go 后端)/03-Web Services (Web 服务)/03-API Lifecycle and Graceful Shutdown (服务生命周期).md>)：做表驱动、接口和依赖测试，运行竞态检查并验证优雅关闭。
- [ ] [首个服务交付](<../05-Go Backend (Go 后端)/05-Go Practice (Go 实践)/03-First Service Delivery (首个服务交付).md>)：在干净环境按 README 启动、测试和调用 API，记录已知限制。

# 5. 主项目

验收：选一个项目，交付可复现的纵向切片、测试、性能与故障证据。

候选案例：[短链接服务](<../11-Projects (项目实践)/02-Backend Projects (后端项目)/01-URL Shortener (短链接服务).md>) · [即时通讯与通知](<../11-Projects (项目实践)/02-Backend Projects (后端项目)/02-IM and Notification (即时通讯与通知).md>) · [秒杀项目](<../11-Projects (项目实践)/02-Backend Projects (后端项目)/03-Flash Sale Project (秒杀项目).md>) · [配置中心项目](<../11-Projects (项目实践)/02-Backend Projects (后端项目)/04-Config Center Project (配置中心项目).md>)。

- [ ] **选题与边界**：阅读选定案例，结合[项目设计模板](<../11-Projects (项目实践)/01-Project Method (项目方法)/01-Project Design Template (项目设计模板).md>)和[需求架构](<../11-Projects (项目实践)/01-Project Method (项目方法)/03-Requirements Architecture and Milestones (需求架构与里程碑).md>)确定场景、非目标、容量假设与验收条件。
- [ ] [威胁建模、认证与会话](<../09-Security Engineering (安全工程)/01-Application Security (应用安全)/01-Threat Modeling Authentication and Sessions (威胁建模、认证与会话).md>) + [授权与 API 安全](<../09-Security Engineering (安全工程)/01-Application Security (应用安全)/02-Authorization and API Security (授权与 API 安全).md>)：为选定接口画信任边界，定义一条可测试的授权规则。
- [ ] **纵向切片**：打通请求、业务、持久化和响应；用一条自动化测试验证关键不变量。
- [ ] [错误处理](<../11-Projects (项目实践)/03-Engineering Decision Records (工程决策记录)/03-Error Handling (错误处理).md>) + [资源生命周期](<../11-Projects (项目实践)/03-Engineering Decision Records (工程决策记录)/01-Resource Lifetime (资源生命周期).md>) + [并发模型](<../11-Projects (项目实践)/03-Engineering Decision Records (工程决策记录)/02-Concurrency Models (并发模型).md>)：沿一次请求说明资源、并发、取消和错误传播。
- [ ] **首轮复盘**：画数据流，口述一个设计取舍，给出对应代码、测试或日志证据。
- [ ] [安全编码与内存安全](<../09-Security Engineering (安全工程)/01-Application Security (应用安全)/03-Secure Coding and Memory Safety (安全编码与内存安全).md>) + [密钥、密码学与供应链](<../09-Security Engineering (安全工程)/02-Identity Cloud and Supply Chain (身份、云与供应链)/01-Secrets Cryptography and Supply Chain (密钥、密码学与供应链).md>)：检查输入、凭据和敏感日志，验证一个失败路径。
- [ ] [测试压测与证据](<../11-Projects (项目实践)/01-Project Method (项目方法)/02-Testing Benchmark and Evidence (测试压测与证据).md>) + [性能基准与剖析](<../02-Engineering Fundamentals (工程基础)/03-Verification and Diagnostics (验证与诊断)/04-Performance Benchmarking and Profiling (性能基准与剖析).md>)：保存负载、环境、原始结果与延迟分位，定位一个瓶颈。
- [ ] [发布验证与质量门禁](<../02-Engineering Fundamentals (工程基础)/03-Verification and Diagnostics (验证与诊断)/07-Release Verification and Quality Gates (发布验证与质量门禁).md>) + [项目标准与自测](<../11-Projects (项目实践)/00-Project Standard (项目标准与自测).md>)：在干净环境演示启动、测试、回滚或恢复，列出已知限制。
- [ ] **项目复盘**：写下一次真实缺陷或故障的现象、证据、根因、修复和防回归措施。

# 算法与推理

与 1–5 并行；每完成一部分主线，选一块练习实现、不变量和反例。

- [ ] **第一块**：[数组与链表](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/01-Arrays and Linked Lists (数组与链表).md>)、[栈队列与哈希](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/02-Stack Queue and Hashing (栈队列与哈希).md>)、[二分查找](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/07-Binary Search (二分查找).md>)；为一个算法写区间或结构不变量。
- [ ] **第二块**：[树遍历与路径](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/04-Tree Traversal and Paths (树遍历与路径).md>)、[二叉搜索树](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/05-Binary Search Trees (二叉搜索树).md>)、[图并查集与最短路](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/06-Graphs Union Find and Shortest Paths (图并查集与最短路).md>)；比较遍历、连通和最短路问题。
- [ ] **第三块**：[一维动态规划](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/10-One Dimensional Dynamic Programming (一维动态规划).md>)、[背包动态规划](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/11-Knapsack Dynamic Programming (背包动态规划).md>)、[序列动态规划](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/13-Sequence Dynamic Programming (序列动态规划).md>)；从状态、转移和边界推导实现。
- [ ] **第四块**：[概率、统计与排队](<../01-Foundations (基础能力)/03-Mathematics and Reasoning (数学与推理)/02-Probability Statistics and Queueing (概率、统计与排队).md>)、[性能与系统思维](<../01-Foundations (基础能力)/01-CS Core (计算机核心)/12-Performance and System Thinking (性能与系统思维).md>)；用项目负载解释吞吐、并发和排队。
- [ ] **按需补充**：[排序算法](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/08-Sorting Algorithms (排序算法).md>)、[堆与 Top K](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/03-Heap and Top K (堆与 Top K).md>)、[回溯与状态空间搜索](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/09-Backtracking and State Space Search (回溯与状态空间搜索).md>)、[区间动态规划](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/12-Interval Dynamic Programming (区间动态规划).md>)、[贪心与滑动窗口](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/14-Greedy and Sliding Window (贪心与滑动窗口).md>)、[字符串与位运算](<../01-Foundations (基础能力)/02-Algorithms (算法与数据结构)/15-String and Bit Algorithms (字符串与位运算).md>)、[离散逻辑、集合与图](<../01-Foundations (基础能力)/03-Mathematics and Reasoning (数学与推理)/01-Discrete Logic Sets and Graphs (离散逻辑、集合与图).md>)。选与当前错误或岗位相关的一篇完成推演和验证。

# 6. 专项深化

主项目首版完成后，依据瓶颈、故障或岗位要求选择相关任务；无需完成所有方向。

## C++ 并发、Linux 与网络性能

- [ ] [线程锁与协作](<../03-C++ Backend (C++ 后端)/05-Concurrency and Performance (并发与性能)/01-Threads Locks and Coordination (线程锁与协作).md>) + [原子与内存序](<../03-C++ Backend (C++ 后端)/05-Concurrency and Performance (并发与性能)/02-Atomics and Memory Order (原子与内存序).md>)：先用共享状态与消息发布示例推导同步；无锁实现留给明确的瓶颈。
- [ ] [线程池](<../03-C++ Backend (C++ 后端)/05-Concurrency and Performance (并发与性能)/03-Thread Pool (线程池).md>) + [优雅关闭与请求排空](<../06-Systems and Networking (系统与网络)/03-Server Networking (服务器网络编程)/06-Graceful Shutdown and Drain (优雅关闭与请求排空).md>)：验证有界提交、停止与任务排空。
- [ ] [IO 模型与多路复用](<../06-Systems and Networking (系统与网络)/01-Linux Runtime (Linux 运行时)/08-IO Models and Multiplexing (IO 模型与多路复用).md>) + [epoll API 与触发模式](<../06-Systems and Networking (系统与网络)/01-Linux Runtime (Linux 运行时)/09-epoll API and Trigger Modes (epoll API 与触发模式).md>) + [Reactor 架构](<../06-Systems and Networking (系统与网络)/03-Server Networking (服务器网络编程)/02-Reactor Architecture (Reactor 架构).md>)：在已有服务上验证 LT/ET、EAGAIN 和部分写。
- [ ] [压测](<../06-Systems and Networking (系统与网络)/03-Server Networking (服务器网络编程)/05-Benchmarking (压测).md>) + [性能分析与优化](<../03-C++ Backend (C++ 后端)/06-Engineering Practice (工程实践)/05-Profiling and Optimization (性能分析与优化).md>)：只优化有基线和剖析证据的瓶颈。
- [ ] [系统与网络标准与自测](<../06-Systems and Networking (系统与网络)/00-Systems and Networking Standard (系统与网络标准与自测).md>) + [C++ 后端标准与自测](<../03-C++ Backend (C++ 后端)/00-C++ Backend Standard (C++ 后端标准与自测).md>)：按目标岗位查缺补漏；[零拷贝](<../06-Systems and Networking (系统与网络)/01-Linux Runtime (Linux 运行时)/11-Zero Copy (零拷贝).md>)、[epoll 内部实现](<../06-Systems and Networking (系统与网络)/01-Linux Runtime (Linux 运行时)/10-epoll Internals (epoll 内部实现).md>)、[协程](<../03-C++ Backend (C++ 后端)/03-Modern C++ (现代 C++)/08-Coroutines (协程).md>)等深入主题按需选学。

## 数据系统与分布式正确性

- [ ] [MVCC 与 Read View](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/06-MVCC and Read Views (MVCC 与 Read View).md>) + [锁与死锁](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/07-Locks and Deadlocks (锁与死锁).md>) + [日志与恢复](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/08-Logging and Recovery (日志与恢复).md>)：用真实事务场景推演可见性、等待和恢复。
- [ ] [查询分析与优化](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/09-Query Analysis and Optimization (查询分析与优化).md>) + [连接池](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/10-Connection Pool (连接池).md>)：从服务指标选择查询或连接问题，验证改动效果。
- [ ] [Redis 数据结构](<../07-Data Systems and Distributed Computing (数据系统与分布式)/02-Cache and Proxy (缓存与代理)/01-Redis Data Structures (Redis 数据结构).md>) + [缓存一致性问题](<../07-Data Systems and Distributed Computing (数据系统与分布式)/02-Cache and Proxy (缓存与代理)/03-Cache Consistency Problems (缓存一致性问题).md>) + [Redis 缓存模式](<../05-Go Backend (Go 后端)/04-Data Integration (数据集成)/02-Redis Cache Patterns (Redis 缓存模式).md>)：仅在项目需要缓存时接入，说明一致性窗口和失败回退。
- [ ] [消息队列](<../07-Data Systems and Distributed Computing (数据系统与分布式)/03-Messaging and Streaming (消息与流)/01-Message Queues (消息队列).md>) + [消息与幂等](<../05-Go Backend (Go 后端)/04-Data Integration (数据集成)/03-Messaging and Idempotency (消息与幂等).md>)：仅在项目需要异步边界时接入，验证重复投递与重试。
- [ ] [一致性与 CAP](<../07-Data Systems and Distributed Computing (数据系统与分布式)/04-Distributed Fundamentals (分布式基础)/01-Consistency and CAP (一致性与 CAP).md>) + [Raft 共识](<../07-Data Systems and Distributed Computing (数据系统与分布式)/04-Distributed Fundamentals (分布式基础)/02-Raft Consensus (Raft 共识).md>)：先手推故障场景；[Paxos 共识](<../07-Data Systems and Distributed Computing (数据系统与分布式)/04-Distributed Fundamentals (分布式基础)/03-Paxos (Paxos).md>)、[分布式事务](<../07-Data Systems and Distributed Computing (数据系统与分布式)/04-Distributed Fundamentals (分布式基础)/04-Distributed Transactions (分布式事务).md>)、[复制](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/11-Replication (复制).md>)和[分库分表](<../07-Data Systems and Distributed Computing (数据系统与分布式)/01-MySQL and SQL (MySQL 与 SQL)/12-Sharding and Partitioning (分库分表).md>)在遇到相应规模或岗位要求时深入。
- [ ] [数据系统与分布式标准与自测](<../07-Data Systems and Distributed Computing (数据系统与分布式)/00-Data Systems and Distributed Computing Standard (数据系统与分布式标准与自测).md>)：用项目中的事务、缓存或消息证据完成相关项。

## 云原生、可靠性与安全治理

- [ ] [容器镜像与运行时](<../08-Cloud Native and Reliability (云原生与可靠性)/01-Containers and Kubernetes (容器与 Kubernetes)/01-Container Images and Runtime (容器镜像与运行时).md>) + [配置、探针与发布](<../08-Cloud Native and Reliability (云原生与可靠性)/01-Containers and Kubernetes (容器与 Kubernetes)/03-Configuration Probes and Rollouts (配置、探针与发布).md>)：容器化已有服务，验证信号、探针与失败回滚。
- [ ] [Kubernetes 工作负载与网络](<../08-Cloud Native and Reliability (云原生与可靠性)/01-Containers and Kubernetes (容器与 Kubernetes)/02-Kubernetes Workloads and Networking (Kubernetes 工作负载与网络).md>) + [Kubernetes 排障与生产运维](<../08-Cloud Native and Reliability (云原生与可靠性)/01-Containers and Kubernetes (容器与 Kubernetes)/05-Kubernetes Troubleshooting and Production Operations (Kubernetes 排障与生产运维).md>)：在测试环境部署并定位一个 Pod 故障。
- [ ] [SLO、错误预算与告警](<../08-Cloud Native and Reliability (云原生与可靠性)/03-Reliability Engineering (可靠性工程)/01-SLO Error Budgets and Alerting (SLO、错误预算与告警).md>) + [容量、自动扩缩与成本](<../08-Cloud Native and Reliability (云原生与可靠性)/03-Reliability Engineering (可靠性工程)/02-Capacity Autoscaling and Cost (容量、自动扩缩与成本).md>)：依据用户路径与负载估算服务目标和容量。
- [ ] [事故响应、混沌与恢复](<../08-Cloud Native and Reliability (云原生与可靠性)/03-Reliability Engineering (可靠性工程)/03-Incident Response Chaos and Recovery (事故响应、混沌与恢复).md>) + [安全事件响应与恢复](<../09-Security Engineering (安全工程)/03-Isolation Detection and Response (隔离、检测与响应)/03-Incident Response and Recovery (安全事件响应与恢复).md>)：做一次可控故障演练，记录恢复与证据。
- [ ] [云身份与容器安全](<../09-Security Engineering (安全工程)/02-Identity Cloud and Supply Chain (身份、云与供应链)/02-Cloud Identity and Container Security (云身份与容器安全).md>) + [安全测试与漏洞管理](<../09-Security Engineering (安全工程)/02-Identity Cloud and Supply Chain (身份、云与供应链)/03-Security Testing and Vulnerability Management (安全测试与漏洞管理).md>)：检查服务身份、依赖和镜像的权限边界。
- [ ] [云原生与可靠性标准与自测](<../08-Cloud Native and Reliability (云原生与可靠性)/00-Cloud Native and Reliability Standard (云原生与可靠性标准与自测).md>) + [安全工程标准与自测](<../09-Security Engineering (安全工程)/00-Security Engineering Standard (安全工程标准与自测).md>)：按项目实际部署范围验收。

## AI 应用与基础设施

- [ ] [Token、向量与模型行为](<../10-AI Systems Engineering (AI 系统工程)/01-AI Foundations (AI 基础)/01-Tokens Embeddings and Model Behavior (Token、向量与模型行为).md>) + [模型 API 与结构化输出](<../10-AI Systems Engineering (AI 系统工程)/01-AI Foundations (AI 基础)/03-Model APIs and Structured Outputs (模型 API 与结构化输出).md>)：调用模型并校验结构、超时和失败响应。
- [ ] [评估数据与实验设计](<../10-AI Systems Engineering (AI 系统工程)/01-AI Foundations (AI 基础)/04-Evaluation Data and Experiment Design (评估数据与实验设计).md>) + [RAG 与向量检索](<../10-AI Systems Engineering (AI 系统工程)/02-LLM Applications and Agents (大模型应用与智能体)/02-RAG and Vector Retrieval (RAG 与向量检索).md>)：建立小型评估集，分别测召回与答案依据。
- [ ] [提示、上下文与工具调用](<../10-AI Systems Engineering (AI 系统工程)/02-LLM Applications and Agents (大模型应用与智能体)/01-Prompt Context and Tool Calling (提示、上下文与工具调用).md>) + [Agent 工作流、评估与安全](<../10-AI Systems Engineering (AI 系统工程)/02-LLM Applications and Agents (大模型应用与智能体)/03-Agent Workflow Evaluation and Safety (Agent 工作流、评估与安全).md>)：实现有限步骤、有限权限的工具流程。
- [ ] [安全 AI Agent 平台](<../11-Projects (项目实践)/02-Backend Projects (后端项目)/05-Secure AI Agent Platform (安全 AI Agent 平台).md>)：仅在此项目确实使用 AI 时，验证一个隔离或审计环节。
- [ ] [推理服务、批处理与缓存](<../10-AI Systems Engineering (AI 系统工程)/03-AI Infrastructure (AI 基础设施)/01-Inference Serving Batching and Caching (推理服务、批处理与缓存).md>) + [AI 可观测性、成本与安全](<../10-AI Systems Engineering (AI 系统工程)/03-AI Infrastructure (AI 基础设施)/03-AI Observability Cost and Security (AI 可观测性、成本与安全).md>)：在有实际负载时测延迟、吞吐和成本；[GPU 调度、容器与多租户](<../10-AI Systems Engineering (AI 系统工程)/03-AI Infrastructure (AI 基础设施)/02-GPU Scheduling Containers and Multi Tenancy (GPU 调度、容器与多租户).md>)按需要深入。
- [ ] [AI 系统工程标准与自测](<../10-AI Systems Engineering (AI 系统工程)/00-AI Systems Engineering Standard (AI 系统工程标准与自测).md>)：用实际调用、评估和运行证据验收相关项。

# 7. 系统设计与表达

从首个服务起记录证据；主项目可演示后，选相关案例训练设计和表达。

- [ ] [项目难点表达](<../12-Career (求职与成长)/01-Interview Expression (面试表达)/03-Project Deep Dive (项目难点表达).md>) + [后端能力证据](<../12-Career (求职与成长)/03-Career Execution (求职执行)/01-Backend Competency Evidence (后端能力证据).md>)：用一次真实故障或性能改动讲清约束、选择、结果和限制。
- [ ] [短链接系统](<../12-Career (求职与成长)/02-System Design (系统设计)/01-URL Shortener (短链接系统).md>)、[即时通信系统](<../12-Career (求职与成长)/02-System Design (系统设计)/02-IM System (即时通信系统).md>)、[秒杀系统设计](<../12-Career (求职与成长)/02-System Design (系统设计)/03-Flash Sale System Design (秒杀系统设计).md>)、[配置中心系统设计](<../12-Career (求职与成长)/02-System Design (系统设计)/04-Config Center System Design (配置中心系统设计).md>)、[API 网关](<../12-Career (求职与成长)/02-System Design (系统设计)/05-API Gateway (API 网关).md>)：选与主项目最接近的一题，完成需求、容量、数据流和失败分析。
- [ ] [数据速记](<../12-Career (求职与成长)/01-Interview Expression (面试表达)/02-Data Quick Reference (数据速记).md>) + [C++ 后端面试高频 100 题](<../12-Career (求职与成长)/01-Interview Expression (面试表达)/01-C++ Backend Interview Top 100 (C++ 后端面试高频 100 题).md>)：从真实项目选问题口述，答不出时回到原笔记验证。
- [ ] [简历证据与作品集](<../12-Career (求职与成长)/03-Career Execution (求职执行)/02-Resume Evidence and Portfolio (简历证据与作品集).md>) + [面试训练与复盘](<../12-Career (求职与成长)/03-Career Execution (求职执行)/03-Interview Practice and Retrospective (面试训练与复盘).md>)：让别人复现项目演示，做一次模拟追问并记录缺口。
- [ ] [求职能力标准与自测](<../12-Career (求职与成长)/00-Career Standard (求职能力标准与自测).md>)：按目标岗位核对证据，再决定下一轮专项。
