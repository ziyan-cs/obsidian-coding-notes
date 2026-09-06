---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# HTTP Request and Response (HTTP 请求与响应)

> [!note] 本节重点：核心考点：HTTP 请求行/请求头/请求体结构、HTTP 响应状态行/响应头/响应体结构

# HTTP 请求结构

```
GET /index.html HTTP/1.1\r\n                    ← 请求行
Host: www.example.com\r\n                        ← 请求头
User-Agent: curl/7.68.0\r\n                     ← 请求头
Accept: */*\r\n                                 ← 请求头
\r\n                                             ← 空行（头结束标记）
body content                                     ← 请求体（可选）
```

**请求行三要素：**
- **方法**：GET/POST/PUT/DELETE 等
- **URI**：资源路径（/index.html?id=1）
- **HTTP 版本**：HTTP/1.0、HTTP/1.1、HTTP/2

**请求头关键字段：**

| 头部             | 作用                | 示例                             |
| -------------- | ----------------- | ------------------------------ |
| Host           | 目标主机（HTTP/1.1 必选） | Host: www.example.com          |
| Content-Length | 请求体长度（字节）         | Content-Length: 42             |
| Content-Type   | 请求体 MIME 类型       | Content-Type: application/json |
| Connection     | 连接管理              | Connection: keep-alive         |
| User-Agent     | 客户端标识             | User-Agent: Mozilla/5.0        |
| Cookie         | 客户端 Cookie        | Cookie: session_id=abc         |
| Authorization  | 认证信息              | Authorization: Bearer token    |
| Accept         | 接受的响应类型           | Accept: application/json       |

# HTTP 响应结构

```
HTTP/1.1 200 OK\r\n                              ← 状态行
Content-Type: text/html\r\n                       ← 响应头
Content-Length: 1234\r\n                         ← 响应头
Date: Mon, 01 Jan 2024 00:00:00 GMT\r\n          ← 响应头
\r\n                                             ← 空行
<html><body>...                                  ← 响应体
```

**状态行三要素：**
- **HTTP 版本**：HTTP/1.1
- **状态码**：200、404、500 等
- **原因短语**：OK、Not Found、Internal Server Error 等

**响应头关键字段：**

| 头部 | 作用 | 示例 |
|------|------|------|
| Content-Type | 响应体类型 | Content-Type: text/html; charset=utf-8 |
| Content-Length | 响应体长度 | Content-Length: 1234 |
| Transfer-Encoding | 传输编码 | Transfer-Encoding: chunked |
| Location | 重定向地址 | Location: /new-path |
| Set-Cookie | 设置 Cookie | Set-Cookie: session_id=xyz; Path=/ |
| Cache-Control | 缓存策略 | Cache-Control: max-age=3600 |
| Server | 服务器信息 | Server: nginx/1.20.1 |

# Content-Length vs Transfer-Encoding: chunked

**Content-Length**：响应体大小已知，直接指定字节数，接收方读完即止。

**Chunked Transfer Encoding**：响应体大小未知（如动态生成内容），分块传输：

```
HTTP/1.1 200 OK\r\n
Transfer-Encoding: chunked\r\n
\r\n
5\r\n                       ← 块大小（十六进制）
Hello\r\n                   ← 块数据
7\r\n
 World!\r\n                 ← 块数据
0\r\n                       ← 结束块（大小为 0）
\r\n                        ← 尾部空行
```

# HTTP 消息的文本协议特点

HTTP/1.1 是基于文本的协议，协议头是 ASCII 文本，用 `\r\n` 分隔行：

- **可读性强**：直接 telnet 调试
- **解析简单**：逐行读取，冒号分隔键值
- **冗余较大**：头部的重复字段每次请求都传输
- **队头阻塞**：文本协议的逐行解析在管道中有天然的 HOL blocking

```
telnet www.example.com 80
GET / HTTP/1.1
Host: www.example.com

```

