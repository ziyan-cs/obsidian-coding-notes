---
tags:
  - redis/core
status: 🌱
---

> **核心考点**：SDS 设计原理、相比 C 字符串的优势、内存预分配策略、二进制安全

## SDS 结构

Redis 自定义的字符串类型，替代 C 字符串（`char*`）：

```c
// Redis 3.2+ 的 SDS 实现（5 种长度类型）
struct sdshdr8 {
    uint8_t len;      // 已使用字节数
    uint8_t alloc;    // 总分配字节数（不含头和 null 终止符）
    unsigned char flags; // 低 3 位存类型（sdshdr5/8/16/32/64）
    char buf[];       // 数据数组（柔性数组）
};
// sdshdr16/32/64 同理，len/alloc 类型不同
```

**为什么设计 5 种类型：** 根据字符串长度选择最小的 header（sdshdr5/sdshdr8/sdshdr16/sdshdr32/sdshdr64），节省内存。

---

## SDS 相比 C 字符串的优势

| 问题 | C 字符串 | SDS |
|------|---------|-----|
| **获取长度** | O(n) 遍历 | O(1) 读 len 字段 |
| **缓冲区溢出** | `strcat` 可能覆盖邻接内存 | API 自动检查 alloc 并扩容 |
| **内存重分配** | 每次修改都需要 | 预分配 + 惰性释放 |
| **二进制安全** | 遇 `\0` 截断（空字符结尾） | 用 len 记录长度，`\0` 可存储 |
| **兼容 C 函数** | 天生兼容 | 也兼容（末尾保留 `\0`） |

---

## 内存预分配策略

```c
// SDS 扩容策略（sds.c 中的 sdsMakeRoomFor）
if (newlen < SDS_MAX_PREALLOC)     // 1MB 以内
    newlen *= 2;                    // 加倍（翻倍增长）
else
    newlen += SDS_MAX_PREALLOC;     // 超过 1MB 后每次加 1MB

// 例：当前 "abc"（len=3）
// append "defghij" → newlen = 10 → 实际分配 20（预分配）
// 下次 append 只需抄一次，避免频繁 realloc
```

**惰性释放：** `sdstrim(s, "X")` 仅修改指针位置，不释放内存；`sdsclear(s)` 将 len 置 0 但保留 alloc。真正释放用 `sdsfree()`。

---

## SDS API 速查

```c
sds s = sdsnew("hello");        // 创建，O(len)
sds s = sdsempty();             // 创建空串
s = sdscat(s, " world");        // 拼接（可能 realloc）
s = sdscpy(s, "new");           // 覆盖
s = sdstrim(s, "abc");          // 去除两端指定字符
sdstoupper(s);                  // 转大写
sdsrange(s, 0, 2);             // 取子串 [0,2]
sdsfree(s);                     // 释放
```

**API 设计特点：** 修改 SDS 的 API 都返回 `sds*`（可能因 realloc 改变地址），调用者必须用返回值。

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| SDS 如何 O(1) 获取长度 | header 中存 len 字段 |
| 二进制安全含义 | 可存 `\0`、图片等非文本数据 |
| 预分配阈值 1MB | 小字符串加倍，大字符串线性增长 |
| SDS 相比 std::string | Redis C 实现，内存紧凑；std::string SSO 且有 RAII |

> **工程要点**：SDS 的预分配策略是"空间换时间"的典型——大多数 append 操作无需重新分配。`SDS_MAX_PREALLOC` 设为 1MB 避免大字符串翻倍浪费。

---



压缩列表实现见 → [01a2-ziplist & listpack (压缩列表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a2-ziplist%20&%20listpack%20(压缩列表).md) · [01a3-skiplist：Sorted Set Internals (跳表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)
