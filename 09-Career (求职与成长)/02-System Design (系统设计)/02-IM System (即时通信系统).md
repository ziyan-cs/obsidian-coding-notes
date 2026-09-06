---
tags:
  - distributed/system
status: 🌱
---

> [!abstract] 核心考点：> WebSocket 长连接管理 + 消息可靠投递（推拉模式）+ 消息序号的全局一致性

# 30 秒回答

**核心结论**：核心考点：> WebSocket 长连接管理 + 消息可靠投递（推拉模式）+ 消息序号的全局一致性


# 需求分析

## 功能需求

| 需求 | 说明 |
|------|------|
| 单聊 | 用户 A → 用户 B 的私信，保证送达 |
| 群聊 | 多人群组消息，支持 2000 人大群 |
| 在线状态 | 实时感知好友是否在线 |
| 消息回执 | 已送达 / 已读 / 失败通知 |
| 历史消息 | 支持漫游，设备间同步 |
| 多端同步 | 手机、PC、Web 同时在线，消息同步 |
| 文件/图片 | 附件消息的上传和下载 |

## 非功能需求

| 维度 | 指标 |
|------|------|
| DAU | 1 亿 |
| 日均消息量 | 50 亿条（人均 50 条） |
| 写 QPS | 峰值 600K/s |
| 读 QPS | 历史消息查询 100K/s |
| 消息延迟 | P99 < 200ms（同城在线） |
| 可用性 | 99.99%（消息不允许丢） |
| 消息不丢 | 写入持久化后才返回 ACK |

# 数据量估算

| 项目 | 计算 |
|------|------|
| 每日消息数 | 1 亿 DAU × 50 条 = 50 亿条 |
| 消息大小 | 1 条 ≈ 500 bytes（含 metadata） |
| 每天存储增量 | 50 亿 × 500B ≈ 250 TB |
| 每月存储 | 250 TB × 30 ≈ 7.5 PB |
| 消息 ID 需求 | 50 亿/天 ≈ 5800/s，Snowflake 完全胜任 |
| 在线连接数 | 峰值并发连接 ≈ 3 千万 |
| 连接内存 | 每条连接 ≈ 10 KB → 3000 万 × 10KB ≈ 300 GB |

> **关键结论**：IM 是典型的**写密集型 + 高并发连接**系统。存储是最大挑战，必须分库分表 + 冷热分离。

# 核心模型

```sql
-- 消息内容表（按 conversation_id 分表，1024 张）
CREATE TABLE message_{shard} (
    msg_id          BIGINT UNSIGNED NOT NULL COMMENT '全局唯一，Snowflake',
    conversation_id BIGINT UNSIGNED NOT NULL COMMENT '会话 ID（单聊/群聊）',
    sender_id       BIGINT UNSIGNED NOT NULL,
    msg_type        TINYINT     NOT NULL COMMENT '1:text 2:image 3:file 4:system',
    content         TEXT        NOT NULL,
    client_seq      BIGINT UNSIGNED NOT NULL COMMENT '客户端序列号（去重用）',
    server_seq      BIGINT UNSIGNED NOT NULL COMMENT '服务端递增序列号（排序用）',
    status          TINYINT     NOT NULL DEFAULT 0 COMMENT '0:正常 1:撤回 2:删除',
    created_at      DATETIME    NOT NULL,
    PRIMARY KEY (conversation_id, server_seq),
    INDEX idx_sender (sender_id, created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 会话表
CREATE TABLE conversation (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    type            TINYINT     NOT NULL COMMENT '1:单聊 2:群聊',
    name            VARCHAR(128) DEFAULT NULL COMMENT '群聊名称',
    owner_id        BIGINT UNSIGNED DEFAULT NULL,
    last_msg_id     BIGINT UNSIGNED DEFAULT 0,
    last_msg_time   DATETIME,
    created_at      DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- 群组成员表
CREATE TABLE group_member (
    group_id        BIGINT UNSIGNED NOT NULL,
    user_id         BIGINT UNSIGNED NOT NULL,
    role            TINYINT DEFAULT 0 COMMENT '0:member 1:admin 2:owner',
    join_time       DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (group_id, user_id),
    INDEX idx_user (user_id)
);

-- 用户设备在线状态（Redis 实时）
-- KEY: online:{user_id}:{device_id} → value: {ws_conn_id, last_heartbeat}
-- KEY: online:{user_id} → SET of device_ids (TTL: 5 min, heartbeat refresh)
```

