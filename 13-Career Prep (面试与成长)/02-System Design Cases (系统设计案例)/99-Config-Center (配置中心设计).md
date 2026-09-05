---
tags:
  - distributed/system
status: 🌱
---

> [!important] **核心考点**
> etcd / ZooKeeper 的 Watch 机制 + 客户端长轮询 + 配置版本管理与灰度发布

> 本文为面试/系统设计参考；动手实现与证据见 [[12-Backend Projects (后端项目)/05-Config Center (配置中心)|Config Center 项目]]。

## 需求分析

### 功能需求

| 需求 | 说明 |
|------|------|
| 配置存储 | 支持 KV 格式、YAML / JSON / Properties 多种格式 |
| 实时推送 | 配置变更后秒级推送到客户端 |
| 版本管理 | 每次变更生成新版本，支持回滚 |
| 环境隔离 | dev / staging / prod 环境数据隔离 |
| 命名空间 | 按业务线 / 应用分组隔离 |
| 权限控制 | 配置读写权限分离，敏感配置加密存储 |
| 灰度发布 | 按 IP / 机器 / 比例灰度推送 |
| 变更审计 | 记录谁在什么时间修改了什么配置 |

### 非功能需求

| 维度 | 指标 |
|------|------|
| 配置总数 | 10 万 ~ 100 万条 |
| 写 QPS | 低（日常变更 ~ 10 QPS，发布期间 ~ 200 QPS） |
| 读 QPS | 中（客户端启动 + Watch 回调 ~ 10 K QPS） |
| 推送延迟 | 配置变更 → 客户端感知 < 1s（P99） |
| 可用性 | 99.999%（配置中心挂了 → 全站不可用，必须极致高可用） |
| 一致性 | 最终一致即可，但变更顺序必须可追溯 |
| 客户端数量 | 10 万 + 实例 |

## 数据量估算

| 项目 | 计算 |
|------|------|
| 配置条目 | 10 万条，每条平均 2 KB → 200 MB |
| 版本历史 | 10 万 × 100 版本 × 2 KB ≈ 20 GB（可定期归档） |
| 客户端连接 | 10 万实例，每 30s 长轮询一次 → 心跳 QPS ≈ 3000/s |
| 推送变更 | 全量推送时 10 万连接同时通知 → 避免惊群效应 |

> **关键洞察**：配置中心数据量极小，核心挑战是**一致性 + 实时推送 + 高可用**，而非存储容量。

## 核心模型

```sql
-- 配置条目表
CREATE TABLE config_item (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    env             VARCHAR(16)     NOT NULL COMMENT 'dev/staging/prod',
    namespace       VARCHAR(64)     NOT NULL COMMENT '业务线/应用',
    group_name      VARCHAR(64)     NOT NULL COMMENT '配置分组',
    config_key      VARCHAR(255)    NOT NULL COMMENT '配置键',
    config_value    TEXT            NOT NULL COMMENT '配置值（加密后存储）',
    format          VARCHAR(16)     DEFAULT 'text' COMMENT 'text/json/yaml/properties',
    version         BIGINT UNSIGNED NOT NULL DEFAULT 1 COMMENT '当前版本号',
    status          TINYINT         NOT NULL DEFAULT 1 COMMENT '0:禁用 1:启用',
    is_encrypted    TINYINT         NOT NULL DEFAULT 0 COMMENT '是否加密存储',
    tags            VARCHAR(512)    DEFAULT NULL COMMENT '标签，用于灰度规则',
    created_by      VARCHAR(64)     NOT NULL,
    created_at      DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at      DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_config (env, namespace, config_key),
    INDEX idx_env_namespace (env, namespace)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 配置版本历史表
CREATE TABLE config_history (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    config_item_id  BIGINT UNSIGNED NOT NULL,
    version         BIGINT UNSIGNED NOT NULL,
    config_value    TEXT            NOT NULL,
    change_comment  VARCHAR(512)    DEFAULT NULL,
    created_by      VARCHAR(64)     NOT NULL,
    created_at      DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_config_version (config_item_id, version)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 应用实例注册表（客户端心跳上报）
-- 也可以直接用 Redis 或 etcd 存储，无需 MySQL
-- Redis HASH: instance:{app_id}:{instance_id}
-- 字段: ip, port, version, status, last_heartbeat
```

## 架构设计图

