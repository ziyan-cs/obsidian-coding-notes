---
study_stage: backlog
---

> [!note] 方法论坐标
> 测试证据设计与可观测性语义统一见 [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md) 和 [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)；本篇聚焦 GoogleTest/GMock、spdlog、Prometheus 与 OpenTelemetry C++。


# 测试：从契约到证据

测试的目标是**证明一个可观察行为在给定条件下成立**，不是为了填覆盖率数字。先列出输入、预期输出与失败边界，再选测试层级；有外部依赖的路径还要验证资源回收、超时和错误传播。

| 层级 | 主要验证 | 典型依赖 |
| --- | --- | --- |
| 单元测试 | 纯逻辑、状态转换、局部不变量 | 尽量可控，不必模拟所有协作者 |
| 集成测试 | SQL、网络协议、文件系统或模块间真实契约 | 临时库、测试容器、真实进程 |
| 端到端测试 | 从入口到出口的关键用户路径 | 接近部署环境，数量受成本约束 |

“单元测试一定毫秒级、E2E 一定分钟级”不是规则；按项目真实运行时间与失败定位成本安排比例。

## GoogleTest：最小可运行行为测试

以下代码只依赖 GoogleTest，不暗含未定义的 `Database` 或 `Service` 类型。`ASSERT_*` 失败时终止**当前测试函数**；`EXPECT_*` 记录失败并继续执行。

```cpp
#include <gtest/gtest.h>
#include <vector>

int sum(const std::vector<int>& xs) {
    int result = 0;
    for (int x : xs) result += x;
    return result;
}

TEST(SumTest, EmptyInputIsZero) {
    EXPECT_EQ(sum({}), 0);
}

TEST(SumTest, AcceptsPositiveAndNegativeValues) {
    EXPECT_EQ(sum({2, -3, 5}), 4);
}
```

需要共享初始化代码时使用 fixture，但 **每个 `TEST_F` 都创建独立 fixture 对象**，不是把同一个数据库状态共享给全部测试。

```cpp
class SumFixture : public ::testing::Test {
protected:
    std::vector<int> input{2, -3, 5};
};

TEST_F(SumFixture, ComputesKnownCase) {
    ASSERT_EQ(input.size(), 3u);
    EXPECT_EQ(sum(input), 4);
}
```

## 什么时候使用 Mock

Mock 用来隔离不可控或昂贵的边界，例如支付网关、时钟、远端服务；不要把内部每次函数调用都锁死，否则重构时测试会无谓失败。先验证对外结果，只有协议明确规定调用次数/顺序时才断言交互。

```cpp
#include <gmock/gmock.h>
#include <string>

struct Store {
    virtual ~Store() = default;
    virtual bool save(const std::string& key, int value) = 0;
};
struct MockStore : Store {
    MOCK_METHOD(bool, save, (const std::string&, int), (override));
};
bool persist(Store& store, int value) {
    return store.save("answer", value);
}
TEST(PersistTest, ReportsStorageFailure) {
    MockStore store;
    EXPECT_CALL(store, save("answer", 42))
        .WillOnce(::testing::Return(false));
    EXPECT_FALSE(persist(store, 42));
}
```

## 执行、框架版本与覆盖率

CMake/CTest 中注册测试后用 `ctest --test-dir build --output-on-failure` 运行；测试应能在干净环境重复执行，不依赖上一用例的残留状态。失败时保存最小复现输入、编译参数与日志，再修代码。

Catch2 v3 使用 `<catch2/catch_test_macros.hpp>`；若使用框架提供的 `main`，CMake 链接 `Catch2::Catch2WithMain`。旧版的 `CATCH_CONFIG_MAIN` + `<catch2/catch.hpp>` 不应直接作为 v3 示例复制。

