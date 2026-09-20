---
study_stage: backlog
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。


> [!note] 本节重点：MySQL C API 连接池实现、连接复用与并发控制、连接池参数调优

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

## 多线程安全的边界

教学原型至少要把“空闲、借出、损坏、关闭”表示为明确状态，并用条件变量的谓词循环等待。不能在唤醒后递归调用获取函数：虚假唤醒和持续竞争会增长调用栈，也会重置或模糊原本的超时预算。创建连接等慢操作还应避免长期占用全局互斥锁。

连接对象必须包含可验证的所有权信息；若代码使用 `in_use`，结构体中就必须有对应成员，并保证它只在锁保护下改变。更稳妥的 C++ 接口会返回 RAII lease，在析构时归还或销毁连接，防止调用方忘记释放。
> [!tip]- **工程要点**：池大小、等待时间、`wait_timeout` 与服务端 `max_connections` 都必须按数据库容量和应用并发压测决定。归还连接前必须回滚未完成事务、清理会话状态；断线后显式重建连接，不依赖隐式自动重连。优先选维护活跃、与当前客户端库兼容的实现，而不是只因名称推荐某个连接池。

> [!summary] 核心摘要
>
> **连接池为什么不能只调大？** 每个连接都消耗数据库端内存、线程/调度与锁竞争预算；池过大可能让数据库更慢。先设上限和超时，归还时清理事务状态，断线后显式建新，并用指标验证排队与数据库负载。
>
> ---
>

> [!info]- 延伸阅读
> - 下一步：[11-Replication (复制)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/11-Replication%20(复制).md)
## 连接池管理的是稀缺状态

数据库连接不是一个普通指针。它包含认证会话、事务状态、隔离级别、临时表、会话变量与服务端资源。连接池的职责是控制这些有状态资源的生命周期，而不仅是避免重复 `connect()`。

```text
creating -> idle -> borrowed -> validating/resetting -> idle
                    |                 |
                    +---- broken -----+-> closing -> closed
```

一次借还协议至少应定义：

- 借用等待的 deadline 与取消信号；
- 池关闭时等待者如何被唤醒；
- 连接断开或协议错误后是否直接销毁；
- 归还前如何回滚未提交事务、恢复 autocommit 与会话变量；
- 最大存活时间、空闲时间和服务端超时如何协调；
- 谁拥有连接句柄，如何防止重复归还和借出后继续使用。

生产代码优先使用经过并发和故障验证的驱动/连接池。手写版本适合学习状态机，但不能只在 happy path 下证明正确。

## 并发设计要点

等待者应通过条件变量或信号量循环检查谓词，并携带绝对 deadline。不要在超时唤醒后递归调用 `get_connection()`：高竞争下会增长调用栈，而且让超时预算难以维持。

```cpp
std::unique_lock lock(mu_);
if (!available_.wait_until(lock, deadline, [&] {
        return closing_ || !idle_.empty();
    })) {
    return timeout_error;
}
if (closing_) return pool_closed_error;
auto conn = take_idle_connection();
```

健康检查也要分层：

- 借出前每次 `ping` 能发现部分断链，但会给每个请求增加往返。
- 空闲连接可由后台任务抽样验证。
- 真正执行 SQL 时仍可能失败，调用者必须处理连接级错误与事务结果不确定性。

因此健康检查不能消除错误处理，只能改变发现故障的时机。

## 容量不是越大越好

连接池上限受数据库可承受并发、应用实例数和请求预算共同约束。若有 `N` 个应用实例，每个实例池上限为 `P`，理论连接需求约为 `N × P`，还要给运维、迁移和故障切换预留余量。过大的池会把应用排队转移为数据库争用，使延迟更差。

从负载测试中观察：

- active、idle、creating 连接数；
- 获取等待时间分位数和超时次数；
- 连接创建、验证与销毁次数；
- 查询延迟与事务时长；
- 数据库 CPU、锁等待、线程/会话上限。

池等待时间显著增长时，要区分连接上限过小、查询变慢、事务泄漏和数据库容量不足，不能直接调大池。

## 故障验证

至少注入以下场景：数据库重启、网络半开、认证轮换、事务中断、池关闭时仍有借出连接、等待超时与连接泄漏。测试不仅断言返回错误，还要验证无死锁、无重复归还、事务不会串到下一个请求，指标能解释故障。

## 参考资料

- [MySQL C API](https://dev.mysql.com/doc/c-api/8.4/en/)
- [MySQL 8.4 Reference Manual: Server System Variables](https://dev.mysql.com/doc/refman/8.4/en/server-system-variables.html)