```text
┌──────────────────────────────────────────────────────────────────────────────────────┐
│  Operations Management                                                               │
│  ┌──────────────┐    ┌─────────────────┐    ┌──────────────────────────────────┐     │
│  │ Admin Web UI │───→│ Admin API       │───→│ Permission Check & Audit Logging│     │
│  └──────────────┘    │ Service         │    └──────────────────────────────────┘     │
│                      └─────────────────┘                                            │
└──────────────────────────────────┬───────────────────────────────────────────────────┘
                                   │
                                   ▼
┌──────────────────────────────────────────────────────────────────────────────────────┐
│  Configuration Storage                                                               │
│                                   ┌──────────────────────────────────────┐           │
│  ┌────────────────┐               │  Config Core Service (Push Logic)    │           │
│  │ MySQL          │◄──────────────┤                                      │           │
│  │ (Primary Store │               └──────┬───────────────────────┬───────┘           │
│  │  Persistence)  │                      │                       │                   │
│  └────────────────┘                      ▼                       ▼                   │
│                                  ┌──────────────────┐  ┌────────────────────┐         │
│                                  │ etcd / ZooKeeper │  │ Redis (Hot Config │         │
│                                  │ (Strong           │  │ Cache, Accelerate │         │
│                                  │  Consistency +    │  │ Reads)            │         │
│                                  │  Watch Mechanism) │  └────────────────────┘         │
│                                  └──────────────────┘                                 │
└──────────────────────────────────┬───────────────────────────────────────────────────┘
                                   │
              ┌────────────────────┼────────────────────┐
              ▼                    ▼                    ▼
┌─────────────────────┐  ┌─────────────────┐  ┌─────────────────────┐
│  Push Channel       │  │ Canary Release  │  │ Monitoring          │
│                     │  │                 │  │                     │
│  ┌───────────────┐  │  │ ┌─────────────┐ │  │ ┌───────────────┐  │
│  │ Long Polling  │  │  │ │ Gray Engine │ │  │ │ Change Event  │  │
│  │ Server (HTTP  │  │  │ └──────┬──────┘ │  │ │ Monitor       │  │
│  │ Long Polling) │  │  │        │        │  │ └───────┬───────┘  │
│  └───────┬───────┘  │  │  ┌─────┴─────┐  │  │         │          │
│          │          │  │  │ IP List /  │  │  │  ┌──────┴──────┐  │
│  ┌───────────────┐  │  │  │ Machine    │  │  │  │ Config      │  │
│  │ WebSocket     │  │  │  │ Tag /      │  │  │  │ Change      │  │
│  │ Server (Real- │  │  │  │ Percentage │  │  │  │ Alert       │  │
│  │ time Push)    │  │  │  └────────────┘  │  │  │ (Sensitive   │  │
│  └───────┬───────┘  │  │                 │  │  │  Configs)    │  │
│          │          │  │  ┌─────────────┐ │  │  └──────────────┘  │
│          │          │  │  │ Batch Push +│ │  │                     │
│          │          │  │  │ Auto Rollback│ │  │  ┌──────────────┐  │
│          │          │  │  └─────────────┘ │  │  │ Audit Log /  │  │
│          │          │  └─────────────────┘  │  │  │ Operation   │  │
│          │          │                       │  │  │ Records     │  │
│          │          │                       │  │  └──────────────┘  │
│          ▼          │                       │                     │
│  ┌──────────────────────────────────────────┐                     │
│  │ Client SDK                              │                     │
│  │  ┌──────────────────┐  ┌──────────────┐ │                     │
│  │  │ Application      │  │ Application  │ │                     │
│  │  │ Instance 1       │  │ Instance 2   │ │                     │
│  │  └──────────────────┘  └──────────────┘ │                     │
│  │  ┌──────────────────┐                    │                     │
│  │  │ Application      │                    │                     │
│  │  │ Instance 3       │                    │                     │
│  │  └──────────────────┘                    │                     │
│  └──────────────────────────────────────────┘                     │
└───────────────────────────────────────────────────────────────────┘
```

### 配置变更推送流程