> [!tip]- **工程要点**：HTTP 协议的头部以 `\r\n\r\n` 结束（空行），这是解析的终止边界。对于固定长度的请求体，Content-Length 指定精确字节数；对于变长内容（如 Server-Sent Events），使用 Transfer-Encoding: chunked。理解这一行终结的解析模型，是实现高性能 HTTP 状态机解析器的基础。

---

HTTP 协议深入见 → [HTTP Methods & Status Codes (方法与状态码)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06b-HTTP%20Methods%20&%20Status%20Codes%20(方法与状态码).md>) · [Keep-Alive & Connection Management (长连接)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06c-Keep-Alive%20&%20Connection%20Management%20(长连接).md>)

---

# HTTP Methods and Status Codes (HTTP 方法与状态码)

> [!note] 本节重点：核心考点：HTTP 方法（GET/POST/PUT/DELETE 等）语义与幂等性、状态码分类（1xx-5xx）与含义

# HTTP 请求方法

| 方法 | 语义 | 幂等 | 安全 | 请求体 | 响应体 | 典型场景 |
|------|------|------|------|--------|--------|---------|
| GET | 获取资源 | 是 | 是 | 通常不用 | 资源表示 | 页面加载、API 查询 |
| HEAD | 获取响应头 | 是 | 是 | 无 | 无 | 检查资源存在性 |
| POST | 提交/创建 | 否 | 否 | 提交数据 | 创建结果 | 表单提交、创建订单 |
| PUT | 全量更新/替换 | 是 | 否 | 完整资源 | 更新结果 | 更新用户信息 |
| PATCH | 部分更新 | 取决于补丁语义 | 否 | 增量数据 | 更新结果 | 修改单个字段 |
| DELETE | 删除资源 | 是 | 否 | 无 | 删除结果 | 删除记录 |
| OPTIONS | 查询支持的方法 | 是 | 是 | 无 | 允许方法 | CORS 预检请求 |

**幂等（Idempotent）：** 重复请求的预期资源状态与执行一次相同；响应状态码/响应体可以不同（例如重复 `DELETE` 可能返回 404）。POST 常用于非幂等创建；PATCH 是否幂等取决于补丁定义。

**安全（Safe）：** 不会修改服务器状态。GET/HEAD/OPTIONS 是安全的，可以放心预取。

# 状态码分类

```
1xx (Informational): 请求已接收，继续处理
2xx (Success):       请求成功接收并处理
3xx (Redirection):   需要进一步操作完成请求
4xx (Client Error):  请求包含错误或无法处理
5xx (Server Error):  服务器处理请求失败
```

# 常见状态码详解

**2xx 成功：**

| 状态码 | 含义 | 场景 |
|--------|------|------|
| 200 OK | 请求成功 | 标准响应 |
| 201 Created | 资源已创建 | POST 创建资源后 |
| 204 No Content | 成功但无响应体 | DELETE 删除成功 |
| 206 Partial Content | 部分内容 | 断点续传、视频流媒体 |

**3xx 重定向：**

| 状态码 | 含义 | 场景 |
|--------|------|------|
| 301 Moved Permanently | 永久重定向 | 域名变更，搜索引擎更新链接 |
| 302 Found | 临时重定向 | 未登录跳转到登录页 |
| 304 Not Modified | 资源未变更 | 条件请求（If-Modified-Since），使用缓存 |
| 307 Temporary Redirect | 临时重定向（保证请求方法不变） | POST 临时转发 |

**4xx 客户端错误：**

| 状态码 | 含义 | 场景 |
|--------|------|------|
| 400 Bad Request | 请求格式错误 | 参数校验失败、JSON 解析失败 |
| 401 Unauthorized | 未认证 | 未提供或无效的认证凭据 |
| 403 Forbidden | 无权限 | 已认证但无权访问，常见于鉴权失败 |
| 404 Not Found | 资源不存在 | URI 路径错误 |
| 405 Method Not Allowed | 方法不允许 | GET-only 接口收到了 POST 请求 |
| 408 Request Timeout | 请求超时 | 客户端在超时时间内未发送完整请求 |
| 409 Conflict | 资源冲突 | 版本冲突、唯一键冲突 |
| 413 Payload Too Large | 请求体过大 | 上传文件超出限制 |
| 429 Too Many Requests | 请求过频 | 限流触发 |

