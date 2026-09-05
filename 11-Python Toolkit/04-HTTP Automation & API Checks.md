---
tags: [language/python, python/http, testing/api]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# HTTP Automation & API Checks — 用 Python 验证服务

> **一句话结论**：接口检查工具要验证 status、body、超时和失败信息；绝不能无超时地发请求，也不能把 token 打进日志。

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

## C++ / Go 对照

Python 适合写黑盒 API checker、回归脚本和压测结果处理；服务实现仍优先放在 Go/C++。工具必须保留可配置 URL、timeout 和明确失败输出，不能只在你电脑上“碰巧能跑”。

## 练习

为你的 Go `GET /healthz` 写 checker：成功打印 JSON；超时、非 200、无效 JSON 分别返回非零退出码。