# 架构设计图

```text
┌──────────────────────────────────────────────────────────────────────────────────────┐
│  Client Layer                                                                        │
│  ┌────────────────┐  ┌────────────────┐  ┌────────────────┐                          │
│  │ iOS/Android App│  │   PC Client    │  │  Web Browser   │                          │
│  └────────────────┘  └────────────────┘  └────────────────┘                          │
└─────────────────────────────────┬────────────────────────────────────────────────────┘
                                  │ WebSocket / TCP
                                  ▼
┌──────────────────────────────────────────────────────────────────────────────────────┐
│  Access Layer (Long-connection Gateway)                                              │
│  ┌───────────────────────────────────────────────────────┐                           │
│  │  LVS / DNS Load Balancer                              │                           │
│  └──────────────────────┬────────────────────────────────┘                           │
│                         ▼                                                             │
│  ┌───────────────────────────────────────────────────────┐                           │
│  │  Connection Gateway Cluster (Stateless, Horizontal    │                           │
│  │  Scaling)                                             │                           │
│  └───────────────────────────────────────────────────────┘                           │
└─────────────────────────────────┬────────────────────────────────────────────────────┘
                                  │ gRPC Internal RPC
                                  ▼
┌──────────────────────────────────────────────────────────────────────────────────────┐
│  Logic Service Layer                                                                 │
│  ┌──────────────────┐    ┌──────────────────┐    ┌───────────────────────────────┐   │
│  │  Router Service  │───→│ Presence Service │    │ Group Service                 │   │
│  │  (Message Route) │    │ (Online Status)  │    │ (Group Membership Management) │   │
│  └────────┬─────────┘    └──────────────────┘    └──────────────┬────────────────┘   │
│           │                                                     │                    │
│           ▼                                                     │                    │
│  ┌──────────────────────────────────────────────────┐           │                    │
│  │  Message Logic (Dedup / Validation / Fanout)     │           │                    │
│  └────────┬─────────────────────────────────────────┘           │                    │
└───────────┼──────────────────────────────────────────────────────┼────────────────────┘
            │                                                      │
            ▼                                                      ▼
┌──────────────────────────────────────────────────────────────────────────────────────┐
│  Storage Layer                                                                       │
│                                                                                       │
│  ┌──────────────┐    ┌──────────────────┐    ┌──────────────────────────┐            │
│  │  Kafka       │───→│ Message Store    │───→│ TDSQL / Vitess (Sharded  │            │
│  │  (Buffer &   │    │ (Persistence)    │    │ Message Database)        │            │
│  │  Peak Shave) │    └──────────────────┘    └──────────────────────────┘            │
│  └──────────────┘                                                                   │
│                                                                                       │
│  ┌────────────────────────────────────────────┐    ┌──────────────────────────┐      │
│  │  Redis Cluster (Online Status / Sequence)  │    │ MySQL (Group Members /   │      │
│  └────────────────────────────────────────────┘    │ Relationship Graph)      │      │
│                                                    └──────────────────────────┘      │
└──────────────────────────────────┬───────────────────────────────────────────────────┘
                                   │
            ┌──────────────────────┼──────────────────────┐
            ▼                      ▼                      ▼
┌───────────────────────┐ ┌──────────────────┐ ┌──────────────────────┐
│  Offline & Push       │ │  Roaming & Search│ │  Sync Service        │
│                       │ │                  │ │  (Multi-device Sync) │
│  ┌─────────────────┐  │ │  ┌────────────┐  │ │        │             │
│  │ Push Service     │  │ │  │ Elastic-   │  │ │        ▼             │
│  │ (APNs / FCM)     │  │ │  │ search     │◄─┤ │  ┌────────────────┐ │
│  └─────────────────┘  │ │  │(Msg Search) │  │  │  │ Sync Cursor    │ │
│                       │ │  └────────────┘  │  │  │ Management     │ │
│  ┌─────────────────┐  │ └──────────────────┘  │  └────────────────┘ │
│  │ Offline Service  │  │                       │                    │
│  │ (Offline Msg     │  │                       │                    │
│  │  Index)          │  │                       │                    │
│  └────────┬─────────┘  │                       │                    │
│           ▼            │                       │                    │
│  ┌─────────────────┐   │                       │                    │
│  │ Redis (Offline   │   │                       │                    │
│  │ Msg Queue)       │   │                       │                    │
│  └─────────────────┘   │                       │                    │
└─────────────────────────┴───────────────────────┴────────────────────┘
```