```text
Admin/Dev             Admin API              MySQL               etcd              Push Server          App Instance
  │                       │                    │                 │                    │                    │
  ├── 1. Modify config   │                    │                 │                    │                    │
  │   (key, value)       │                    │                 │                    │                    │
  │─────────────────────→│                    │                 │                    │                    │
  │                       ├── 2. Permission   │                 │                    │                    │
  │                       │   check           │                 │                    │                    │
  │                       │  (internal)       │                 │                    │                    │
  │                       ├── 3. Write config │                 │                    │                    │
  │                       │   (version+1)     │                 │                    │                    │
  │                       │──────────────────→│                 │                    │                    │
  │                       ├── 4. Update etcd  │                 │                    │                    │
  │                       │   key             │                 │                    │                    │
  │                       │   (/config/env/ns │                 │                    │                    │
  │                       │    /key)          │                 │                    │                    │
  │                       │───────────────────────────────────→│                    │                    │
  │                       │                    │                 ├── 5. Watch event  │                    │
  │                       │                    │                 │   triggers        │                    │
  │                       │                    │                 │───────────────────→│                    │
  │                       │                    │                 │                    ├── 6. Push change  │
  │                       │                    │                 │                    │   notification    │
  │                       │                    │                 │                    │   (key,            │
  │                       │                    │                 │                    │    new_version)    │
  │                       │                    │                 │                    │──────────────────→│
  │                       │                    │                 │                    │                    │
  │                       │                    │                 │                    │ 7. Load new config│
  │                       │                    │                 │                    │    locally (hot    │
  │                       │                    │                 │                    │    reload)         │
  │                       │                    │                 │                    │◄───────────────────┤
  │                       │                    │                 │                    │ 8. ACK (new_      │
  │                       │                    │                 │                    │    version)        │
  │                       │                    │                 │                    │◄───────────────────┤
  │                       │                    ├── 9. Record ACK │                    │                    │
  │                       │                    │   status        │                    │                    │
  │                       │                    │   (optional;    │                    │                    │
  │                       │                    │   required for  │                    │                    │
  │                       │                    │   canary)       │                    │                    │
  │◄──── 10. Push ────────┤◄───────────────────┤                 │                    │                    │
  │         complete      │                    │                 │                    │                    │
  │         notification   │                    │                 │                    │                    │
```

## 配置推送机制对比

| 方式 | 实时性 | 资源消耗 | 实现复杂度 | 推荐场景 |
|------|--------|---------|-----------|---------|
| **etcd Watch** | 实时 | 低（长连接） | 中等 | 核心配置，强一致需求 |
| **HTTP 长轮询** | 准实时（~30s） | 中 | 简单 | 大规模客户端，兼容性好 |
| **WebSocket** | 实时 | 低 | 中等 | 需要双向通信的场景 |
| **客户端定时拉取** | 分钟级 | 中 | 最简单 | 非敏感配置，容忍延迟 |

**推荐组合**：etcd Watch（配置存储层一致）+ 长轮询（客户端回退方案）

客户端的推送策略：
```
1. 优先 WebSocket 实时推送
2. WS 断开 → 长轮询兜底（30s 间隔）
3. 长轮询也失败 → 定时拉取（60s 间隔，指数退避）
```

## 关键难点与解决方案

### 1. 客户端配置热更新

**问题**：Java / Go / C++ 应用中，配置变更后如何不重启进程生效。

**方案**：
- **Java**：`@RefreshScope` + Spring Cloud Bus 刷新 Bean
- **Go**：定时检测配置版本号，Compare-and-Swap 原子替换全局配置指针
- **C++**：`std::atomic<Config*>` + 双缓冲，读线程无锁切换

**通用模式**：配置代理层 + 版本号比较 + 无锁切换

```cpp
// C++ 无锁热更新示意
std::atomic<Config*> g_config;

void reload_if_changed(int64_t new_version) {
    auto old = g_config.load(std::memory_order_acquire);
    if (old->version < new_version) {
        auto new_cfg = new Config(*old); // 拷贝
        new_cfg->update_from_remote();    // 合并增量
        g_config.store(new_cfg, std::memory_order_release);
        // 延迟删除旧配置（RCU 风格）
        old_pending_delete.push(old);
    }
}

// 业务线程：读取配置
auto cfg = g_config.load(std::memory_order_acquire);
int timeout = cfg->rpc_timeout_ms;
```

### 2. 灰度发布

**问题**：配置改错导致线上故障，需要分批生效、自动回滚。

