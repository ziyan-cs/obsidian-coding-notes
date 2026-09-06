---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# 30 秒回答

**核心结论**：学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。


# Connection Pool Design (连接池设计)

> [!note] 本节重点：核心考点：连接池多线程安全设计、连接复用与回收、池大小调优与性能隔离

# 为什么需要连接池

建立 TCP 连接的成本：
- 三次握手：1.5 RTT
- TLS 握手：1-2 RTT（TLS 1.3/1.2）
- 慢启动阶段：初始拥塞窗口小

对于频繁的短时请求，每次新建连接的开销巨大。连接池通过**复用已有连接**消除这些开销。

# 连接池的核心职责

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

# 基本接口

```c
typedef struct connection_pool {
    // 连接链表
    connection *idle_list;      // 空闲连接
    connection *active_list;    // 活跃连接
    int idle_count;
    int active_count;
    int max_total;
    int min_idle;
    int max_idle;

    // 同步
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    // 统计
    uint64_t total_created;
    uint64_t total_acquired;
    uint64_t total_timeout;
} connection_pool;

// 从连接池获取连接
connection *pool_acquire(connection_pool *pool) {
    pthread_mutex_lock(&pool->mutex);

    // 有空闲连接 → 直接取出
    if (pool->idle_list != NULL) {
        connection *conn = pool->idle_list;
        pool->idle_list = conn->next;
        pool->idle_count--;
        conn->next = pool->active_list;
        pool->active_list = conn;
        pool->active_count++;
        pthread_mutex_unlock(&pool->mutex);
        return conn;
    }

    // 没有空闲但未达上限 → 新建
    if (pool->active_count < pool->max_total) {
        connection *conn = create_new_connection();
        conn->next = pool->active_list;
        pool->active_list = conn;
        pool->active_count++;
        pool->total_created++;
        pthread_mutex_unlock(&pool->mutex);
        return conn;
    }

    // 已达上限 → 等待
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += MAX_WAIT_SEC;
    int ret = pthread_cond_timedwait(&pool->cond, &pool->mutex, &ts);
    if (ret == ETIMEDOUT) {
        pool->total_timeout++;
        pthread_mutex_unlock(&pool->mutex);
        return NULL;  // 超时返回
    }

    // 被唤醒后递归获取
    pthread_mutex_unlock(&pool->mutex);
    return pool_acquire(pool);
}

// 归还连接到连接池
void pool_release(connection_pool *pool, connection *conn) {
    pthread_mutex_lock(&pool->mutex);

    // 检查连接是否有效
    if (!conn_is_alive(conn)) {
        close_connection(conn);
        pool->active_count--;
        // 如果低于最小空闲，新建一个
        if (pool->idle_count < pool->min_idle)
            create_idle_connection(pool);
        pthread_cond_signal(&pool->cond);  // 唤醒等待者
        pthread_mutex_unlock(&pool->mutex);
        return;
    }

    // 从活跃列表移除
    remove_from_active(pool, conn);

    if (pool->idle_count >= pool->max_idle) {
        // 空闲连接太多了，关闭
        close_connection(conn);
    } else {
        // 放回空闲列表
        conn->next = pool->idle_list;
        pool->idle_list = conn;
        pool->idle_count++;
    }
    pool->active_count--;

    pthread_cond_signal(&pool->cond);  // 唤醒等待者
    pthread_mutex_unlock(&pool->mutex);
}
```

# 连接保活与健康检查

```c
// 后台线程定期检查
void *health_check_thread(void *arg) {
    connection_pool *pool = (connection_pool *)arg;
    while (1) {
        sleep(HEALTH_CHECK_INTERVAL);  // 每 5 秒检查一次

        pthread_mutex_lock(&pool->mutex);
        connection *prev = NULL;
        connection *curr = pool->idle_list;

        while (curr) {
            if (!conn_is_alive(curr)) {
                // 移除坏连接
                if (prev) prev->next = curr->next;
                else pool->idle_list = curr->next;
                close_connection(curr);
                pool->idle_count--;

                // 补充新连接
                connection *new_conn = create_new_connection();
                new_conn->next = pool->idle_list;
                pool->idle_list = new_conn;
                pool->idle_count++;
                break;  // 重新开始遍历
            }
            prev = curr;
            curr = curr->next;
        }
        pthread_mutex_unlock(&pool->mutex);
    }
}
```

# 连接池大小调优

**误区：连接池越大越好**

```
连接池大小的启发式（仅用于提出初始假设，NEEDS_VERIFY）：
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

# 30 秒回答 / 自测

- **30 秒回答**：连接池维护空闲/活跃两套连接集合，`acquire` 优先取空闲、未达上限则新建、达上限则阻塞等待；`release` 校验存活后放回或关闭，配合健康检查剔除坏连接。
- **常见误区**：连接池越大越好（实际受核心数与等待/处理时间比值约束，过大反而增加上下文切换）；归还前不校验存活，把坏连接放回池中。
- **自测**：1) `min_idle` / `max_idle` / `max_total` 分别控制什么？ 2) 如何避免"取到已被服务端关闭的连接"？

---

服务器设计模式系列见 → [Buffer Design：Read & Write Buffer (缓冲区设计)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/11-Buffer%20Design：Read%20&%20Write%20Buffer%20(缓冲区设计)%20⭐.md>) · [Graceful Shutdown (优雅关闭)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/13-Graceful%20Shutdown%20(优雅关闭)%20⭐.md>)

---

# Protocol Framing and Buffering (协议分帧与缓冲区)

> [!note] 本节重点：核心考点：读写 Buffer 设计模式、缓冲区扩容策略、读事件与写事件的管理

# 为什么需要 Buffer

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

# Buffer 核心结构

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

**关键操作：**

```c
// 可读数据长度
size_t buffer_readable(buffer *b) {
    return b->write_pos - b->read_pos;
}

