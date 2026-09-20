---
study_stage: backlog
tags: [go/rpc, protocol/protobuf, api/evolution]
---

> [!abstract] 学习定位
> RPC 把网络调用包装成接口，但不会消除超时、重试、部分失败与兼容问题；Protobuf schema 是跨服务长期契约，字段编号一旦发布就必须谨慎演进。

# 先看远程调用的真实边界

本地函数调用失败时通常知道是否执行；RPC timeout 只说明调用方没及时收到结果，服务端可能未开始、执行中、已经成功或响应丢失。所有有副作用的方法必须考虑幂等键、查询状态或业务唯一约束。

~~~text
client -> DNS/connect/TLS -> load balancer -> server queue
       -> decode -> handler -> database/downstream -> encode -> response
~~~

deadline 沿调用链传播，并为清理和返回预留预算。重试只针对明确瞬态错误，限制次数与总时间，并使用 jitter；多层自动重试会放大流量。

# Protobuf 契约

~~~proto
syntax = "proto3";

message CreateUserRequest {
  string request_id = 1;
  string name = 2;
}

message User {
  int64 id = 1;
  string name = 2;
}
~~~

字段编号是 wire identity。删除字段后使用 `reserved` 保留编号和旧名称，不把编号分配给新含义。新增可选字段通常向后兼容；改变字段类型、语义、单位或 required-like 业务约束可能破坏旧客户端。

proto3 默认值与“字段未出现”可能难以区分，需要存在性时使用支持 presence 的字段设计、wrapper 或显式状态。生成代码中的语言零值不等于业务有效值，服务端仍需校验。

# 服务方法与错误模型

方法粒度围绕业务动作，不把数据库表 CRUD 原样暴露。错误分为无效输入、未认证、无权限、不存在、冲突、资源耗尽、暂时不可用和内部错误；映射为稳定 status，同时保留服务端错误链与 request/trace ID。

不要把内部 SQL、堆栈或敏感对象写进客户端错误。可重试性由明确状态和方法幂等性共同决定，不能只看“网络错误”。

# Streaming、背压与取消

流式 RPC 适合连续结果、上传下载或双向会话，但需要处理慢消费者、消息大小、半途取消和资源上限。发送成功只表示交给传输层，不一定代表业务方持久化完成。

每个 stream 定义关闭协议：谁停止发送、如何表示正常结束、错误后是否允许重连、重连从哪个 offset 恢复。无界接收后启动 goroutine 会绕过 HTTP/2 流控并耗尽内存。

# 版本与兼容验证

- schema 进入版本控制，生成代码由固定工具链产生。
- CI 执行 breaking-change 检查，并保留旧客户端/新服务、新客户端/旧服务的契约测试。
- 服务先兼容读取新旧字段，再迁移客户端，最后停止写旧字段；删除要经过使用观测和完整发布周期。
- 日志和指标记录方法、状态、deadline、消息大小和版本，不使用用户 ID 等高基数标签。

# 实践

为 todo 服务定义 Create/Get/List 三个 RPC。给 Create 加 request ID 与数据库唯一约束；模拟服务端提交后响应丢失，验证客户端重试不会重复创建。随后新增可选 description 字段，用旧客户端与新服务互测，并演示为什么复用字段编号会错误解释数据。

> [!question]- 理解检查
> 1. RPC timeout 后，为什么不能断言服务端没有执行？
> 2. 删除 Protobuf 字段后为什么还要 reserved 编号？
> 3. 流式 RPC 中慢消费者如何把压力传回服务端？

> [!info]- 官方参考
> - [Protocol Buffers Programming Guides](https://protobuf.dev/programming-guides/)
> - [gRPC Guides](https://grpc.io/docs/guides/)
> - [Go Generated Code Guide](https://protobuf.dev/reference/go/go-generated/)
