---
study_stage: backlog
tags: [language/python, python/data]
---

> [!abstract] 学习定位
> Python 很适合离线分析、清洗、报告与自动化；线上高并发核心路径应基于性能、延迟、团队和已有系统选择语言，而非“Python 写得快”。

> [!note] 先测量，再迁移
> 数据量大或运行慢不自动意味着要换语言。先确认瓶颈是网络、数据库、算法、I/O 还是 Python 代码本身；很多任务用流式处理、索引或查询下推就能解决。

# 数据契约与可复现性

先写输入契约：字段名、类型、单位、时区、空值、唯一键、编码和允许范围。CSV 的空字符串、缺失列和字符串 `"NULL"` 不是同一概念；JSON number 也不能自动代表业务金额。解析边界将原始值转换成明确类型，非法记录进入错误报告而不是静默填零。

每次分析保存输入文件哈希或数据版本、程序版本、参数、开始时间和输出 schema。输出使用稳定排序，浮点格式和时区明确，使同一输入可以比较。随机抽样固定 seed，但同时说明 seed 只保证工具与算法稳定时的可复现性。

# 聚合、分位数与内存

平均值容易被极端值影响；延迟分析通常同时报告样本数、错误率和 p50/p95/p99。必须说明 percentile 算法和窗口，因为不同工具的近似算法可能不同。样本太少时，p99 没有稳定解释价值。

精确排序全部值需要 O(n) 内存和 O(n log n) 时间。大数据可使用流式计数、分桶、近似分位数或把聚合下推到数据库。先估算输入规模与聚合状态，不要等进程 OOM 才决定架构。

# 小型持久化与 SQLite

数据超过单文件脚本的查询能力，但又不需要独立数据库服务时，SQLite 是有用边界：支持事务、索引和 SQL，文件也便于携带。它不是高并发网络数据库；多个 writer、长事务和共享网络文件系统需要专门评估。

写入使用事务，批量参数化执行，不用字符串拼 SQL。为重复导入设计唯一键或 upsert，并记录源版本，避免脚本重跑制造重复数据。

# 验证结果

为统计结果准备可人工核对的小样本，手算期望值；再用真实规模测试时间和内存。检查记录总数守恒、唯一键覆盖、金额/计数范围和错误记录数量。报告不仅给结果，还要说明排除了什么数据以及为什么。

练习：读取压测 CSV，校验字段和时区，按 endpoint 输出请求数、错误率和 p50/p95/p99。先对十行样本手算，再生成百万行输入测量峰值内存；若超限，改为分块或 SQLite 聚合并比较结果。
# 一个可检查的 CSV 聚合

下面的程序只保存按 endpoint 聚合的状态，不把整份输入读入内存；它仍可能因为 endpoint 种类无限增加而耗尽内存，因此设置维度上限。

```python
import csv
from collections import Counter
from pathlib import Path

def count_statuses(path: Path, max_endpoints: int = 10_000) -> dict[str, tuple[int, int]]:
    if max_endpoints < 1:
        raise ValueError("max_endpoints must be positive")
    total: Counter[str] = Counter()
    failed: Counter[str] = Counter()
    with path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        if not {"endpoint", "status"} <= set(reader.fieldnames or ()):
            raise ValueError("CSV needs endpoint and status columns")
        for line_no, row in enumerate(reader, start=2):
            endpoint = row["endpoint"]
            if not endpoint:
                raise ValueError(f"line {line_no}: empty endpoint")
            try:
                status = int(row["status"])
            except (TypeError, ValueError) as exc:
                raise ValueError(f"line {line_no}: invalid status") from exc
            if not 100 <= status <= 599:
                raise ValueError(f"line {line_no}: status out of range")
            if endpoint not in total and len(total) >= max_endpoints:
                raise ValueError("too many distinct endpoints")
            total[endpoint] += 1
            failed[endpoint] += status >= 500
    return {key: (total[key], failed[key]) for key in sorted(total)}
```

这段代码的“失败”定义为 5xx；是否把 4xx、timeout 或业务错误计入失败，必须先写入报告契约。小样本先手算总数、失败数和错误率；分位数另需明确算法与窗口，不能从上述两个计数反推出 p95。

# 动手验证

准备十行压测 CSV，覆盖缺列、空 endpoint、非法 status 与超过维度上限。先手算预期，再运行并对照；对百万行输入测峰值内存。如果维度上限确实不足，再比较数据库聚合、外部排序或近似算法，而不是直接删掉边界。
