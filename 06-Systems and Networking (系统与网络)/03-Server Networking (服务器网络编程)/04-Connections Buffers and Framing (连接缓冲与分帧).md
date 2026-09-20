---
study_stage: backlog
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

> [!summary] 核心摘要
>
> 连接对象同时管理 fd、读写缓冲、协议解析状态和生命周期。TCP 是字节流，必须用长度、分隔符或固定格式恢复消息边界，并处理半包、粘包、背压和关闭竞态。

# Connection Pool Design (连接池设计)

> [!note] 本节重点：连接池多线程安全设计、连接复用与回收、池大小调优与性能隔离

## 为什么需要连接池

建立连接可能付出 TCP 往返、TLS 握手、认证和连接初始化成本；典型 TCP 客户端需要约 1 RTT 才能完成建连，TLS 1.3 完整握手通常再需 1 RTT，恢复路径与 TCP Fast Open/QUIC 则另论。不能把“1.5 RTT + 1–2 RTT”当作所有连接的固定公式。初始拥塞窗口也会影响新连接的数据传输。

对于频繁的短时请求，每次新建连接的开销巨大。连接池通过**复用已有连接**消除这些开销。

## 连接池的核心职责

```text
┌───────────────────────────────────────────┐
│  Application                              │
├──────────┬──────────┬─────────────────────┤
│ Thread 1 │ Thread 2 │  Thread 3           │
└─────┬────┴─────┬────┴────────┬────────────┘
      │ get()    │ get()       │ get()
      ▼          ▼              ▼
┌───────────────────────────────────────────┐
│  Connection Pool                          │
├───────────────────────────────────────────┤
│  ┌─────────────────────┐  ┌────────────┐  │
│  │ Idle Connection Q   │  │ In-Use Set │  │
│  │ (idle_connections)  │  │ (in_use)   │  │
│  └──────────┬──────────┘  └──────┬─────┘  │
│             │                    │        │
│     ┌───────┴───────┐    ┌───────┴───┐    │
│     │ min_idle      │    │ max_size  │    │
│     └───────────────┘    └───────────┘    │
└──────────┬────────────────────────────────┘
           │ acquire/release
           ▼
┌───────────────────────────────────────────┐
│  Database                                 │
├──────────┬──────────┬─────────────────────┤
│ MySQL 1  │ MySQL 2  │  MySQL 3            │
└──────────┴──────────┴─────────────────────┘
```

**核心参数：**
- `min_idle`：最小空闲连接数（预热保持）
- `max_idle`：最大空闲连接数（防止资源浪费）
- `max_total`：最大总连接数（防止打垮后端）
- `max_wait`：获取连接的最大等待时间

## 连接池的状态与所有权

真实连接池的核心不是一段链表代码，而是保持 `idle + leased + creating + returning <= max_total`（建连预留与归还清理中的连接也计入容量）。一条连接在任意时刻只属于其中一种状态；返回池时必须先从借用集合移出。池关闭后不再发放连接，所有等待者得到明确错误。

| 操作 | 必须处理的边界 |
| --- | --- |
| `acquire(deadline)` | 空闲连接可能已失效；池满时等待必须保留原 deadline，取消/关闭要唤醒等待者 |
| 建连 | 先在锁内预留容量，再解锁执行可能阻塞的 connect/auth；失败时回收预留并通知等待者 |
| `release(conn)` | 连接只归还一次；事务/协议状态须复位，坏连接要从借用集合移除并释放容量 |
| 健康检查 | 锁内摘取候选，锁外做网络 I/O；检查通过也不保证下一次使用不会断开 |
| `close()` | 拒绝新借用，关闭空闲连接，等待或取消在用连接并按期限返回 |

```text
acquire(deadline):
  lock pool
  loop:
    if closing: unlock; return pool-closed
    if idle exists: take one; mark leased; unlock; validate/use or retire
    if idle + leased + creating + returning < max_total:
      creating++; unlock; connect with deadline
      lock; creating--; if success and not closing: mark leased; unlock; return
      notify waiter; unlock; return connection error
    wait on condition until state changes or original deadline expires

release(conn):
  lock; verify conn is leased exactly once; move leased -> returning; unlock
  reset protocol state outside lock (may fail or require I/O)
  lock; move returning -> idle only if healthy, open and below idle limit
  otherwise release capacity; notify waiter; unlock; close retired conn
```

