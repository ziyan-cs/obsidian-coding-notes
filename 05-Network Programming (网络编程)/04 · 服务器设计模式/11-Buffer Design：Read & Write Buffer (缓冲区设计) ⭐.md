---
tags:
  - network
  - server-design
---

> **核心考点**：读写 Buffer 设计模式、缓冲区扩容策略、读事件与写事件的管理

## 为什么需要 Buffer

网络编程中，数据以流的形式到达，无法预知每次 `read()` 会收到多少数据：

```
TCP 流可能分段：
  read → "GET /ind"
  read → "ex.html"
  read → " HTTP/1.1\r\nHost: "

Buffer 的作用：
  1. 暂存未处理完的数据
  2. 提供连续的、可扩展的内存空间
  3. 管理读写位置，避免数据拷贝
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
//    read_pos      write_pos            capacity
//    │             │                    │
//    v             v                    v
//  ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
//  │ │ │ │H│e│l│l│o│ │W│o│r│l│d│ │ │ │
//  └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
//  已读│    待handledata     │ Free Space │
//      ← 可回收空间 →
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

## 写 Buffer 与事件管理

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

    ssize_t n = write(conn->fd, conn->data + conn->read_pos, pending);
    if (n > 0) {
        conn->read_pos += n;  // 标记已发送
    }
    // 如果 n < pending，下次 EPOLLOUT 继续发送
}
```

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

> **工程要点**：Buffer 设计的核心是**减少数据拷贝**和**避免缓冲区溢出**。Compact + 翻倍扩容是最常用的组合——每次读操作前先 compact（将未读数据移到头部），空间不够时翻倍扩容。对于高性能场景，使用 `readv`/`writev` 实现零拷贝发送，避免用户空间的数据拼接。

---

## 关联笔记

- [Connection Pool Design (连接池设计)](/05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/10-Connection%20Pool%20Design%20(连接池设计)%20⭐.md)
- [Server Performance：Benchmarking with wrk (压测)](/05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/12-Server%20Performance：Benchmarking%20with%20wrk%20(压测)%20⭐.md)
- [Graceful Shutdown (优雅关闭)](/05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/13-Graceful%20Shutdown%20(优雅关闭)%20⭐.md)
- [Backend Architecture Patterns：分层架构, CQRS, 事件驱动 (后端架构模式)](/05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/14-Backend%20Architecture%20Patterns：分层架构,%20CQRS,%20事件驱动%20(后端架构模式)%20⭐.md)
- [TCP⧸IP Stack Overview (协议栈总览)](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/01-TCP⧸IP%20Stack%20Overview%20(协议栈总览).md)
