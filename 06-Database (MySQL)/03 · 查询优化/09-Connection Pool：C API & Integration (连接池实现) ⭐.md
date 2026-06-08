---
tags:
  - database/optimization
status: 🌱
---

> **核心考点**：MySQL C API 连接池实现、连接复用与并发控制、连接池参数调优

## MySQL C API 基础

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

## 连接池核心设计

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

## 连接有效性检查

MySQL 连接可能因为网络超时、服务器重启等原因断开。使用前需要 ping：

```c
// MySQL 自带的 ping 命令（自动重连）
int db_conn_alive(db_connection *conn) {
    // mysql_ping 如果连接断开，会自动重连（如果 CLIENT_RECONNECT 已启用）
    // 但自动重连不推荐在高并发场景使用（可能导致死锁）
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

## MySQL 连接池的关键参数

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
# 应用层建议值
initial_size = 5       # 初始连接数：预热启动
min_idle = 5           # 最小空闲：保证突发流量
max_active = 20-50     # 最大活跃连接数：根据 CPU 和数据库规格调整
max_wait = 30ms        # 获取连接最大等待时间
timeout = 30s          # 连接超时时间（MySQL wait_timeout 默认 8h）
```

## MySQL Server 端配置

```ini
# MySQL 配置，配合连接池使用
max_connections = 500           # 最大连接数（默认 151）
wait_timeout = 300              # 空闲连接超时（秒，建议 300-600）
interactive_timeout = 300       # 交互式连接超时
thread_cache_size = 64          # 线程缓存
# 如果应用频繁短连接，thread_cache_size 要设大些

# 连接过多时的表现：
#   Too many connections → 调大 max_connections 或减小连接池
#   Aborted connections → 网络问题或认证失败
```

## 多线程安全

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

> **工程要点**：数据库连接池的坑：1) MySQL `wait_timeout` 默认 8 小时，连接池中的空闲连接可能被 MySQL 服务端断开——所以获取连接时必须先做 `mysql_ping` 或 `SELECT 1` 验证；2) 连接泄露——应用获取连接后必须保证在 finally 中 release；3) 事务未提交——获取连接时注意是否需要回滚残留事务。推荐 C++ 项目使用成熟的连接池库如 AliSQL 的 ConnectionPool 或自研基于 `MYSQL` 句柄池的封装。

---



MySQL基础连接配置见 → [MySQL Basics (MySQL 基础)](/06-Database%20(MySQL)/01%20·%20SQL基础/02-MySQL%20Basics%20(MySQL%20基础).md)
