---
tags:
  - redis/core
status: seed
review_due: 2026-10-10
confidence: 1
verified: NEEDS_VERIFY
---

> [!important] **核心考点**
> hiredis 同步/异步 API、连接池设计、Pipeline 批量操作、Redis 项目集成模式

## hiredis 库

Redis 官方 C 客户端库，轻量、同步/异步 API 支持。

### 编译链接

```bash
git clone https://github.com/redis/hiredis.git
cd hiredis && make && make install
# 编译：g++ -o app main.cpp -lhiredis -levent
```

### 同步 API

```cpp
#include <hiredis/hiredis.h>
#include <iostream>

int main() {
    // 连接 Redis（默认 127.0.0.1:6379）
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        std::cerr << "Connection error: " << c->errstr << std::endl;
        return 1;
    }

    // 执行命令
    redisReply *reply = (redisReply*)redisCommand(c, "SET key1 value1");
    std::cout << "SET: " << reply->str << std::endl;
    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(c, "GET key1");
    if (reply->type == REDIS_REPLY_STRING) {
        std::cout << "GET: " << reply->str << std::endl;
    }
    freeReplyObject(reply);

    // 批量 Pipeline
    redisAppendCommand(c, "SET k1 v1");
    redisAppendCommand(c, "SET k2 v2");
    redisAppendCommand(c, "GET k1");

    redisReply *r1, *r2, *r3;
    redisGetReply(c, (void**)&r1); freeReplyObject(r1);
    redisGetReply(c, (void**)&r2); freeReplyObject(r2);
    redisGetReply(c, (void**)&r3);
    if (r3->type == REDIS_REPLY_STRING)
        std::cout << "Pipeline GET: " << r3->str << std::endl;
    freeReplyObject(r3);

    redisFree(c);
    return 0;
}
```

### 异步 API（基于 libevent）

```cpp
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>
#include <event.h>

void onConnect(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        std::cerr << "Error: " << c->errstr << std::endl;
        return;
    }
    std::cout << "Connected!" << std::endl;
}

void onCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = (redisReply*)r;
    if (reply == NULL) return;
    std::cout << "Callback: " << reply->str << std::endl;
}

int main() {
    event_base *base = event_base_new();
    redisAsyncContext *ac = redisAsyncConnect("127.0.0.1", 6379);
    redisLibeventAttach(ac, base);
    
    redisAsyncSetConnectCallback(ac, onConnect);
    redisAsyncCommand(ac, onCallback, NULL, "GET mykey");
    
    event_base_dispatch(base);
    redisAsyncFree(ac);
    return 0;
}
```

---

## 连接池设计（C++ 简单实现）

```cpp
class RedisPool {
    struct Connection {
        redisContext *ctx;
        bool in_use;
    };
    std::vector<Connection> pool_;
    std::mutex mtx_;
    std::condition_variable cv_;

public:
    RedisPool(int size) {
        for (int i = 0; i < size; i++) {
            redisContext *c = redisConnect("127.0.0.1", 6379);
            pool_.push_back({c, false});
        }
    }

    redisContext* get() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] {
            for (auto& conn : pool_)
                if (!conn.in_use) return true;
            return false;
        });
        for (auto& conn : pool_) {
            if (!conn.in_use) {
                conn.in_use = true;
                return conn.ctx;
            }
        }
        return nullptr; // unreachable
    }

    void put(redisContext *ctx) {
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto& conn : pool_) {
            if (conn.ctx == ctx) {
                conn.in_use = false;
                cv_.notify_one();
                return;
            }
        }
    }
};
```

---

## 项目集成架构

```
┌──────────────────────────┐
│  Business Code           │
│  get/set/incr            │
└──────┬───────────────────┘
       ↓
┌──────────────────────────┐
│  Cache Abstraction Layer │  ← 统一接口，支持序列化/压缩/降级
│  CacheClient             │
└──────┬───────────────────┘
       ↓
┌──────────────────────────┐
│  Redis Connection Pool   │  ← 连接复用，避免频繁建立 TCP
│  (hiredis)               │
└──────┬───────────────────┘
       ↓
┌──────────────────────────┐
│  Redis Cluster           │
└──────────────────────────┘
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| redisCommand 的返回值 | `redisReply*`，用完必须 `freeReplyObject` |
| Pipeline 为什么快 | 减少 RTT，一批命令一次发送，一次接收 |
| 连接池的必要性 | TCP 连接建立开销大（三次握手 + 认证） |
| 异步 hiredis 依赖 | 需要事件库（libevent/libev） |
| 序列化方式 | JSON / protobuf / MessagePack |

> [!tip]- **工程要点**
> 生产环境推荐使用 Pipeline 批量操作（可提升 5-10 倍吞吐）。注意 Pipeline 无事务性，中间失败不影响后续命令。超时时间设为 200-500ms 较为合理，避免长时间等待。

---

Redis 性能模型与缓存策略详解见 → [Redis Single Thread Model (单线程模型为何高性能)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01e-Redis%20Single%20Thread%20Model%20(单线程模型为何高性能)%20⭐.md) · [Expiration & Eviction Strategy (过期与淘汰策略)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01c-Expiration%20&%20Eviction%20Strategy%20(过期与淘汰策略)%20⭐.md)
