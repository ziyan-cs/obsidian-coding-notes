---
study_stage: backlog
---

> [!abstract] 学习定位
> gRPC 用 Protobuf 契约与 HTTP/2 流承载服务间 RPC；它适合受控的内部服务边界，但不自动解决 deadline、兼容性、鉴权和可观测性。

> [!summary] 核心摘要
>
> gRPC 的价值是可生成的强类型契约、流式 RPC 与 HTTP/2 连接复用；生产可靠性仍取决于 deadline、取消、错误分类、版本演进和监控。

# gRPC and Protobuf (RPC 与序列化)

> [!note] 本节重点：gRPC 通信模型、Protobuf 序列化、C++ gRPC 服务端/客户端实现、与 HTTP/REST 对比

## 为什么需要 RPC

HTTP/JSON 适合公开 API、浏览器和直接调试；当内部服务需要可生成的契约或双向流时，gRPC 值得评估。但 JSON 并不必然慢，也能配合 OpenAPI 获得类型契约；SSE、WebSocket 和分块响应同样能提供不同形式的流式能力。

gRPC 的取舍是 Protobuf 契约、HTTP/2 流与代码生成，代价包括代理兼容、二进制调试、版本治理和依赖链复杂度。

---

# Protobuf（协议缓冲区）

## 定义接口（.proto 文件）

```protobuf
syntax = "proto3";

package userservice;

// 服务定义
service UserService {
    rpc GetUser (GetUserRequest) returns (User);
    rpc ListUsers (ListUsersRequest) returns (stream User);  // 服务端流
    rpc UpdateUser (stream UpdateUserRequest) returns (User); // 客户端流
    rpc Chat (stream ChatMessage) returns (stream ChatMessage); // 双向流
}

// 消息定义
message GetUserRequest {
    int32 user_id = 1;
}

message ListUsersRequest {}
message UpdateUserRequest { User user = 1; }
message ChatMessage { string text = 1; }

message User {
    int32 id = 1;
    string name = 2;
    string email = 3;
    int32 age = 4;
    // ... 更多字段
}
```

## Protobuf 编码特点

| 特性 | 说明 |
|------|------|
| **varint 编码** | 非负小整数通常较短；负 `int32` 不一定省空间，需区分 `sint32` 的 ZigZag 编码 |
| **字段编号 1–15** | 字段号与 wire type 组成的 tag 通常占 1 字节；字段值另计 |
| **字段编号 16–2047** | tag 通常占 2 字节；更大字段号占更多字节 |
| **proto3 默认值省略** | 标量零值默认不序列化（隐式 presence，非 Go 的 omitempty） |
| **演进兼容** | 增加可选字段通常可兼容旧二进制读者；未知字段在二进制透传中通常保留，但转 JSON 或手动逐字段复制可能丢失。删除字段要 `reserved` 其编号和名称，不能复用旧 tag |

相比 JSON，Protobuf 通常体积更小、编解码更快（二进制 + 字段编号），但**具体倍数受消息结构、字段类型与运行时实现影响，无固定值**，应以本场景基准测试为准（MEASURE_LOCALLY）。

---

## C++：一次 RPC 的契约与失败路径

先由 `.proto` 生成 C++ 消息类与服务/客户端 stub，再实现服务方法。下面是**源文件片段**：它依赖生成的 `user_service.grpc.pb.h`，不是单独编译即可运行的服务器。为了让取消、错误码与响应赋值清楚，示例只实现 `GetUser`，其余三个 RPC 需要单独补实现与测试。

```cpp
#include <chrono>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "user_service.grpc.pb.h"

class UserServiceImpl final : public userservice::UserService::Service {
public:
    grpc::Status GetUser(grpc::ServerContext* context,
                         const userservice::GetUserRequest* request,
                         userservice::User* reply) override {
        if (context->IsCancelled()) {
            return {grpc::StatusCode::CANCELLED, "request cancelled"};
        }
        if (request->user_id() != 1) {
            return {grpc::StatusCode::NOT_FOUND, "user not found"};
        }
        reply->set_id(1);
        reply->set_name("Ada");
        return grpc::Status::OK;
    }
};

grpc::Status FetchUser(const std::shared_ptr<grpc::Channel>& channel,
                       int user_id, userservice::User* reply) {
    auto stub = userservice::UserService::NewStub(channel);
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() +
                         std::chrono::seconds(2));
    userservice::GetUserRequest request;
    request.set_user_id(user_id);
    return stub->GetUser(&context, request, reply);
}
```

