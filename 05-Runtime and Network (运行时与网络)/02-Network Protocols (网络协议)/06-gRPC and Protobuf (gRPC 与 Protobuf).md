---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 一句话结论：gRPC 用 Protobuf 契约与 HTTP/2 流承载服务间 RPC；它适合受控的内部服务边界，但不自动解决 deadline、兼容性、鉴权和可观测性。

# 30 秒回答

**核心结论**：gRPC 的价值是可生成的强类型契约、流式 RPC 与 HTTP/2 连接复用；生产可靠性仍取决于 deadline、取消、错误分类、版本演进和监控。

# gRPC and Protobuf (RPC 与序列化)

> [!note] 本节重点：核心考点：gRPC 通信模型、Protobuf 序列化、C++ gRPC 服务端/客户端实现、与 HTTP/REST 对比

# 为什么需要 RPC

微服务架构中，服务之间需要通信。HTTP/REST 虽然简单，但存在痛点：
- 文本协议（JSON）序列化/反序列化开销大
- 无强类型接口约束，客户端与服务端容易不一致
- 只支持请求-响应模式，流式通信需要额外实现

**gRPC** 解决这些问题：Protobuf 强类型 + HTTP/2 多路复用 + 四种通信模式。

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
| **varint 编码** | 小整数用 1 字节，大整数用更多，自动变长 |
| **字段编号 1-15** | 1 字节编码（高频字段放这里） |
| **字段编号 16+** | 2 字节编码 |
| **proto3 默认值省略** | 标量零值默认不序列化（隐式 presence，非 Go 的 omitempty） |
| **前向兼容** | 新增字段不影响旧客户端（老代码忽略不识别的字段） |

相比 JSON，Protobuf 通常体积更小、编解码更快（二进制 + 字段编号），但**具体倍数受消息结构、字段类型与运行时实现影响，无固定值**，应以本场景基准测试为准（NEEDS_VERIFY）。

---

# C++ gRPC 服务端

```cpp
#include <grpcpp/grpcpp.h>
#include "user_service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class UserServiceImpl final : public UserService::Service {
    Status GetUser(ServerContext* context, const GetUserRequest* request,
                   User* reply) override {
        // 从数据库/缓存查询用户
        int userId = request->user_id();
        User user = db_.findUser(userId);
        if (!user) {
            return Status(grpc::NOT_FOUND, "user not found");
        }
        *reply = std::move(user);
        return Status::OK;
    }

    // 服务端流：批量返回
    Status ListUsers(ServerContext* context, const ListUsersRequest* request,
                     grpc::ServerWriter<User>* writer) override {
        for (const auto& user : db_.allUsers()) {
            writer->Write(user);  // 多次 Write，流式返回
        }
        return Status::OK;
    }

private:
    UserDatabase db_;
};

int main() {
    UserServiceImpl service;
    ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port 50051" << std::endl;
    server->Wait();  // 阻塞等待
}
```

---

# C++ gRPC 客户端

```cpp
#include <grpcpp/grpcpp.h>
#include "user_service.grpc.pb.h"

using grpc::ClientContext;
using grpc::Status;

class UserClient {
public:
    UserClient(const std::string& target)
        : stub_(UserService::NewStub(
              grpc::CreateChannel(target, grpc::InsecureChannelCredentials()))) {}

    // 简单 RPC（Unary）
    User GetUser(int userId) {
        GetUserRequest req;
        req.set_user_id(userId);

        User reply;
        ClientContext ctx;
        // 设置超时（关键！）
        ctx.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

        Status status = stub_->GetUser(&ctx, req, &reply);
        if (!status.ok()) {
            throw std::runtime_error("GetUser failed: " + status.error_message());
        }
        return reply;
    }

    // 服务端流式读取
    void ListUsers() {
        ListUsersRequest req;
        ClientContext ctx;
        auto reader = stub_->ListUsers(&ctx, req);
        User user;
        while (reader->Read(&user)) {
            std::cout << "Got user: " << user.name() << std::endl;
        }
        Status status = reader->Finish();  // 检查流结束状态
    }

private:
    std::unique_ptr<UserService::Stub> stub_;
};
```

