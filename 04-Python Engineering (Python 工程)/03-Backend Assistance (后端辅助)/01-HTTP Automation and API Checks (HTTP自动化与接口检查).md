---
study_stage: backlog
tags: [language/python, python/http, testing/api]
---

> [!abstract] 学习定位
> 接口检查工具要验证 status、body、超时和失败信息；绝不能无超时地发请求，也不能把 token 打进日志。

> [!warning] 自动化脚本也属于生产边界
> 脚本的失败输出应足够让人定位问题，但不能泄露 `Authorization`、Cookie、token 或完整用户数据。把 URL、timeout 与凭据来源显式配置，避免“只在我的电脑能跑”。

```python
import json
from urllib.error import HTTPError, URLError
from urllib.parse import urlsplit
from urllib.request import Request, urlopen

MAX_BODY = 1_048_576  # 1 MiB；由服务契约决定

def health(url: str, timeout: float = 3.0) -> dict[str, object]:
    if timeout <= 0:
        raise ValueError("timeout must be positive")
    host = urlsplit(url).hostname or "<unknown>"
    request = Request(url, headers={"Accept": "application/json"})
    try:
        with urlopen(request, timeout=timeout) as response:
            if response.status != 200:
                raise RuntimeError(f"unexpected HTTP {response.status} from {host}")
            media_type = response.headers.get_content_type()
            if media_type != "application/json" and not media_type.endswith("+json"):
                raise ValueError(f"unexpected content type from {host}")
            body = response.read(MAX_BODY + 1)
    except HTTPError as exc:
        raise RuntimeError(f"HTTP {exc.code} from {host}") from exc
    except URLError as exc:
        raise RuntimeError(f"network error for {host}") from exc
    if len(body) > MAX_BODY:
        raise ValueError(f"response too large from {host}")
    try:
        value = json.loads(body)
    except (UnicodeError, json.JSONDecodeError) as exc:
        raise ValueError(f"invalid JSON from {host}") from exc
    if not isinstance(value, dict):
        raise ValueError(f"expected JSON object from {host}")
    return value
```

# 请求生命周期与预算

一次 HTTP 调用可能在 DNS、建连、TLS、等待响应头和读取 body 任一阶段失败。`urlopen(timeout=...)` 约束底层阻塞操作，不能把它误认为整次请求的严格总 deadline；整体预算与取消需要在调用层另行设计。

复用 client/session 可以复用连接池，避免每次重新握手。连接池也可能成为排队点，应限制并发并记录池等待、首字节和总耗时。客户端任务被取消后要停止后续重试和读取。

# 重试、限流与幂等

只对明确的瞬态故障重试，并设置最大次数、总 deadline、指数退避和 jitter。400 类业务/输入错误通常不应重试；429 依据服务端限流提示；认证失败先修复凭据。POST 已在服务端完成但响应丢失时，盲目重试可能重复扣款或创建资源，因此使用业务幂等键或查询状态。

批量检查必须有有界并发。若服务端已经过载，无界 task 和多层重试会形成重试风暴。报告原始失败分类，不把最终“重试成功”完全隐藏，否则无法发现系统正在退化。

# 响应契约与分页

先限制允许读取的 body 大小，再按 `Content-Type` 解码。校验 status、必要 header、JSON schema、字段范围和业务不变量；200 携带错误对象仍可能是失败，204 则本来就没有 body。

分页循环必须处理游标终止、重复页、最大页数和中途失败。结果很大时逐页处理或落盘，不在内存积累全部对象。保存断点前要明确重复处理是否幂等。

# TLS、认证与日志

默认验证证书和主机名，不用 `verify=False` 修复证书问题。token 从环境或秘密系统注入，限制作用域和有效期；日志记录 host、method、status、耗时和 request ID，但不记录 Authorization、Cookie 和敏感 body。

上面的 checker 假设 URL 来自可信配置。若 URL 可由不可信用户提供，必须额外限制 scheme、目标 host/IP 与重定向，并考虑 DNS 变化；仅用 `urlsplit` 取出 host 供错误提示，**不能**防止 SSRF。

# 测试矩阵

用本地测试 server 或传入 fake transport 覆盖：成功、非 2xx、非法 JSON、body 过大、连接 timeout、读取 timeout、429、重复游标和客户端取消。断言重试不越过总预算，非幂等请求不会自动重复执行。
# 动手验证

为一个本地 `GET /healthz` 写 checker，并用本地测试 server 覆盖：200 对象、200 数组、非 JSON、超过上限、非 2xx、连接失败和超时。CLI 应将失败映射为非零退出码，只输出脱敏 host、分类和 request ID。上例不自动重试；若要重试，应单独设计总 deadline、幂等前提与退避策略。

> [!info]- 延伸阅读
> - 下一步：[02-Data Processing Boundaries (数据处理边界)](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/02-Data%20Processing%20Boundaries%20(数据处理边界).md)
