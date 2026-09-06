---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# Connection Pool (连接池)

> [!note] 本节重点：核心考点：MySQL C API 连接池实现、连接复用与并发控制、连接池参数调优

# MySQL C API 基础

```c
#include <mysql/mysql.h>

// 初始化 MySQL 连接
MYSQL *conn = mysql_init(NULL);

// 连接数据库
mysql_real_connect(conn, host, user, pass, db, port, NULL, 0);

// 执行 SQL
mysql_query(conn, "SELECT * FROM user");

// 获取结果
MYSQL_RES *result = mysql_store_result(conn);
MYSQL_ROW row;
while ((row = mysql_fetch_row(result))) {
    // 处理行数据
}
mysql_free_result(result);

// 关闭连接
mysql_close(conn);
```

# 连接池核心设计

数据库连接池的核心与通用网络连接池类似，但增加了 MySQL 特有的检查机制：

```c
typedef struct db_connection {
    MYSQL *mysql;           // MySQL 连接句柄
    time_t last_used;       // 最后使用时间
    int is_alive;           // 连接是否有效
    int transaction_open;   // 是否有未提交的事务
    char db[64];            // 当前数据库
} db_connection;

typedef struct db_connection_pool {
    db_connection *connections;
    int max_size;           // 最大连接数
    int min_size;           // 最小空闲连接数
    int current_size;       // 当前连接数
    int idle_count;         // 空闲连接数

    // 连接配置（用于新建连接）
    char host[256];
    int port;
    char user[64];
    char pass[64];
    char db[64];

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} db_connection_pool;
```

# 连接有效性检查

MySQL 连接可能因为网络超时、服务器重启等原因断开。使用前需要 ping：

```c
// MySQL 自带的 ping 命令
int db_conn_alive(db_connection *conn) {
    // 不要依赖隐式自动重连：重连可能丢失会话状态、事务和临时对象。
    if (mysql_ping(conn->mysql) != 0) {
        // 连接真的断了
        return 0;
    }
    return 1;
}

// 或者在获取连接时执行一个轻量查询
int db_conn_validate(db_connection *conn) {
    if (mysql_query(conn->mysql, "SELECT 1") != 0) {
        // 连接有问题，尝试重连
        if (mysql_ping(conn->mysql) != 0) {
            return 0;  // 重连失败
        }
    }
    return 1;
}
```

# MySQL 连接池的关键参数

```c
// 连接池初始化
db_connection_pool *pool_create(const char *host, int port,
                                 const char *user, const char *pass,
                                 const char *db, int min, int max) {
    db_connection_pool *pool = calloc(1, sizeof(db_connection_pool));
    strcpy(pool->host, host);
    pool->port = port;
    strcpy(pool->user, user);
    strcpy(pool->pass, pass);
    strcpy(pool->db, db);
    pool->min_size = min;
    pool->max_size = max;
    pool->connections = calloc(max, sizeof(db_connection));

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    // 预先创建 min_size 个连接
    for (int i = 0; i < min; i++) {
        db_connection *conn = &pool->connections[pool->current_size++];
        conn->mysql = create_mysql_connection(pool);
        conn->is_alive = 1;
        pool->idle_count++;
    }
    return pool;
}
```

**连接池参数最佳实践：**

```ini
initial_size = 5       # 初始连接数：预热启动
min_idle = 5           # 最小空闲：保证突发流量
max_active = 20-50     # 最大活跃连接数：根据 CPU 和数据库规格调整
max_wait = 30ms        # 获取连接最大等待时间
timeout = 30s          # 连接超时时间（MySQL wait_timeout 默认 8h）
```

# MySQL Server 端配置

```ini
max_connections = 500           # 最大连接数（默认 151）
wait_timeout = 300              # 空闲连接超时（秒，建议 300-600）
interactive_timeout = 300       # 交互式连接超时
thread_cache_size = 64          # 线程缓存

```

# 多线程安全

```c
db_connection *pool_get_connection(db_connection_pool *pool) {
    pthread_mutex_lock(&pool->mutex);

    // 查找空闲连接
    for (int i = 0; i < pool->current_size; i++) {
        if (pool->connections[i].is_alive && !pool->connections[i].in_use) {
            // 检查连接是否有效
            if (!db_conn_alive(&pool->connections[i])) {
                // 重建连接
                mysql_close(pool->connections[i].mysql);
                pool->connections[i].mysql = create_mysql_connection(pool);
                pool->connections[i].is_alive = 1;
            }
            pool->connections[i].in_use = 1;
            pool->idle_count--;
            pthread_mutex_unlock(&pool->mutex);
            return &pool->connections[i];
        }
    }

    // 无空闲连接但未达上限 → 新建
    if (pool->current_size < pool->max_size) {
        int idx = pool->current_size++;
        pool->connections[idx].mysql = create_mysql_connection(pool);
        pool->connections[idx].is_alive = 1;
        pool->connections[idx].in_use = 1;
        pthread_mutex_unlock(&pool->mutex);
        return &pool->connections[idx];
    }

    // 已达上限 → 等待
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += MAX_WAIT_SECONDS;
    int ret = pthread_cond_timedwait(&pool->cond, &pool->mutex, &ts);
    pthread_mutex_unlock(&pool->mutex);

    if (ret == ETIMEDOUT) return NULL;
    return pool_get_connection(pool);  // 递归重试
}
```

> [!tip]- **工程要点**：池大小、等待时间、`wait_timeout` 与服务端 `max_connections` 都必须按数据库容量和应用并发压测决定。归还连接前必须回滚未完成事务、清理会话状态；断线后显式重建连接，不依赖隐式自动重连。优先选维护活跃、与当前客户端库兼容的实现，而不是只因名称推荐某个连接池。

# 30 秒回答

**连接池为什么不能只调大？** 每个连接都消耗数据库端内存、线程/调度与锁竞争预算；池过大可能让数据库更慢。先设上限和超时，归还时清理事务状态，断线后显式建新，并用指标验证排队与数据库负载。

---

MySQL基础连接配置见 → [MySQL Basics (MySQL 基础)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/02-MySQL%20Basics%20(MySQL%20基础).md)

# 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

# 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **06-Connection Pool (连接池)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
