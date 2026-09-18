---
status: learning
confidence: low
content_verified: 2026-09-17
previous_review_due: 2026-10-01
tags: [language/python, python/http, testing/api]
---

> [!abstract] 学习定位
> 接口检查工具要验证 status、body、超时和失败信息；绝不能无超时地发请求，也不能把 token 打进日志。

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

# 请求生命周期与预算

一次 HTTP 调用可能在 DNS、建连、TLS、等待响应头和读取 body 任一阶段失败。timeout 不是“网络慢的重试按钮”，而是调用方愿意等待的预算；连接 timeout 与整体 deadline 应按客户端能力分别配置。

复用 client/session 可以复用连接池，避免每次重新握手。连接池也可能成为排队点，应限制并发并记录池等待、首字节和总耗时。客户端任务被取消后要停止后续重试和读取。

# 重试、限流与幂等

只对明确的瞬态故障重试，并设置最大次数、总 deadline、指数退避和 jitter。400 类业务/输入错误通常不应重试；429 依据服务端限流提示；认证失败先修复凭据。POST 已在服务端完成但响应丢失时，盲目重试可能重复扣款或创建资源，因此使用业务幂等键或查询状态。

批量检查必须有有界并发。若服务端已经过载，无界 task 和多层重试会形成重试风暴。报告原始失败分类，不把最终“重试成功”完全隐藏，否则无法发现系统正在退化。

# 响应契约与分页

先限制允许读取的 body 大小，再按 `Content-Type` 解码。校验 status、必要 header、JSON schema、字段范围和业务不变量；200 携带错误对象仍可能是失败，204 则本来就没有 body。

分页循环必须处理游标终止、重复页、最大页数和中途失败。结果很大时逐页处理或落盘，不在内存积累全部对象。保存断点前要明确重复处理是否幂等。

# TLS、认证与日志

默认验证证书和主机名，不用 `verify=False` 修复证书问题。token 从环境或秘密系统注入，限制作用域和有效期；日志记录 host、method、status、耗时和 request ID，但不记录 Authorization、Cookie 和敏感 body。

# 测试矩阵

用本地测试 server 或传入 fake transport 覆盖：成功、非 2xx、非法 JSON、body 过大、连接 timeout、读取 timeout、429、重复游标和客户端取消。断言重试不越过总预算，非幂等请求不会自动重复执行。
# 检查维度

- 网络：域名、连接、timeout、TLS。
- 协议：method、status、header、JSON 格式。
- 业务：必须字段、错误码、响应时间阈值。
- 安全：token 从环境变量/安全存储读取；日志中脱敏。

> [!summary] 核心摘要
>
> Python 很适合做黑盒 API checker：发一个可配置、带 timeout 的请求，检查协议层（状态码、header、JSON）与业务层（字段、错误码、阈值），在失败时以非零退出码和脱敏上下文结束。它验证的是服务可观察到的行为，不替代服务端单元测试。

> [!question]- 自测：先回答再展开
> 1. `HTTPError` 和 `URLError` 分别代表哪一类失败？为什么要分别报告？
> 2. 为什么“请求成功返回 200”还不足以判定接口健康？
> 3. 如何让命令行脚本在健康检查失败时返回非零退出码？

# C++ / Go 对照

Python 适合写黑盒 API checker、回归脚本和压测结果处理；服务实现仍优先放在 Go/C++。工具必须保留可配置 URL、timeout 和明确失败输出，不能只在你电脑上“碰巧能跑”。

# 练习

为你的 Go `GET /healthz` 写 checker：成功打印 JSON；超时、非 200、无效 JSON 分别返回非零退出码。

> [!info]- 延伸阅读
> - 下一步：[02-Data Processing Boundaries (数据处理边界)](/04-Python%20Engineering%20(Python%20工程)/03-Backend%20Assistance%20(后端辅助)/02-Data%20Processing%20Boundaries%20(数据处理边界).md)


