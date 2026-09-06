---
tags:
  - network/server
status: seed
review_due: 2026-09-26
confidence: 1
verified: stable
---

# Connection Pool Design — 连接池设计

> [!important] **核心考点**：连接池多线程安全设计、连接复用与回收、池大小调优与性能隔离

## 为什么需要连接池

建立 TCP 连接的成本：
- 三次握手：1.5 RTT
- TLS 握手：1-2 RTT（TLS 1.3/1.2）
- 慢启动阶段：初始拥塞窗口小

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

## 基本接口

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

## 连接保活与健康检查

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

## 连接池大小调优

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

## 30 秒回答 / 自测

- **30 秒回答**：连接池维护空闲/活跃两套连接集合，`acquire` 优先取空闲、未达上限则新建、达上限则阻塞等待；`release` 校验存活后放回或关闭，配合健康检查剔除坏连接。
- **常见误区**：连接池越大越好（实际受核心数与等待/处理时间比值约束，过大反而增加上下文切换）；归还前不校验存活，把坏连接放回池中。
- **自测**：1) `min_idle` / `max_idle` / `max_total` 分别控制什么？ 2) 如何避免"取到已被服务端关闭的连接"？

---

服务器设计模式系列见 → [Buffer Design：Read & Write Buffer (缓冲区设计)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/11-Buffer%20Design：Read%20&%20Write%20Buffer%20(缓冲区设计)%20⭐.md>) · [Graceful Shutdown (优雅关闭)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/13-Graceful%20Shutdown%20(优雅关闭)%20⭐.md>)
