---
tags:
  - network/server
status: 🌱
---

> [!important] **核心考点**：wrk/ab 压测工具使用、QPS/TPS/延迟指标分析、性能瓶颈定位方法

## 压测指标

**QPS（Queries Per Second）：** 每秒请求数，衡量吞吐量
**延迟（Latency）：** 请求从发起到响应的耗时
  - Avg：平均延迟
  - P50：50% 请求在此时间内完成（中位数）
  - P99：99% 请求在此时间内完成（长尾）
  - Max：最大延迟
**并发连接数：** 同时维持的连接数量

## wrk 使用

wrk 是轻量级 HTTP 压测工具，利用多线程 + epoll 生成高并发负载。

```bash
# 基本用法
wrk -t12 -c400 -d30s http://localhost:8080

# -t12     ：12 个线程
# -c400    ：400 个并发连接
# -d30s    ：压测 30 秒

# 输出示例：
# Running 30s test @ http://localhost:8080
#   12 threads and 400 connections
#   Thread Stats   Avg      Stdev     Max   +/- Stdev
#     Latency    45.67ms   12.34ms 345.67ms   85.20%
#     Req/Sec   723.45    89.12     1.23k    70.10%
#   259,842 requests in 30.00s, 342.99MB read
#   Requests/sec:   8661.40
#   Transfer/sec:     11.43MB
```

**高级选项：**
```bash
# POST 请求压测
wrk -t4 -c100 -d30s -s post.lua http://localhost:8080/api

# post.lua 内容：
wrk.method = "POST"
wrk.body   = '{"key": "value"}'
wrk.headers["Content-Type"] = "application/json"

# 打印详细延迟分布
wrk -t2 -c50 -d10s --latency http://localhost:8080
# 输出包含：P50/P75/P90/P99 延迟

# 使用管道（pipeline）
wrk -t2 -c50 -d10s -s pipeline.lua http://localhost:8080
```

## 吞吐量与延迟的关系

<img src="assets/file-20260620143339758.png" alt="吞吐量与延迟的性能拐点模型" width="800">

## 性能瓶颈定位流程

```
1. wrk 压测得到 QPS 和延迟
2. 观察 QPS 是否随并发线性增长
   是 → 未到瓶颈，继续增加并发
   否 → 到达瓶颈，缩小范围定位

3. 确认瓶颈方向：
    - CPU 100%          → 计算瓶颈（算法/内存分配/锁竞争）
    - CPU 低但 QPS 上不去 → IO 瓶颈（磁盘/网络/锁等待）
    - iowait 高          → 磁盘瓶颈
    - 上下文切换高       → 锁竞争/线程过多

4. 使用工具进一步定位：
    top/vmstat           → CPU、内存、上下文切换
    perf top             → 热点函数
    strace -c            → 系统调用频次
    iostat -x 1          → 磁盘 IO
    ss -s                → socket 统计
```

## ab（Apache Bench）

```bash
# 基本用法
ab -n 100000 -c 100 http://localhost:8080/

# -n 100000：总请求数
# -c 100   ：并发数

# 输出关注：
# Requests per second:    8661.40 [#/sec] (mean)  ← QPS
# Time per request:       11.543 [ms] (mean)       ← 平均延迟
# Transfer rate:          11434.56 [Kbytes/sec]    ← 吞吐量
# Failed requests:        0                        ← 错误数
```

## 常见问题与分析方法

| 现象 | 可能原因 | 验证方法 | 解决方向 |
|------|---------|---------|---------|
| QPS 低但 CPU 空闲 | 锁竞争 | `perf top` 看 spin_lock | 无锁结构/减少临界区 |
| QPS 到一定值后下降 | 资源耗尽 | 观察 fd/memory | 增加限制/优化资源管理 |
| 延迟 P50 低但 P99 高 | GC STW/锁竞争 | 火焰图 | 减少全 GC/分段锁 |
| QPS 波动大 | 定时任务/批处理 | strace 跟踪 | 均匀化调度/限制批处理量 |
| 上下文切换超高 | 线程太多 | `vmstat 1` 看 cs | 协程/线程池 |

## 压测注意事项

1. **压测客户端不能成为瓶颈**：wrk 客户端本身需要足够资源
2. **预热阶段**：前几秒数据不计（JIT 预热、缓存填充）
3. **避免同机压测**：客户端和服务端互相影响
4. **长时间压测**：短时压测无法发现内存泄漏
5. **关注错误率**：QPS 再高，有错误也没意义
6. **不要只看平均延迟**：P99 往往比平均延迟高 5-10 倍

> [!tip]- **工程要点**：压测的核心是**找到系统的极限拐点**而非跑出最大 QPS。先小并发找到延迟基线，再逐步增加并发直到 QPS 增速放缓或延迟出现拐点——拐点处的并发数就是服务器的"最佳并发度"。在此基础上留 30-50% 余量作为生产配置。

---

性能优化相关见 → [Connection Pool Design (连接池设计)](</05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/10-Connection%20Pool%20Design%20(连接池设计)%20⭐.md>) · [Buffer Design：Read & Write Buffer (缓冲区设计)](</05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/11-Buffer%20Design：Read%20&%20Write%20Buffer%20(缓冲区设计)%20⭐.md>)