这是**算法伪代码**，不是可直接编译的 C/C++。实际实现还需解决“锁外校验后状态又变化”的并发竞态、建连失败后的唤醒、条件变量虚假唤醒、时钟选择、连接所有权与异常安全。`pthread_cond_timedwait` 默认按条件变量配置的时钟解释绝对时间；若选择单调时钟，初始化属性与构造 deadline 必须一致，不能递归重置超时。

## 活性与容量验证

测试至少覆盖：池满时等待直到超时；多个等待者被归还连接唤醒；建连失败后容量不泄漏；关闭期间借用者归还；坏连接不再发放；重复 release 被拒绝。指标应包含空闲/借用/创建中数量、等待队列、等待耗时、建连错误、超时和失效淘汰。只有保证不变量与故障路径，才谈池大小调优。

## 连接池大小调优

**误区：连接池越大越好**

```
连接池大小的启发式（仅用于提出初始假设，MEASURE_LOCALLY）：
  连接数 = 核心数 × (1 + 等待时间 / 处理时间)

  例如：CPU 8 核，请求处理时间 5ms，等待 DB 响应 20ms
  连接数 = 8 × (1 + 20/5) = 8 × 5 = 40

  连接太少：CPU 等待，利用率低
  连接太多：上下文切换 + 资源竞争，性能反而下降
```

**调优实践：**
1. 从较小的池开始（如 10-20）
2. 逐步增加，观察 TPS/QPS 增速
3. 当 TPS 不再随连接数增长时停止
4. 监控连接等待超时率

> [!tip]- **工程要点**：连接池的正确实现涉及超时、取消、健康检查、关闭、总连接上限与监控。不要在池锁内执行可能阻塞的建连或健康检查；连接数先由后端容量和等待指标约束，再用压测调整。不存在通用“最快连接池”实现。

> [!summary] 核心摘要
>
> - **常见误区**：连接池越大越好（实际受核心数与等待/处理时间比值约束，过大反而增加上下文切换）；归还前不校验存活，把坏连接放回池中。
> - **自测**：1) `min_idle` / `max_idle` / `max_total` 分别控制什么？ 2) 如何避免"取到已被服务端关闭的连接"？
>
> ---
>
> ---

# Protocol Framing and Buffering (协议分帧与缓冲区)

> [!note] 本节重点：读写 Buffer 设计模式、缓冲区扩容策略、读事件与写事件的管理

## 为什么需要 Buffer

网络编程中，数据以流的形式到达，无法预知每次 `read()` 会收到多少数据：

```text
┌──────────────────────────┐    ┌──────────────────────────┐
│  Network Interface /     │    │  Network Interface /     │
│  Socket                  │    │  Socket                  │
└──────────┬───────────────┘    └──────────────────────────┘
           │ epoll readable             ▲
           ▼                            │ epoll writable
┌──────────────────────────┐    ┌───────┴──────────────────┐
│  Read Buffer             │    │  Write Buffer            │
│  (ring buffer / kbuf)    │    │  (ring buffer / kbuf)    │
│  read_pos → write_pos    │    │  write_pos → send_pos    │
└──────────┬───────────────┘    └──────────────────────────┘
           │ read / parse               ▲
           ▼                            │ encode / serialize
┌───────────────────────────────────────┴──────────────────┐
│  Application Layer Processing                            │
└──────────────────────────────────────────────────────────┘
```

## Buffer 核心结构

```c
typedef struct buffer {
    char *data;           // 底层内存
    size_t capacity;      // 总容量
    size_t read_pos;      // 已读位置（读指针）
    size_t write_pos;     // 已写位置（写指针）
} buffer;

// 图解：
//
//          read_pos    write_pos capacity
//          │           │         │
//          v           v         v
//  ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
//  │ │ │ | │H│e│l│l│o│ │W│o│r│l│d│ │ │ │ | | | |
//  └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
//  ┌───────┬─────────────────────┬─────────────┐
//  | Read  |   Pending Valid     | Free Write  |
//  | Area  │      Data           │   Space     │
//  ├───────┴─────────────────────┼─────────────┤
//  |    ← Recyclable Memory →    |             |
//  └─────────────────────────────┴─────────────┘
```

**核心不变量：** `0 <= read_pos <= write_pos <= capacity`；可读区间为 `[read_pos, write_pos)`。必须另设每连接读/写缓冲上限，防止慢客户端或恶意长度让内存无界增长。