真实服务在 `GetUser` 中要让数据库查询服从剩余 deadline，并把“未找到”和“依赖失败”映射为不同 status；客户端检查 `status.ok()`，不要只读取 `reply`。流式 RPC 每次 `Write` 可能失败，读取循环结束还必须检查 `Finish()` 的最终状态。一个 `Channel` 可复用，但它不替每次 RPC 自动设业务 deadline。服务端监听地址、TLS 凭据和生成代码的构建步骤，以 [gRPC C++ basics](https://grpc.io/docs/languages/cpp/basics/) 的目标版本为准。

## gRPC 四种通信模式

| 模式 | 说明 | 适用场景 |
|------|------|---------|
| **Unary RPC** | 请求-响应，一对一 | 传统 CRUD、查询 |
| **Server Streaming** | 客户端发一个请求，服务端流式返回 | 批量拉取数据、日志导出 |
| **Client Streaming** | 客户端流式发送，服务端一次性返回 | 大文件上传、批量写入 |
| **Bidirectional Streaming** | 双向流，各自独立收发 | 聊天、实时协同、实时推送 |

---

## gRPC vs HTTP/REST

| 维度 | gRPC | HTTP/REST |
|------|------|-----------|
| 传输 | 通常使用 HTTP/2（二进制帧、多路复用） | 常见为 HTTP/1.1，也可运行在 HTTP/2 或 HTTP/3 上 |
| 序列化 | Protobuf（二进制，强类型） | JSON（文本，弱类型） |
| 接口契约 | `.proto` 文件（代码生成） | OpenAPI/Swagger（文档） |
| 流式通信 | 原生支持四种模式 | 需 WebSocket/SSE 补充 |
| 浏览器支持 | 需要 gRPC-Web | 天然支持 |
| 人类可读 | 二进制，不可直接 curl | JSON 文本，可 curl 调试 |
| C++ 生态 | 官方 cmake 支持，代码生成 | 手写 JSON 解析 + HTTP client |

**工程建议：**
- **内部服务间通信** → gRPC（性能好，强类型，流式支持）
- **外部客户端/浏览器** → REST/gRPC-Web
- **C++ 后端** → gRPC 是成熟且常用的 RPC 方案之一；也可按生态、运维和互操作需求评估 Thrift、HTTP/JSON 或自研协议

---

## 生产配置要点

| 边界 | 落地要求 |
| --- | --- |
| Deadline 与取消 | 客户端按业务预算设置 deadline；服务端把剩余时间传给下游，取消后停止无意义工作 |
| Status 与重试 | 区分 `INVALID_ARGUMENT`、`NOT_FOUND`、`UNAVAILABLE`、`DEADLINE_EXCEEDED`；只在语义可重试且有幂等保护时重试 |
| Channel 复用 | 复用长期 Channel/Stub，避免每次 RPC 都重新建连；连接行为受 resolver、负载均衡与配置影响 |
| Keepalive | 不复制“10 秒无请求也 ping”的模板；先与服务端/代理约定，过密 PING 可能收到 `GOAWAY: too_many_pings` |
| 安全 | 生产链路配置 TLS/身份认证，控制 metadata 的敏感字段，避免把内部错误详情直接暴露给客户端 |
| 观测 | 按服务、方法、status 记录次数、耗时和在途流；长流另跟踪存活时间、背压与消息尺寸 |

接口演进时优先新增字段；废弃字段时保留 `reserved` 编号和名称，不能把旧 tag 重新赋予另一种含义。`optional`/presence、`oneof`、枚举新增值及 Protobuf JSON 映射需分别做跨版本测试；“二进制能解析”不等于业务含义兼容。

拦截器可承担认证、追踪或统一日志，但 C++ API 与工厂注册方式随 gRPC 版本变化；在锁定依赖版本后参照官方示例实现，不在通用笔记里保留可能无法编译的伪代码。生产评估还要纳入代理是否支持 HTTP/2/gRPC、最大消息尺寸、流级背压和优雅关闭。

### 官方资料

- [gRPC C++ 文档](https://grpc.io/docs/languages/cpp/)
- [Protocol Buffers 文档](https://protobuf.dev/)
- 核验日期：2026-09-06

---
