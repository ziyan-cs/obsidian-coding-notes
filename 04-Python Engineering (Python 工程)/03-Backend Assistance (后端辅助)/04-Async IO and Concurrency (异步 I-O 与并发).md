---
status: stable
confidence: medium
content_verified: 2026-09-18
previous_review_due: 2026-10-03
---

> [!abstract] 学习定位
> `asyncio` 适合大量可等待的 I/O；CPU 密集计算仍应使用进程、原生扩展或专门服务。

> [!summary] 核心摘要
>
> 顺序任务保持同步；大量网络等待时使用异步 I/O；CPU 密集任务使用多进程或下沉到 C++。无论模型如何，都要有并发上限、timeout、取消和失败分类。

# 事件循环心智模型

协程运行到 `await` 时，如果所等待的操作尚未完成，就把执行权交还事件循环。事件循环选择其他可运行任务；操作完成后，原任务才有机会继续。因此异步提高的是等待期间的利用率，不会自动缩短 CPU 计算。

- coroutine 是可暂停的计算描述；调用 `async def` 函数只得到协程对象。
- task 把协程安排进事件循环，并保存结果或异常。
- `await` 表达依赖关系；它不是新线程，也不保证并行。
- 在协程里直接调用阻塞库或长时间计算，会卡住同一事件循环上的其他任务。

```text
task A: run ---- await network ........ resume
task B:      run ---- await database ........ resume
event loop chooses runnable work; it cannot preempt blocking Python code
```

# 有界并发

无限制创建 task 会同时消耗连接、内存、文件描述符并压垮下游。并发控制必须靠 queue、semaphore 或固定 worker 数表达。

```python
import asyncio

async def fetch_one(client, url, limit):
    async with limit:
        async with asyncio.timeout(3):
            return await client.get(url)

async def run_all(client, urls):
    limit = asyncio.Semaphore(20)
    async with asyncio.TaskGroup() as group:
        tasks = [group.create_task(fetch_one(client, u, limit)) for u in urls]
    return [task.result() for task in tasks]
```

示例表达结构，不规定生产参数。并发数和 timeout 应根据下游容量、连接池、延迟基线与资源限制测量。输入可能无限到来时，不应先为全部输入创建 task；使用有界 queue，让生产速度受消费者容量约束。

# timeout、取消与失败

timeout 触发后要让取消继续传播，并在 `finally` 或异步 context manager 中释放连接、锁和临时资源。不要捕获 `CancelledError` 后悄悄继续，否则服务关闭时任务可能无法收敛。

重试只适合瞬态故障，并且要有：

- 最大次数或总时间预算；
- 指数退避与随机抖动；
- 幂等请求或幂等键；
- 对永久错误、限流和取消的单独处理。

`TaskGroup` 提供结构化并发：子任务的生命周期受同一作用域管理，一个任务失败时会协调取消其余任务，并以异常组报告。使用前以项目支持的 Python 版本文档为准。

# 阻塞与 CPU 工作

| 工作负载 | 优先模型 | 边界 |
| --- | --- | --- |
| 少量顺序 I/O | 同步代码 | 最容易阅读与调试 |
| 支持异步 API 的大量网络 I/O | `asyncio` | 需要有界并发与取消 |
| 暂无异步接口的短阻塞 I/O | `asyncio.to_thread` | 不用于逃避 CPU 瓶颈 |
| 纯 Python CPU 密集计算 | 进程池 | 参数复制、启动与回收有成本 |
| 已有高性能算法 | 原生扩展或独立服务 | 明确内存、ABI 或网络边界 |

# 排查顺序

先记录任务数、队列长度、成功率、timeout、P95/P99 与事件循环延迟。若吞吐下降，检查是否存在阻塞调用、无界任务、连接池排队、下游限流或重试风暴，而不是先增加并发。

> [!question]- 理解检查
> 为什么把同步 HTTP 客户端直接放进 `async def` 不会得到真正的异步并发？当消费者速度低于生产者时，有界 queue 如何保护内存和下游？

> [!info]- 官方参考
> - [Coroutines and Tasks](https://docs.python.org/3/library/asyncio-task.html)
> - [Synchronization Primitives](https://docs.python.org/3/library/asyncio-sync.html)
> - [concurrent.futures](https://docs.python.org/3/library/concurrent.futures.html)
