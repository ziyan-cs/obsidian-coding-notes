---
tags:
  - network/http
status: seed
review_due: 2026-09-26
confidence: 1
verified: stable
---

# State Machine Parser Implementation — 状态机解析实现

> [!abstract] 核心考点：> HTTP 解析的有限状态机实现、状态驱动解析流程、缓冲区管理与边界处理

## 为什么用状态机解析 HTTP

HTTP 协议是基于文本的流式协议，数据可能分多次到达（TCP 流特性）。状态机能很好地处理**部分到达**的数据——解析到哪一步就停在对应状态，新数据到达后继续。

```
TCP 流可能分段到达：
  第一次: "GET /index"
  第二次: ".html HTTP/1.1\r\nHost: www"
  第三次: ".example.com\r\n\r\n"

状态机：每次处理到当前数据末尾 → 保存状态 → 等待下次数据
```

## HTTP 解析状态定义

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

## 核心解析循环

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

## 缓冲区管理

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

## 完整解析流程

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
