---
study_stage: backlog
---

> [!summary] 核心摘要
>
> TCP 只提供有序字节流，不保留应用消息边界；所谓粘包和拆包是读取批次与消息边界不一致。应用层必须使用定长、分隔符、长度前缀或自描述协议，并在缓冲区中处理半包、并包和恶意长度。
>

> [!note] 本节重点：什么是粘包、根本原因、四种解决方案

# 什么是粘包？

**粘包（Sticky Packet）**：接收方在读取数据时，无法正确区分出原始的消息边界，多个消息被"粘"在一起，或一个消息被拆成多段读取。

```text
┌──────────────────────────────────────┐
│  Sender (Data Stream)                │
├──────────┬──────────┬────────────────┤
│  Msg A   │  Msg B   │  Msg C         │
└──────────┴──────────┴───────┬────────┘
                              │
                              ▼
              ┌──────────────────────────┐
              │  TCP Byte Stream         │
              │  (may merge/split)       │
              └──────┬───────────────────┘
                     │
     ┌───────────────┼───────────────┐
     │               │               │
     ▼               ▼               ▼
┌───────────┐ ┌───────────┐ ┌───────────┐
│ Case 1:   │ │ Case 2:   │ │ Case 3:   │
│ Normal    │ │ Sticky    │ │ Split     │
│ [A][B][C] │ │ [AB][C]   │ │ [A][BC]   │
└───────────┘ └───────────┘ └───────────┘
┌───────────────────────────────────────┐
│ Case 4: Fragmented                    │
│ [A] [B-half] [B-rest]                 │
└───────────────────────────────────────┘
```

---

# 为什么一次读写不能对应一条消息

## 1. TCP 是字节流协议

TCP **不保留消息边界**，只保证字节的顺序和可靠性。发送的 "消息" 概念在 TCP 层是不存在的，只有连续的字节流。

## 2. 发送端可能合并小段

Nagle 算法可能推迟部分小数据发送，以减少过多小 TCP 段：

- 条件与行为还受未确认数据、MSS、发送缓冲区及实现影响，不应背成单一 if 公式。
- 即使禁用 Nagle，多个 `write()` 仍可能被一次 `read()` 读到；一次 `write()` 也可能被多次读取。Nagle **不是**消息边界丢失的根因。

## 3. 接收缓冲区读取时机

- 接收方没有及时读取，缓冲区积压了多条消息
- 应用层一次 read() 可能读出多条消息的数据

> **注意：粘包是应用层问题，不是 TCP 的 bug。** TCP 本就是字节流，正确的应用层协议设计需要自己定义消息边界。

---

# 四种解决方案

## 方案一：固定长度消息

每条消息长度固定，接收方每次读取固定字节数。

```
发送：[MSG_001____][MSG_002____]（每条固定 10 字节）
接收：累计读满 10 字节才得到一条完整消息；单次 read(10) 可能提前返回
```

- ✅ 实现简单
- ❌ 消息长度不灵活，短消息浪费空间

## 方案二：特殊分隔符

用特定字符标记消息结尾（如 `\n`、`\r\n`、`\0`）。

```
发送：Hello\nWorld\n
接收：按 \n 分割，还原两条消息
```

- ✅ 实现简单，适合以行/分隔符组织的文本协议（例如 Redis RESP 的部分帧）
- ❌ 消息内容中不能含有分隔符（或需转义）

## 方案三：消息头 + 长度字段（最常用）

在消息前加固定长度的头部，头部中包含消息体的长度。

```
┌─────────────────┬───────────────────────┐
│  Header (4字节) │   Body (N字节)        │
│  length = N     │  实际消息内容         │
└─────────────────┴───────────────────────┘

接收流程：
1. 先读 4 字节，解析出 N
2. 再读 N 字节，得到完整消息体
```

- ✅ 灵活、高效，适合二进制协议
- ✅ 常见于二进制协议；具体产品的帧头格式、长度含义和最大帧限制各不相同
- ❌ 需要处理拆包逻辑（一次 read() 可能只读到部分头部）

> [!tip] HTTP/1.1 的“消息边界”不能简单归为分隔符：请求/响应头以空行结束，但消息体由 `Content-Length`、`Transfer-Encoding: chunked` 或连接关闭等规则界定。

## 方案四：应用层自定义完整协议（TLV）

TLV（Type-Length-Value）结构：

```
┌─────────┬─────────┬──────────────┐
│  Type   │ Length  │    Value      │
│ (2字节) │ (4字节) │  (Length字节) │
└─────────┴─────────┴──────────────┘
```

- ✅ 扩展性强，支持多种消息类型；上图只是自定义示例，不代表 MQTT 等协议的实际字节布局
- ✅ 适合需要显式类型与长度的自定义二进制协议

---

# 长度前缀的阻塞读取示例

下面是 POSIX 阻塞 socket 的示例：先读满 4 字节长度，再读满消息体。`false` 仅表示**尚未开始下一帧时**遇到正常 EOF；读到半帧后连接关闭属于协议截断。生产代码还需按协议决定空消息是否允许，以及异常后如何关闭连接。

```cpp
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <system_error>
#include <vector>
#include <unistd.h>

// 仅适用于阻塞 socket；允许在本次读取的第一个字节之前遇到正常 EOF。
bool readExactly(int fd, void* dest, std::size_t size) {
    auto* p = static_cast<unsigned char*>(dest);
    std::size_t done = 0;
    while (done < size) {
        const ssize_t n = ::read(fd, p + done, size - done);
        if (n > 0) {
            done += static_cast<std::size_t>(n);
        } else if (n == 0) {
            if (done == 0) return false;
            throw std::runtime_error("truncated frame");
        } else if (errno != EINTR) {
            throw std::system_error(errno, std::generic_category(), "read");
        }
    }
    return true;
}

bool readMessage(int fd, std::vector<char>& out) {
    std::uint32_t networkLength = 0;
    if (!readExactly(fd, &networkLength, sizeof networkLength)) return false;

    constexpr std::uint32_t kMaxBody = 16U * 1024U * 1024U;
    const std::uint32_t bodyLength = ntohl(networkLength);
    if (bodyLength > kMaxBody) throw std::runtime_error("frame too large");

    out.resize(bodyLength);
    if (bodyLength != 0 && !readExactly(fd, out.data(), bodyLength)) {
        throw std::runtime_error("truncated frame body");
    }
    return true;
}
```

发送端也必须按相同格式先写 `htonl(bodyLength)` 的 4 字节，再写消息体；`write()`/`writev()` 可能部分写入，必须保存偏移并继续发送。非阻塞 socket 不能直接套用此函数：遇到 `EAGAIN/EWOULDBLOCK` 时要保留已读头部、期望体长和已读体长，待下一次可读事件继续。限长之外，还需考虑读超时、慢速客户端、总缓冲预算和异常连接清理。`TCP_NODELAY` 只影响部分小包发送时机，**不能代替分帧**。

# 总结对比

|方案|适用场景|优点|缺点|
|---|---|---|---|
|固定长度|消息格式固定的内部协议|边界易计算|每帧仍须累计读满，变长数据不便|
|分隔符|按行组织的文本协议|易观察、易调试|要处理转义、限长与扫描成本|
|长度头部|自定义二进制协议|适合变长消息|须校验长度并累计读满|
|TLV|需要扩展字段的二进制协议|可扩展字段类型|须定义类型、长度及未知字段规则|

---