// 可写空间长度
size_t buffer_writable(buffer *b) {
    return b->capacity - b->write_pos;
}

// 初始化
void buffer_init(buffer *b, size_t initial_size) {
    b->data = malloc(initial_size);
    b->capacity = initial_size;
    b->read_pos = 0;
    b->write_pos = 0;
}

// 写入数据（从 socket 读入 buffer）
int buffer_append(buffer *b, const char *data, size_t len) {
    if (b->capacity - b->write_pos < len) {
        // 空间不够 → 先尝试 compact，再扩容
        buffer_compact(b);
        if (b->capacity - b->write_pos < len) {
            buffer_expand(b, b->capacity * 2);  // 翻倍扩容
        }
    }
    memcpy(b->data + b->write_pos, data, len);
    b->write_pos += len;
    return 0;
}

// 读取数据
size_t buffer_read(buffer *b, char *out, size_t len) {
    size_t readable = buffer_readable(b);
    size_t n = len < readable ? len : readable;
    memcpy(out, b->data + b->read_pos, n);
    b->read_pos += n;
    return n;
}

// Compact：将未读数据移到头部
void buffer_compact(buffer *b) {
    size_t readable = buffer_readable(b);
    if (readable > 0 && b->read_pos > 0) {
        memmove(b->data, b->data + b->read_pos, readable);
    }
    b->read_pos = 0;
    b->write_pos = readable;
}

// 扩容
void buffer_expand(buffer *b, size_t new_capacity) {
    b->data = realloc(b->data, new_capacity);
    b->capacity = new_capacity;
}
```

# 写 Buffer 与事件管理

对于非阻塞 socket，`write()` 可能无法一次性发送所有数据：

```c
// 写缓冲区：暂存未发送完的数据
typedef struct connection {
    buffer read_buf;     // 读缓冲区：暂存收到的数据
    buffer write_buf;    // 写缓冲区：暂存待发送的数据
    int fd;
    int events;          // 当前关注的事件（EPOLLIN / EPOLLOUT）
} connection;

// 尝试发送数据
int connection_send(connection *conn, const char *data, size_t len) {
    // 如果写缓冲区为空，尝试直接发送
    if (buffer_readable(&conn->write_buf) == 0) {
        ssize_t n = write(conn->fd, data, len);
        if (n > 0) {
            data += n;
            len -= n;
        }
    }

    // 未发送完的放入缓冲区
    if (len > 0) {
        buffer_append(&conn->write_buf, data, len);
        // 关注 EPOLLOUT 事件（socket 可写时继续发送）
        conn->events |= EPOLLOUT;
        update_epoll_events(conn);
    }
}

// 在事件循环中处理 EPOLLOUT
void handle_write(connection *conn) {
    size_t pending = buffer_readable(&conn->write_buf);
    if (pending == 0) {
        // 写缓冲区清空，取消对 EPOLLOUT 的关注
        conn->events &= ~EPOLLOUT;
        update_epoll_events(conn);
        return;
    }

    ssize_t n = write(conn->fd,
                      conn->write_buf.data + conn->write_buf.read_pos,
                      pending);
    if (n > 0) {
        conn->write_buf.read_pos += n;  // 标记已发送
    }
    // 如果 n < pending，下次 EPOLLOUT 继续发送
}
```

# 扩容策略对比

| 策略 | 空间浪费 | 扩容次数 | 适用场景 |
|------|---------|---------|---------|
| 固定大小 | 无 | 0 | 已知最大消息大小 |
| 翻倍扩容 | 最多浪费 50% | O(log n) | 通用场景 |
| 1.5 倍扩容 | 略多 | O(log n) | 内存敏感场景 |
| 分段链表（chain） | 极少 | 无需扩容 | 流式处理 |

**翻倍扩容 vs 1.5 倍：**
- 翻倍扩容：每次扩容大小翻倍，均摊 O(1)，但可能浪费内存
- 1.5 倍扩容：增长更平缓，内存利用率更高（翻倍可能导致下一块分配失败）

# 零拷贝 Buffer

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

# 实际项目中的 Buffer

- **libevent**：`struct evbuffer`，支持链式存储、零拷贝、回调通知
- **muduo**：`Buffer` 类，用 `std::vector<char>` 实现，prependable 空间支持
- **Redis**：`sds`（简单动态字符串），用空间预分配消除 realloc 热点

> [!tip]- **工程要点**：Buffer 设计首先要保证边界、部分读写与背压正确，再考虑减少 copy。Compact 不是“每次读事件都必须做”，应在需要连续空闲空间时再做；`readv`/`writev` 减少用户态拼接，但不自动消除所有 copy 或内核开销。

# 30 秒回答 / 自测 · 延伸要点 2
- **30 秒回答**：读写 Buffer 用 `read_pos`/`write_pos` 两个游标区分"已读/待处理/可写"三段；写不下先 compact 再翻倍扩容；非阻塞 `write` 写不完的暂存写 Buffer 并注册 `EPOLLOUT`。
- **常见误区**：`write()` 返回正数但 < len 时直接丢弃剩余数据；扩容后旧指针失效未更新。
- **自测**：1) 为什么每次读事件要先 `buffer_compact`？ 2) `EPOLLOUT` 何时注册、何时撤销？

---

服务器设计模式系列见 → [Connection Pool Design (连接池设计)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/10-Connection%20Pool%20Design%20(连接池设计)%20⭐.md>) · [Server Performance：Benchmarking with wrk (压测)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/12-Server%20Performance：Benchmarking%20with%20wrk%20(压测)%20⭐.md>)



# 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

# 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **04-Connections Buffers and Framing (连接缓冲与分帧)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
