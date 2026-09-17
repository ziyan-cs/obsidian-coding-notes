---
status: stable
confidence: high
verified: 2026-09-17
review_due: 2026-09-25
---

> [!abstract] 一句话结论：异常表达失败语义，context manager 保证资源释放，类型标注把接口约束前置到阅读与检查阶段。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：只捕获能处理的异常并保留上下文；用 `with` 把获取与释放写在同一作用域；用类型标注配合检查器提前发现接口不匹配。

# 三条规则

1. 只捕获你能恢复或能补充上下文的异常；不要裸 `except` 吞掉错误。
2. 文件、连接、锁等资源优先写进 `with`，让退出路径天然释放资源。
3. 对公共函数标注输入、输出与可空性；类型不是运行时验证的替代品。

# 后端辅助中的用法

- 读取配置失败：抛出含文件路径和字段名的异常。
- 批量调用 API：区分网络瞬态失败、业务拒绝与数据格式错误。
- 数据转换函数：用类型标注暴露期望 schema，必要时配合运行时校验。

# 三件工具如何配合

```python
from pathlib import Path

def load_name(path: Path) -> str:
    try:
        with path.open(encoding="utf-8") as f:
            return f.read().strip()
    except OSError as exc:
        raise RuntimeError(f"cannot read {path}") from exc
```

`with` 保证文件关闭；异常链 `from exc` 保留底层原因；`Path -> str` 标注让调用契约更清楚。类型检查能发现接口不匹配，但它不会替你验证 JSON 字段、网络响应或用户输入；这些需要显式的运行时校验。

# 边界

库代码通常应抛出有语义的异常，让应用入口统一记录日志和决定 HTTP/CLI 返回码。若每一层都记录同一异常，会制造重复日志；若捕获后返回 `None`，又会把真正的失败伪装成正常分支。

> [!question]- 自测：先回答再展开
> 1. `finally` 与 context manager 分别解决什么问题？
> 2. 哪种情况应当重新抛出异常，而不是记录日志后继续？
