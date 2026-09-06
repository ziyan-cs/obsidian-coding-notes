---
tags: [vault/moc, backend/systems]
status: learning
review_due: 2026-09-13
confidence: 1
verified: 2026-09-06
---

# Backend Systems MOC — 后端系统导航

> [!abstract] 后端系统知识回答同一个问题：请求如何穿过操作系统、网络、存储与中间件，并在失败和压力下仍然可控。

## 推荐顺序

1. [[00-Linux MOC (Linux 导航)|Linux]]：进程、I/O、文件描述符、事件驱动。
2. [[00-Network MOC (网络编程导航)|网络]]：TCP、HTTP、Socket 与服务器模型。
3. [[00-Database MOC (数据库导航)|数据库]]：SQL、索引、事务、复制与查询优化。
4. [[00-Distributed MOC (分布式导航)|分布式与中间件]]：缓存、消息队列、代理与一致性。

## 使用方式

- 排查代码行为时：先从 Linux / 网络找运行时模型。
- 设计数据流时：从数据库、缓存、消息队列选组件并记录取舍。
- 遇到性能结论时：先定义指标，再用压测与观测验证，不背固定数字。

## 关联

- [[00-C++ Backend Core Map (后端能力地图)]]
- [[00-Project Map (项目地图)]]