## 消息投递流程

```text
Sender           Connection Gateway    Router Service     Receiver GW      Message Store
  │                      │                   │                │                │
  ├── 1. Send message   │                   │                │                │
  │   (conversation_id, │                   │                │                │
  │   content, client_seq)                   │                │                │
  │────────────────────→│                   │                │                │
  │                      ├── 2. Route       │                │                │
  │                      │   request        │                │                │
  │                      │─────────────────→│                │                │
  │                      │                   ├── 3. Persist  │                │
  │                      │                   │   message     │                │
  │                      │                   │   (get        │                │
  │                      │                   │   server_seq) │                │
  │                      │                   │───────────────→│                │
  │                      │                   │◄── 4. Return  ─┤                │
  │                      │                   │   msg_id,      │                │
  │                      │                   │   server_seq   │                │
  │                      │                   │                │                │
  │                      │                   ├── 5. Online    │                │
  │                      │                   │   push (Write  │                │
  │                      │                   │   & Check)     │                │
  │                      │                   │────────────────→│                │
  │                      │                   │                │                │
  │◄──── 6. Return ACK ──┤◄──────────────────┤                │                │
  │   (msg_id,           │                   │                │                │
  │   server_seq)        │                   │                │                │
  │                      │                   │                7. Message ACK   │
  │                      │                   │                │  (ACK or NAK)  │
  │                      │                   ├── 8. Delivery  │                │
  │◄──── (optional) ─────┤◄──────────────────┤   receipt      │                │
```

# 消息分发模型对比

| 模型 | 原理 | 优点 | 缺点 | 适用场景 |
|------|------|------|------|---------|
| **写扩散**（Fanout） | 发送者写一次，消息复制到每个接收者的收件箱 | 读扩散时 O(1)，读取快 | 群聊写放大严重 | 单聊、小群 |
| **读扩散**（Pull） | 发送者写到消息表，接收者拉取 | 写友好，存储少 | 读放大（需查群 + 查消息） | 大群（1000+） |
| **推拉结合** | 在线推送 + 离线拉取 | 兼顾两者 | 实现复杂 | **推荐方案** |

**最终策略**：
- **单聊**：写扩散（直接推送到接收方在线通道）
- **群聊 ≤ 500 人**：写扩散，逐个推送
- **群聊 > 500 人**：读扩散，写入 Group Timeline，成员拉取

# 关键难点与解决方案

## 1. 消息序号（Sequence）的全局有序

**问题**：分布式环境下如何保证消息的顺序性且不重复。

**方案**：两级序列号
- **server_seq**：每个 conversation 一个递增序列，Redis INCR 或 DB 自增段。用于消息排序和断点续传
- **msg_id**：全局唯一 Snowflake ID，用于去重和消息定位

```
消息排序：按 (conversation_id, server_seq) 升序
断点续传：客户端携带 last_server_seq，服务端返回后续消息
```

## 2. 消息可靠投递

