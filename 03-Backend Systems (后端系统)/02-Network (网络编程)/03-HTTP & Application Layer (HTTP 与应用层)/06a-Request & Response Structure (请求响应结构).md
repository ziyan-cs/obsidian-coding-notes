---
tags:
  - network/http
status: seed
review_due: 2026-09-26
confidence: 1
verified: stable
---

# Request & Response Structure — 请求响应结构

> [!important] **核心考点**：HTTP 请求行/请求头/请求体结构、HTTP 响应状态行/响应头/响应体结构

## HTTP 请求结构

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

## HTTP 响应结构

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

## Content-Length vs Transfer-Encoding: chunked

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

## HTTP 消息的文本协议特点

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
