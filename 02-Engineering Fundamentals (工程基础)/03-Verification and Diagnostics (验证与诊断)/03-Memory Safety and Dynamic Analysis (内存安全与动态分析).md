---
status: stable
confidence: high
content_verified: 2026-09-18
verified: 2026-10-14
review_stage: learn
review_due: 2026-10-14
tags: [engineering/verification]
---

> [!abstract] 学习定位
> 内存安全工具通过运行时插桩或模拟观察已执行路径，能暴露越界、生命周期错误、未定义行为和数据竞争，但不能证明未执行路径正确。本篇统一选择、运行和解释动态分析工具的方法。

# 从失败类型选择工具

| 问题 | 首选工具 | 关键边界 |
|---|---|---|
| 越界、use-after-free、double free | AddressSanitizer (ASan) | 需要重新编译，未执行路径不可见 |
| 部分未定义行为 | UndefinedBehaviorSanitizer (UBSan) | 并非覆盖全部 C/C++ UB |
| 数据竞争 | ThreadSanitizer (TSan) | 开销较高，通常与 ASan 分开构建 |
| 未初始化读取 | MemorySanitizer / Valgrind Memcheck | MSan 要求依赖也被插桩 |
| 泄漏 | LeakSanitizer / Memcheck | 需区分真正泄漏与退出时仍可达 |
| 无法重编译的二进制 | Valgrind | 平台限制与较高运行开销 |

工具报告是“发生了可疑事件”的证据。先保留完整报告、构建参数和输入，再定位首次非法访问；最后崩溃位置可能只是内存早已被破坏后的受害点。

# Sanitizer 构建与运行

保持栈信息可读，并分别建立配置：

```bash
# ASan + UBSan
clang++ -O1 -g -fno-omit-frame-pointer \
  -fsanitize=address,undefined app.cpp -o app-asan

# TSan 单独构建
clang++ -O1 -g -fno-omit-frame-pointer \
  -fsanitize=thread app.cpp -o app-tsan
```

ASan 与 TSan 的运行时模型通常不兼容，不应放在同一二进制。测试环境尽量接近真实依赖与工作负载，但 sanitizer 构建的性能数据不能代表 release 构建。

CI 中让错误产生非零退出码并保存报告；不要用全局 suppression 让流水线“变绿”。第三方库确有已知问题时，抑制规则应限定调用栈、说明来源并设置复查条件。

# 报告定位与工具组合

以 ASan 的 heap-use-after-free 为例：

1. 看错误类型、访问线程和读写大小；
2. 看非法访问栈，回答“谁还在使用”；
3. 看释放栈，回答“生命周期在哪里结束”；
4. 看分配栈，确认对象身份；
5. 检查跨线程 ownership、回调捕获和取消顺序。

不要只在访问处加空指针判断。use-after-free 的修复通常在所有权、同步或任务生命周期，而非受害语句。

UBSan 报告的有符号溢出、错误移位、未对齐访问和无效 vptr 可能在当前机器上“看起来能跑”，但优化器可以基于语言规则产生不同结果。修复后还要把边界输入加入测试。

TSan 报告的是 data race：两个线程访问同一内存、至少一个写、且没有可识别的 happens-before。互斥、原子或线程封闭可以建立同步；仅凭“实际结果没错”不能忽略 race。

## Valgrind Memcheck

```bash
valgrind --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --error-exitcode=1 \
  ./app <args>
```

泄漏分类：

- definitely lost：没有指针能到达分配块，优先修复；
- indirectly lost：被已泄漏对象持有，通常随根泄漏消失；
- possibly lost：只剩内部指针，需要人工判断；
- still reachable：退出时仍可达，不一定是 bug，但长期进程要关注增长趋势。

Memcheck 的未初始化值报告应沿 origin 追到首次未初始化来源，而不是在条件判断处随便赋默认值。

## 覆盖盲区与组合验证

动态分析只检查实际执行路径，因此需要与测试策略组合：

```text
边界/性质/fuzz 输入 -> 扩大路径覆盖
sanitizer             -> 检测路径上的非法行为
static analysis        -> 检查部分未执行路径
review                 -> 验证 ownership 与并发设计
```

自定义 allocator、内联汇编、JIT、fork/exec、信号处理和跨语言 FFI 可能改变工具行为。遇到不确定报告，先查对应编译器版本文档并构造最小复现。

# CI 与回归证据

```text
debug tests
ASan + UBSan tests
TSan concurrency tests
release tests
scheduled Valgrind/fuzz corpus
```

不是每个提交都必须跑全部昂贵检查，但主分支和发布候选必须有明确策略。记录编译器版本、sanitizer 选项、suppression 和失败 artifact。

# 实践与资料

- [ ] 分别制造越界、use-after-free、泄漏和 data race，解释报告的每组栈。
- [ ] 让 sanitizer job 在发现错误时阻断 CI 并上传完整日志。
- [ ] 为一次生命周期 bug 画出 owner、borrower 与释放时序，再修复设计。
- [ ] 用 fuzz corpus 驱动 sanitizer，证明新输入能扩大路径覆盖。

## 关联专题

- [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)：用测试输入扩大动态分析覆盖。
- [Debugging and Failure Localization (调试与故障定位)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/02-Debugging%20and%20Failure%20Localization%20(调试与故障定位).md)：从报告回到首次错误状态。

## 参考资料

- [Clang AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- [Clang ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html)
- [Valgrind Memcheck Manual](https://valgrind.org/docs/manual/mc-manual.html)