**灰度流程**：
```
灰度规则（IP白名单 / 机器标签 / 百分比）
    ↓
先推一台 → 观察 5 分钟（监控指标无异常）
    ↓
推 10% → 观察 10 分钟
    ↓
推 50% → 观察 10 分钟
    ↓
全量推送
    ↓
如果任意阶段监控告警 → 自动回滚到上一版本
```

### 3. 共享配置的三级缓存

**问题**：10 万客户端同时读取配置，不能全部打到 etcd。

**三级缓存架构**：

| 层级 | 存储 | 作用 |
|------|------|------|
| L1 | 客户端本地内存 | 零延迟读取，应用实际取值处 |
| L2 | 客户端本地文件 | 进程重启后快速恢复，减少冷启动拉取 |
| L3 | 服务端（etcd/Redis） | 版本变更的权威来源，变更时推送通知 |

### 4. 惊群效应（全量推送）

**问题**：修改一条公共配置，10 万客户端同时回源拉取 → 服务瞬间被打爆。

**方案**：
- 推送通知中带上增量数据的版本号，客户端判断本地版本号是否落后
- 分布式限流：服务端用令牌桶控制每秒回源的客户端数量
- 随机延迟：客户端收到通知后，在 [0, 5s] 随机等待后再拉取

### 5. 敏感配置加密

**问题**：数据库密码、API Key 如果明文存储 → 内网泄露即可用。

**方案**：
- **存储加密**：AES-256-GCM 加密后存 DB，密钥由 KMS 管理
- **传输加密**：TLS 传输
- **访问控制**：配置中心管理员手动授权，细粒度到单条配置的读权限
- **审计日志**：敏感配置的所有读取操作记录 + 告警
- **运行时解密**：配置推送后客户端在内存中解密，不在日志中打印明文

## 面试追问

| 追问方向 | 参考回答 |
|---------|---------|
| **etcd 和 ZooKeeper 选哪个？为什么？** | etcd 使用 Raft + gRPC，API 简洁（CRUD + Watch），Go 实现运维友好。ZK 是更成熟的选择，但 API 较底层（临时节点需手动管理心跳）。现代云原生项目多选 etcd（K8s 的选择），但需要评估自建 vs 云服务 |
| **配置回滚时怎么保证一致性？** | 回滚 = 把历史版本的 value 重新写入 + version+1。如果回滚期间有新变更，版本号不会冲突（每次递增）。回滚也是一个变更，同样走灰度发布流程。关键是回滚操作本身也要被审计记录 |
| **如何保证配置不会"改丢"？** | ① MySQL 存储全量历史版本；② 变更写入 etcd 必须先写 DB（Write-Ahead 模式）；③ etcd 自身维护 Revision 号可做校验；④ 双写一致性通过事务 ID 保证 |
| **配置中心崩溃时，客户端还能工作吗？** | 能。客户端内存 + 本地文件保留最新配置快照，服务端崩溃期间客户端用缓存继续工作。恢复后重新建立 Watch，同步增量变更。这要求配置本身的正确性——配置中心崩溃不影响已加载的配置 |
| **客户端怎么监听配置变更？底层原理？** | ① SDK 与 Push Server 建立长连接（WebSocket / gRPC Stream）；② 维护一个监听列表（感兴趣的 key pattern）；③ 服务端配置变更时推送 (key, version) 通知；④ 客户端根据 version 判断是否需要拉取；⑤ 拉取到新配置后执行热更新回调 |
| **如何区分"配置不存在"和"配置值为空"？** | 服务端返回明确的状态码：NOT_FOUND vs EMPTY_VALUE。客户端 SDK 中返回 Optional\<String\>，业务层按需处理 |
| **配置中的 DB 密码怎么轮换？** | 通过配置中心定时任务自动轮换，新密码加密后写入配置中心。依赖密码的客户端通过 Watch 接收到新密码，在线切换连接池。AB 测试：保留旧连接直到旧连接耗尽 |
| **推拉结合的实现细节？** | 先推（Push Server 通知客户端版本变化），后拉（客户端主动拉取新值）。推不可靠时靠拉兜底。客户端定时在后台拉取全量版本号对比（类似于 DNS 的 SOA 查询），发现不一致时拉取增量 |

配置中心依赖的分布式共识协议详解见 → [Leader Election (Raft)](07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c1-Leader%20Election%20(领导者选举)%20⭐.md) · [Log Replication (Raft)](07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c2-Log%20Replication%20(日志复制)%20⭐.md)