```text
append(bytes):
  reject if bytes exceeds configured remaining capacity
  if tail space insufficient: compact unread bytes only when worthwhile
  if still insufficient: grow to at least write_pos + bytes.length
      check size_t overflow, max capacity and allocation failure first
  copy bytes into tail; advance write_pos

consume(n):
  n = min(n, write_pos - read_pos)
  process/copy exactly n bytes; advance read_pos
  if read_pos == write_pos: reset both offsets to zero
```

这是状态算法，不是可直接编译的 C。若用 `realloc`，先写入临时指针，成功后才替换旧指针；否则分配失败会丢失原指针。扩容不能只做一次 `capacity * 2`：大请求可能仍放不下，而且乘法会溢出。读取协议长度字段后先校验最大帧长，再考虑分配。

## 写 Buffer 与事件管理

非阻塞 `send/write` 可能只写一部分，也可能返回 `EINTR`、`EAGAIN`、`EPIPE` 等。以下流程由连接所属 I/O loop 执行，跨线程业务结果必须先投递回该 loop：

```text
enqueue(response):
  if pending bytes + response exceeds high-water mark: pause read / reject / close by policy
  append response to bounded write buffer; try_flush()

try_flush():
  while pending bytes exist:
    n = send(fd, pending prefix)
    if n > 0: consume exactly n bytes; continue
    if EINTR: retry
    if EAGAIN or EWOULDBLOCK: enable EPOLLOUT; return
    otherwise: record error and close connection through owner loop
  disable EPOLLOUT when buffer becomes empty
  if pending bytes dropped below low-water mark: resume reads if safe
```

`EPOLLOUT` 通常持续就绪，因此不要在无待发送数据时长期关注它，否则事件循环可能空转。ET 模式下处理一次可写事件后应尽力发送到 `EAGAIN` 或清空；LT 模式也要限制单连接单轮工作量，避免饿死其他连接。对端关闭与 fd 复用期间，只有所属 loop 才能安全修改该连接的事件和缓冲状态。

## 扩容策略对比

| 策略 | 空间浪费 | 扩容次数 | 适用场景 |
|------|---------|---------|---------|
| 固定大小 | 无 | 0 | 已知最大消息大小 |
| 翻倍扩容 | 最多浪费 50% | O(log n) | 通用场景 |
| 1.5 倍扩容 | 略多 | O(log n) | 内存敏感场景 |
| 分段链表（chain） | 极少 | 无需扩容 | 流式处理 |

**翻倍扩容 vs 1.5 倍：**
- 翻倍扩容：每次扩容大小翻倍，均摊 O(1)，但可能浪费内存
- 1.5 倍扩容：增长更平缓，内存利用率更高（翻倍可能导致下一块分配失败）

## 零拷贝 Buffer

高级 Buffer 设计使用**指针/偏移量**而非 memcpy 来避免数据拷贝：

```c
// 使用分散-聚集 IO（readv/writev）
typedef struct buffer_chain {
    struct buffer_chain *next;
    char *data;
    size_t len;
    size_t total_size;  // 包含未使用部分
} buffer_chain;

// writev 一次发送多个 buffer_chain
struct iovec iov[MAX_IOV];
int iovcnt = 0;
for (buffer_chain *c = head; c && iovcnt < MAX_IOV; c = c->next) {
    iov[iovcnt].iov_base = c->data;
    iov[iovcnt].iov_len = c->len;
    iovcnt++;
}
ssize_t n = writev(fd, iov, iovcnt);
```

## 实际项目中的 Buffer

- **libevent**：`struct evbuffer`，支持链式存储、零拷贝、回调通知
- **muduo**：`Buffer` 类，用 `std::vector<char>` 实现，prependable 空间支持
- **Redis**：`sds`（简单动态字符串），用空间预分配消除 realloc 热点

> [!tip]- **工程要点**：Buffer 设计首先要保证边界、部分读写与背压正确，再考虑减少 copy。Compact 不是“每次读事件都必须做”，应在需要连续空闲空间时再做；`readv`/`writev` 减少用户态拼接，但不自动消除所有 copy 或内核开销。

复查时重点追问两个状态边界：扩容失败后原缓冲区是否仍可用？`EPOLLOUT` 只在有待发送字节时关注，发送完是否确实撤销？**不应**每次读事件都 `compact`；只有尾部空间不足且头部可回收时才考虑搬移。