**问题**：网络断开、服务重启、进程崩溃都可能导致消息丢失。

**方案**：ACK + 重试 + 幂等

```
发送         网关             存储
  │─── Msg ──→│─── Store ───→│
  │           │←── msg_id ──│
  │←── ACK ──│               │
  │─── Ack ──→│               │  ← 确认存储完成

超时未 ACK：客户端重试，服务端通过 (sender_id, client_seq) 去重
```

## 3. 多端同步

**问题**：手机、PC、Web 同时在线，消息不能重复推送。

**方案**：Sync Cursor（同步游标）
- 每个设备维护一个 `sync_cursor`（即已同步的最大 server_seq）
- 设备上线时，从 sync_cursor+1 开始拉取未同步消息
- 消息推送时带上 server_seq，设备按 seq 去重

## 4. 群聊写放大

**问题**：2000 人群每人发一条 → 2000 次写扩散 = 400 万次写入。

**方案**：
- 500 人以下群：写扩散（写入所有成员的 Timeline）
- 500 人以上群：读扩散（写入 Group Timeline，成员 Pull）
- 分层混合策略，按群活跃度动态切换

## 5. 连接网关的水平扩展

**问题**：WebSocket 有状态，连接固定在某台机器。

**方案**：
- Gateway 无状态化：所有业务数据存 Redis，Gateway 只做连接保持
- 客户端重连时，DNS/LVS 分配到任意 Gateway（新连接重新注册到 Router Service）
- 消息推送时，Router 查用户当前所在 Gateway，转发过去

# 面试追问

| 追问方向 | 参考回答 |
|---------|---------|
| **2 万人超级群怎么设计？** | 读扩散 + Topic 分区。群消息写入 Group Timeline（Kafka Topic），成员各自记录消费 offset。实际微信/QQ 都限制群人数上限（2000~5000） |
| **如何实现已读/未读功能？** | 每条消息维护一个已读用户 Bitmap（用 Redis Bitmap），每群一个 Bitmap key。更新操作用 BITOP，已读用户查询 O(1)。数据量大时可分片 |
| **消息撤回的实现原理？** | 发送者发起撤回请求 → 校验撤回窗口（通常 2 分钟）→ 更新 message.status = 1（逻辑删除）→ 推送撤回通知到在线接收方 + 离线时同步删除 |
| **如何保证消息不重复？** | Client 发送时生成唯一 client_seq（递增 + 设备号），服务端以 (sender_id, client_seq) 做幂等校验。同一 client_seq 的消息直接返回已保存的 msg_id |
| **消息搜索（全文检索）怎么设计？** | 通过 Canal 订阅 MySQL Binlog 同步到 Elasticsearch，按 conversation_id 和 sender_id 建立索引。搜索时查 ES 获取 msg_id 列表，再到 DB 拉取完整内容 |
| **Redis 存储在线状态的容量？** | 1 亿用户 × 2 设备 × 50 bytes（key） + 20 bytes（value）≈ 1.4 GB。加上设备 mapping 和心跳数据，总计约 5~10 GB，单集群轻松承载 |
| **消息表分片键怎么选？** | 按 `conversation_id` 哈希分片，保证同一会话所有消息落入同一分片，天然支持范围查询（会话内翻页）。查询用户历史消息时需广播到所有分片 |

IM 系统网络编程基础详解见 → [WebSocket Protocol](05-Network%20Programming%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/10-WebSocket%20Protocol%20(WebSocket协议).md) · [Reactor & Proactor Pattern](05-Network%20Programming%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐.md)



# 零基础阅读路径

先将本页结论转换成自己的 30 秒表达；再补一个具体约束和项目证据；最后练习反例与取舍，避免把速记当成理解。

# 常见误区

- 只背标准答案，不把结论放进真实约束、取舍和项目证据中，表达会显得空泛。
- 不计时复述、不追问反例，也不通过项目日志或代码核对，就会形成虚假熟练。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-IM System (即时通信系统)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