**5xx 服务端错误：**

| 状态码 | 含义 | 场景 |
|--------|------|------|
| 500 Internal Server Error | 服务器内部错误 | 未捕获异常、空指针 |
| 502 Bad Gateway | 网关/代理收到无效响应 | Nginx 后端服务挂了 |
| 503 Service Unavailable | 服务暂时不可用 | 服务器过载、维护中 |
| 504 Gateway Timeout | 网关超时 | 上游服务响应超时 |

# RESTful API 设计中的方法使用

```
GET    /users          → 获取用户列表    200 OK
GET    /users/42       → 获取单个用户    200 OK / 404
POST   /users          → 创建用户        201 Created
PUT    /users/42       → 全量更新用户    200 OK / 204
PATCH  /users/42       → 部分更新用户    200 OK
DELETE /users/42       → 删除用户        204 No Content / 404
```

> [!tip]- **工程要点**：优先用 HTTP 状态码表达传输/资源层语义；业务错误码仍可放在结构化响应体中，便于客户端精确处理。关键是团队约定一致，而不是把所有失败都包装成 `200 OK`。例如：`{"error": {"code": "VALIDATION_ERROR", "message": "email is required"}}`。

# 30 秒回答

**幂等和安全有什么区别？** 安全方法不应改变服务器的预期状态；幂等方法允许改变状态，但重复执行后的资源状态应等价。`DELETE` 通常幂等但不安全；POST 通常既不安全也不幂等。重试前必须同时看方法语义、请求体和服务端幂等键设计。

**自测：** 为什么 PATCH 不能机械标为“非幂等”？为什么不能把“重复 DELETE 返回 404”直接当作不幂等？

---

HTTP 协议结构见 → [Request & Response Structure (请求响应结构)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06a-Request%20&%20Response%20Structure%20(请求响应结构).md>) · [Keep-Alive & Connection Management (长连接)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06c-Keep-Alive%20&%20Connection%20Management%20(长连接).md>)

---

# HTTP Connection Management (HTTP 连接管理)

> [!note] 本节重点：核心考点：Keep-Alive 长连接复用、Connection 头、管道传输 Pipeline、HTTP 队头阻塞问题

# 短连接 vs 长连接

**短连接（HTTP/1.0 默认）：**
```
客户端 → 打开 TCP → 请求 → 响应 → 关闭 TCP （下一个请求重复此流程）
```
每个请求都新建 TCP 连接，三次握手 + 慢启动开销巨大。

**长连接（HTTP/1.1 默认）：**
```
客户端 → 打开 TCP → 请求 → 响应 → 请求 → 响应 → ... → 关闭
```
复用同一 TCP 连接发送多个请求，避免连接建立开销。

# Connection 头

HTTP/1.1 默认开启 Keep-Alive，通过 Connection 头控制：

```http
Connection: keep-alive
Keep-Alive: timeout=5, max=100   # 超时 5s，最大 100 个请求

Connection: close
```

**Connection: close 的行为：**
服务端发送完当前响应后关闭连接。常用于处理异常或负载过高的场景。

# Pipeline：请求管道

管道（Pipelining）允许客户端在**收到上一个响应之前**发送下一个请求：

```
非管道模式（串行）：
  C: 请求1 → → → 
  S:          → 响应1 → → → 
  C:                  请求2 → → → 
  S:                            → 响应2

管道模式：
  C: 请求1 → 请求2 → 请求3 → 
  S:          → 响应1 → 响应2 → 响应3 →
```