---

# gRPC 四种通信模式

| 模式 | 说明 | 适用场景 |
|------|------|---------|
| **Unary RPC** | 请求-响应，一对一 | 传统 CRUD、查询 |
| **Server Streaming** | 客户端发一个请求，服务端流式返回 | 批量拉取数据、日志导出 |
| **Client Streaming** | 客户端流式发送，服务端一次性返回 | 大文件上传、批量写入 |
| **Bidirectional Streaming** | 双向流，各自独立收发 | 聊天、实时协同、实时推送 |

---

# gRPC vs HTTP/REST

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

# 生产配置要点

## 连接池与复用

```cpp
// 创建连接池，复用 channel（channel 是线程安全的）
auto channel = grpc::CreateChannel(target, creds);
auto stub1 = UserService::NewStub(channel);
auto stub2 = OrderService::NewStub(channel);  // 复用同一连接

// 设置 Keepalive
grpc::ChannelArguments args;
args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);        // 10s ping
args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);      // 5s 超时
args.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1); // 无活跃请求也 ping
auto ch = grpc::CreateCustomChannel(target, creds, args);
```

## 超时与重试

```cpp
// 客户端超时
ClientContext ctx;
ctx.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(200));

// 服务端超时（从 metadata 中读取 deadline）
auto deadline = context->deadline();
if (std::chrono::system_clock::now() >= deadline) {
    return Status(grpc::DEADLINE_EXCEEDED, "timeout");
}
```

## 拦截器（Interceptor）

```cpp
// 日志拦截器示例
class LogInterceptor : public grpc::Interceptor {
    void Intercept(grpc::InterceptorBatchMethods* methods) override {
        if (methods->QueryInterceptionHookPoint(
                grpc::InterceptionHookPoints::POST_RECV_INITIAL_METADATA)) {
            std::cout << "RPC started" << std::endl;
        }
        methods->Proceed();  // 继续拦截链
    }
};
```

> **版本/API 需验证（NEEDS_VERIFY）**：拦截器 C++ API 在不同 gRPC 版本间有变化（`grpc::Interceptor` 基类、`InterceptorBatchMethods`、工厂注册方式等），上例为示意，编译前以所用 gRPC 版本头文件为准。

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| gRPC vs Thrift | gRPC 基于 HTTP/2，Thrift 可自定义传输层；gRPC 生态更好 |
| Protobuf 编码 | Varint + ZigZag + 字段编号 (field_number << 3 \| wire_type) |
| gRPC 流的实现 | 基于 HTTP/2 的 DATA frame，同一连接多流复用 |
| gRPC 为什么快 | Protobuf 编解码快 + HTTP/2 多路复用减少连接数 |
| Channel 的安全性 | 多个 Stub 共享 Channel 是线程安全的，不需要额外锁 |

> [!tip]- **工程要点**：内部服务可优先评估 gRPC，但不是默认答案。每个 RPC 都应显式设置 deadline；复用 Channel 而非每次新建。Keepalive 应按代理、负载均衡器和服务端策略配置，过于激进会制造无效流量。CMake 中锁定并验证所用 gRPC 版本与 ABI。

## 官方资料

- [gRPC C++ 文档](https://grpc.io/docs/languages/cpp/)
- [Protocol Buffers 文档](https://protobuf.dev/)
- 核验日期：2026-09-06

---

gRPC 底层协议见 → [HTTP/2 Key Features (HTTP2核心特性了解)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/08-HTTP⧸2%20Key%20Features%20(HTTP2核心特性了解).md>) · [HTTPS & TLS Overview (HTTPS原理概览)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/07-HTTPS%20&%20TLS%20Overview%20(HTTPS原理概览).md>)

## 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

## 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **06-gRPC and Protobuf (gRPC 与 Protobuf)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
