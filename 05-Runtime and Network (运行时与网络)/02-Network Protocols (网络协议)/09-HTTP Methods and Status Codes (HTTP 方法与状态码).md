---
tags:
  - network/http
status: 🌱
---

# 09-HTTP Methods and Status Codes (HTTP 方法与状态码)

> [!abstract] 核心考点：HTTP 方法（GET/POST/PUT/DELETE 等）语义与幂等性、状态码分类（1xx-5xx）与含义

## HTTP 请求方法

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

## 状态码分类

```
1xx (Informational): 请求已接收，继续处理
2xx (Success):       请求成功接收并处理
3xx (Redirection):   需要进一步操作完成请求
4xx (Client Error):  请求包含错误或无法处理
5xx (Server Error):  服务器处理请求失败
```

## 常见状态码详解

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

## RESTful API 设计中的方法使用

```
GET    /users          → 获取用户列表    200 OK
GET    /users/42       → 获取单个用户    200 OK / 404
POST   /users          → 创建用户        201 Created
PUT    /users/42       → 全量更新用户    200 OK / 204
PATCH  /users/42       → 部分更新用户    200 OK
DELETE /users/42       → 删除用户        204 No Content / 404
```

> [!tip]- **工程要点**：优先用 HTTP 状态码表达传输/资源层语义；业务错误码仍可放在结构化响应体中，便于客户端精确处理。关键是团队约定一致，而不是把所有失败都包装成 `200 OK`。例如：`{"error": {"code": "VALIDATION_ERROR", "message": "email is required"}}`。

## 30 秒回答

**幂等和安全有什么区别？** 安全方法不应改变服务器的预期状态；幂等方法允许改变状态，但重复执行后的资源状态应等价。`DELETE` 通常幂等但不安全；POST 通常既不安全也不幂等。重试前必须同时看方法语义、请求体和服务端幂等键设计。

**自测：** 为什么 PATCH 不能机械标为“非幂等”？为什么不能把“重复 DELETE 返回 404”直接当作不幂等？

---

HTTP 协议结构见 → [Request & Response Structure (请求响应结构)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06a-Request%20&%20Response%20Structure%20(请求响应结构).md>) · [Keep-Alive & Connection Management (长连接)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06c-Keep-Alive%20&%20Connection%20Management%20(长连接).md>)