**Pipelining 的现实问题：**
- **队头阻塞**（HOL Blocking）：如果响应1生成慢，后面的响应都被阻塞
- **实现复杂**：响应顺序必须与请求顺序一致（否则客户端无法匹配）
- **浏览器默认关闭**：实际推广失败，绝大多数客户端不启用
- **代理兼容性差**：很多中间代理不支持 Pipelining

# 队头阻塞（Head-of-Line Blocking）

HTTP/1.1 的核心缺陷：同一连接上的请求必须有序返回。

```
请求 A（大文件 10MB）    → 阻塞
请求 B（小型 JSON）      → 等待 A 完成    ← HOL Blocking!
请求 C（小型 JSON）      → 等待 B 完成
```

**缓解方案：**

| 方案 | 原理 | 缺点 |
|------|------|------|
| 多连接 | 浏览器打开 6-8 个并行 TCP | 连接竞争、服务器压力大 |
| 域名分片 | 将资源分散到不同域名 | DNS 开销、管理复杂 |
| HTTP/2 多路复用 | 二进制分帧，请求交错 | 需要升级协议 |

# Keep-Alive 工程配置

**Nginx 配置：**
```nginx
http {
    keepalive_timeout  65;        # 空闲连接超时
    keepalive_requests 1000;      # 单个连接最大请求数
}
```

**权衡点：**
```
Keep-Alive 时间过短：
  - 连接复用率低，浪费三次握手
  - 适合短突发请求场景

Keep-Alive 时间过长：
  - 占用服务器 fd 资源（C10K 问题）
  - 客户端已消失但连接未释放
  - 适合频繁请求场景（如 API 服务）
```

> [!tip]- **工程要点**：HTTP/1.1 长连接减少建连成本，但管道化的响应有序性会导致应用层 HOL。HTTP/2 用流多路复用消除了这类请求级 HOL，却仍受 TCP 丢包的传输层 HOL 影响；是否升级应结合客户端覆盖、代理链路和实际压测决定。

# HTTP Parser State Machine (HTTP 解析状态机)

> [!note] 本节重点：核心考点：> HTTP 解析的有限状态机实现、状态驱动解析流程、缓冲区管理与边界处理

# 为什么用状态机解析 HTTP

HTTP 协议是基于文本的流式协议，数据可能分多次到达（TCP 流特性）。状态机能很好地处理**部分到达**的数据——解析到哪一步就停在对应状态，新数据到达后继续。

```
TCP 流可能分段到达：
  第一次: "GET /index"
  第二次: ".html HTTP/1.1\r\nHost: www"
  第三次: ".example.com\r\n\r\n"

状态机：每次处理到当前数据末尾 → 保存状态 → 等待下次数据
```

# HTTP 解析状态定义

```c
typedef enum {
    // 请求行解析
    METHOD_START,       // 等待方法首字符
    METHOD,             // 读取方法名
    URI_START,          // 等待 URI 起始
    URI,                // 读取 URI
    VERSION_H,          // 遇到 H（HTTP 的 H）
    VERSION_HT,         // HT
    VERSION_HTT,        // HTT
    VERSION_HTTP,       // HTTP
    VERSION_MAJOR,      // 主版本号
    VERSION_DOT,        // 版本号点
    VERSION_MINOR,      // 次版本号
    REQ_LINE_CR,        // 请求行 \r
    REQ_LINE_LF,        // 请求行 \n

    // 头部解析
    HEADER_START,       // 头部起始（也可能是空行）
    HEADER_KEY,         // 读取 header key
    HEADER_COLON,       // 冒号
    HEADER_SPACE,       // 冒号后的空格
    HEADER_VALUE,       // 读取 header value
    HEADER_CR,          // 头部 \r
    HEADER_LF,          // 头部 \n

    // 空行 → 进入 body
    HEADERS_COMPLETE_CR, // 空行的 \r
    HEADERS_COMPLETE_LF, // 空行的 \n → body 开始

    // Body
    BODY_IDENTITY,       // Content-Length body
    BODY_CHUNKED_SIZE,   // chunked 块大小
    BODY_CHUNKED_DATA,   // chunked 块数据
    BODY_CHUNKED_CR,     // chunked 块 \r
    BODY_CHUNKED_LF,     // chunked 块 \n
    BODY_CHUNKED_END,    // chunked 结束

    COMPLETE,            // 解析完成
    ERROR                // 解析错误
} http_parser_state;
```