覆盖率只回答“哪些路径被运行过”，不回答断言是否有效。优先补边界值、错误路径、并发关闭、重试幂等和真实依赖的集成测试；给 Sanitizer 单独测试配置。参考 [GoogleTest Primer](https://github.com/google/googletest/blob/main/docs/primer.md) 与 [Catch2 v3 CMake 集成](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md)。

---

# 可观测性：从症状追到原因

一次请求“慢了”不是一个足够明确的问题。先用指标确定**何时、哪些路由、影响多少请求**；再用日志查具体失败上下文，用 trace 观察跨服务耗时分布。三者互补，但不要求每个小程序从第一天就部署完整平台。

| 信号 | 回答的问题 | 最小字段/维度 |
| --- | --- | --- |
| 日志（logs） | 这次请求发生了什么异常？ | 时间、级别、服务、request/trace ID、错误上下文 |
| 指标（metrics） | 故障影响面和趋势如何？ | 请求率、错误率、延迟分布、队列/连接池状态 |
| 追踪（traces） | 时间花在调用链的哪一段？ | trace/span ID、父子关系、服务名、关键事件 |

## 日志：清楚、可关联、可控成本

`spdlog` 支持同步与异步 logger。下面是独立的同步示例；调用日志库前应按项目构建配置安装并链接对应版本。

```cpp
#include <spdlog/spdlog.h>

int main() {
    const int port = 8080;
    spdlog::info("server started port={}", port);
    spdlog::warn("request timed out route={} trace_id={}",
                 "/orders", "example-trace-id");
}
```

在请求入口生成或接收 request/trace ID，并贯穿该请求的日志。避免记录密码、令牌、完整个人信息；高频路径控制日志量。异步日志**并非零开销或绝不阻塞**：队列满时默认策略可阻塞调用者，改为丢弃策略则可能丢日志；还要定义退出时的 flush 与故障降级。参考 [spdlog async logger 源码](https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/async_logger.h)。

## 指标：分母、单位、维度

从服务契约出发定义最小指标集：

- `http_requests_total`：Counter，按**模板路由**、方法、结果类别统计，计算请求率和错误率。
- `http_request_duration_seconds`：Histogram，观测一次请求的秒数；经典直方图导出 `_bucket`、`_sum`、`_count`，可用 PromQL 聚合估算 P99。
- `requests_in_flight`：Gauge，表示当前正在处理的请求。
- `up` 是 Prometheus 抓取目标时生成的可达性信号，**不是**业务进程自行维护的“健康=1/0”。可达不等于业务健康。

不要把用户 ID、完整 URL、订单号直接放进 label，否则时序数量会随用户/请求增长。P99 也不是“某一个 bucket 的值”；经典直方图需按 `le` 聚合后用 `histogram_quantile` 估算，准确度受 bucket 边界影响。参考 [Prometheus metric types](https://prometheus.io/docs/concepts/metric_types/)。

## 追踪：传播上下文，再分析路径

入口收到 `traceparent` 时提取上下文；新建 span 后把上下文注入下游 HTTP header 或 gRPC metadata。仅在本地创建 span、却不传播父上下文，跨服务图仍会断开。对数据库或 RPC 子操作建子 span，并记录耗时、错误状态与必要属性；避免把敏感数据塞进属性。

C++ OpenTelemetry 的 SDK、Exporter、传输与框架集成是**不同层**。文中的步骤是接入顺序，不是可直接复制的“自动 gRPC 拦截器”代码：

1. 选定当前 OpenTelemetry C++ 与 instrumentation 包版本，配置 provider/exporter。
2. 在服务入口提取上游 context、创建 span scope；出站调用注入 context。
3. 发一条测试请求，在后端确认父子关系、错误标签和采样结果。
4. 在负载下测量埋点成本，再决定采样率和数据保留周期。

依赖项目自己的框架集成能力，不假设每个 gRPC 客户端都存在同名“自动拦截器”。参考 [OpenTelemetry C++ instrumentation 文档](https://opentelemetry.io/docs/languages/cpp/instrumentation/)。

## 落地与排障次序

先让服务具备**请求计数、错误率、延迟直方图和结构化错误日志**。跨服务调用增多后补 trace。一次故障按“告警窗口 → 指标定位受影响路由 → trace 找耗时段 → 日志核对失败上下文 → 构造回归测试”闭环；监控不替代测试，也不保证单次故障总有完整 trace。

---

> [!info]- 延伸阅读
> - 下一步：[05-Profiling and Optimization (性能分析与优化)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/06-Engineering%20Practice%20(工程实践)/05-Profiling%20and%20Optimization%20(性能分析与优化).md)
