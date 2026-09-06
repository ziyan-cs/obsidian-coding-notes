---
tags: [language/python, python/http, testing/api]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-HTTP Automation and API Checks (HTTP自动化与接口检查)

> [!abstract] 一句话结论：接口检查工具要验证 status、body、超时和失败信息；绝不能无超时地发请求，也不能把 token 打进日志。

> [!warning] 自动化脚本也属于生产边界
> 脚本的失败输出应足够让人定位问题，但不能泄露 `Authorization`、Cookie、token 或完整用户数据。把 URL、timeout 与凭据来源显式配置，避免“只在我的电脑能跑”。

```python
from urllib.request import Request, urlopen
from urllib.error import URLError, HTTPError
import json

def health(url: str, timeout: float = 3.0) -> dict:
    request = Request(url, headers={"Accept": "application/json"})
    try:
        with urlopen(request, timeout=timeout) as response:
            if response.status != 200:
                raise RuntimeError(f"unexpected status: {response.status}")
            return json.load(response)
    except HTTPError as exc:
        raise RuntimeError(f"HTTP {exc.code} from {url}") from exc
    except URLError as exc:
        raise RuntimeError(f"network error for {url}") from exc
```

## 检查维度

- 网络：域名、连接、timeout、TLS。
- 协议：method、status、header、JSON 格式。
- 业务：必须字段、错误码、响应时间阈值。
- 安全：token 从环境变量/安全存储读取；日志中脱敏。

## 30 秒回答

Python 很适合做黑盒 API checker：发一个可配置、带 timeout 的请求，检查协议层（状态码、header、JSON）与业务层（字段、错误码、阈值），在失败时以非零退出码和脱敏上下文结束。它验证的是服务可观察到的行为，不替代服务端单元测试。

## 自测

1. `HTTPError` 和 `URLError` 分别代表哪一类失败？为什么要分别报告？
2. 为什么“请求成功返回 200”还不足以判定接口健康？
3. 如何让命令行脚本在健康检查失败时返回非零退出码？

## C++ / Go 对照

Python 适合写黑盒 API checker、回归脚本和压测结果处理；服务实现仍优先放在 Go/C++。工具必须保留可配置 URL、timeout 和明确失败输出，不能只在你电脑上“碰巧能跑”。

## 练习

为你的 Go `GET /healthz` 写 checker：成功打印 JSON；超时、非 200、无效 JSON 分别返回非零退出码。

## 常见误区

- 把脚本一次跑通当成工程正确，忽略环境隔离、输入校验、错误分类和可重复运行。
- 只读 API 名称而不为文件、网络、时间等外部边界写一个可控测试。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
01-HTTP Automation and API Checks (HTTP自动化与接口检查)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