# 核心解析循环

```c
typedef struct {
    http_parser_state state;
    char *buffer;
    size_t buffer_used;
    size_t buffer_size;

    // 解析中间结果
    char method[16];
    char uri[1024];
    // 头部 key-value 表
    // body 指针和长度
} http_parser;

http_parser_state http_parser_execute(http_parser *parser, const char *data, size_t len) {
    size_t i = 0;
    char ch;

    while (i < len && parser->state != COMPLETE && parser->state != ERROR) {
        ch = data[i++];

        switch (parser->state) {
            case METHOD_START:
                if (is_upper_alpha(ch)) {
                    parser->method[0] = ch;
                    parser->method_len = 1;
                    parser->state = METHOD;
                } else { parser->state = ERROR; }
                break;

            case METHOD:
                if (is_upper_alpha(ch)) {
                    parser->method[parser->method_len++] = ch;
                } else if (ch == ' ') {
                    parser->method[parser->method_len] = '\0';
                    parser->state = URI_START;
                } else { parser->state = ERROR; }
                break;

            // ... 其他状态转换 ...

            case HEADERS_COMPLETE_LF:
                // 空行结束，进入 body 解析
                if (parser->chunked) {
                    parser->state = BODY_CHUNKED_SIZE;
                } else {
                    parser->body_remaining = parser->content_length;
                    parser->state = parser->body_remaining > 0
                        ? BODY_IDENTITY : COMPLETE;
                }
                break;

            default:
                break;
        }
    }
    return parser->state;
}
```

# 缓冲区管理

```c
// 环形缓冲区或线性缓冲区的管理
typedef struct {
    char *data;          // 缓冲区起始
    size_t size;         // 缓冲区总大小
    size_t read_pos;     // 已消费位置
    size_t write_pos;    // 已写入位置
    int  is_full;        // 缓冲区满标志
} buffer_t;

// 关键操作：
// 1. 从 socket 读取数据到缓冲区
// 2. 调用解析器消费数据
// 3. 调整 read_pos
// 4. 当剩余空间不足时 compact（移动未消费数据到头部）
```

**边界处理要点：**
- 解析器不应假设一次数据包含完整的 HTTP 消息
- 每次返回状态给调用方，调用方负责提供更多数据
- 缓冲区用尽时扩展或 compact
- HTTP 头部大小通常限制在 8KB 内防止恶意请求

# 完整解析流程

```
TCP 数据到达 → buffer 写入 → parse() 逐字节消费
                                    ↓
                        状态停留在部分解析处
                                    ↓
                更多数据 → buffer 继续写入 → parse() 从断点继续
                                    ↓
                            COMPLETE → 处理请求/响应
                                    ↓
                        重置解析器 → 解析下一个消息
```

> [!tip]- **工程要点**：实际生产中使用现成的高性能解析器（http_parser、llhttp），它们使用 goto 驱动的状态机（而非 switch-case）以获得极致性能。理解状态机原理比自己实现更重要——关键是**解析器不持有状态本身的语义**，只做字节级别的状态跳转，上层回调才是业务处理入口。

---

HTTP 协议基础见 → [Request & Response Structure (请求响应结构)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06a-Request%20&%20Response%20Structure%20(请求响应结构).md>) · [Keep-Alive & Connection Management (长连接)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06c-Keep-Alive%20&%20Connection%20Management%20(长连接).md>)

# 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

# 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **04-HTTP Protocol (HTTP 协议)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
