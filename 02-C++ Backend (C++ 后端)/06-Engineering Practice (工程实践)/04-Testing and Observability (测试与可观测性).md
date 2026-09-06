---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# Testing & Mocking (测试与模拟)

> [!note] 本节重点：核心考点：单元测试框架、测试金字塔、Mock 对象、TDD 基础、CI 集成

# 测试金字塔

```text
test pyramid
  - many: unit tests for functions and classes
  - some: integration tests for module boundaries
  - few: end-to-end tests for user journeys
```

| 层级 | 速度 | 数量 | 维护成本 |
|------|------|------|---------|
| 单元测试 | 毫秒级 | 多 | 低 |
| 集成测试 | 秒级 | 中 | 中 |
| E2E 测试 | 分钟级 | 少 | 高 |

# Google Test 基础

```cpp
#include <gtest/gtest.h>

// 测试一个函数
int add(int a, int b) { return a + b; }

TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(add(1, 2), 3);
    EXPECT_EQ(add(10, 20), 30);
}

TEST(AddTest, NegativeNumbers) {
    EXPECT_EQ(add(-1, -2), -3);
    EXPECT_EQ(add(-5, 5), 0);
}

// 测试 Fixture（多个测试共享设置）
class MyTest : public ::testing::Test {
protected:
    void SetUp() override { /* 每个 TEST_F 前执行 */ }
    void TearDown() override { /* 每个 TEST_F 后执行 */ }
    Database db_;  // 共享成员
};

TEST_F(MyTest, CanInsert) {
    EXPECT_TRUE(db_.insert("key", "value"));
}

TEST_F(MyTest, CanQuery) {
    db_.insert("key", "value");
    EXPECT_EQ(db_.query("key"), "value");
}
```

# 断言速查

| 断言 | 用途 |
|------|------|
| `EXPECT_TRUE(cond)` / `EXPECT_FALSE(cond)` | 布尔条件 |
| `EXPECT_EQ(a, b)` / `EXPECT_NE(a, b)` | 相等/不等 |
| `EXPECT_LT(a, b)` / `EXPECT_GT(a, b)` | 小于/大于 |
| `EXPECT_STREQ(s1, s2)` | C 字符串相等 |
| `EXPECT_THROW(stmt, exception_type)` | 预期抛异常 |
| `EXPECT_NO_THROW(stmt)` | 预期不抛异常 |
| `EXPECT_NEAR(a, b, eps)` | 浮点数近似相等 |

`ASSERT_*` 版本在失败时**终止当前测试**（而非继续）。`EXPECT_*` 则继续执行后续行。

# GMock

```cpp
#include <gmock/gmock.h>

// 1. 定义接口
class Database {
public:
    virtual ~Database() = default;
    virtual bool save(const std::string& key, int value) = 0;
    virtual int load(const std::string& key) = 0;
};

// 2. 使用 GMock 生成 Mock
class MockDatabase : public Database {
public:
    MOCK_METHOD(bool, save, (const std::string&, int), (override));
    MOCK_METHOD(int, load, (const std::string&), (override));
};

// 3. 测试中使用
TEST(ServiceTest, SaveData) {
    MockDatabase mock_db;
    EXPECT_CALL(mock_db, save("key", 42))
        .Times(1)
        .WillOnce(testing::Return(true));

    Service svc(&mock_db);
    EXPECT_TRUE(svc.saveData("key", 42));
}
```

## GMock 匹配器

```cpp
using ::testing::_;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Return;

EXPECT_CALL(mock, method(_, Ge(10)))      // 任何第一个参数 + >=10 的第二个参数
    .Times(AtLeast(1))
    .WillRepeatedly(Return(0));

EXPECT_CALL(mock, method(Eq("exact"), _))
    .Times(Exactly(2));                   // 精确调用 2 次
```

# Catch2 轻量测试框架

```cpp
// Catch2 的集成方式随主版本与包管理方式不同；按当前官方文档配置
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("vectors can be resized") {
    std::vector<int> v;
    v.resize(10);
    REQUIRE(v.size() == 10);
    REQUIRE(v.capacity() >= 10);

    SECTION("resizing bigger changes size and capacity") {
        v.resize(15);
        REQUIRE(v.size() == 15);
    }

    SECTION("resizing smaller changes size but not capacity") {
        v.resize(5);
        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 10);  // vector 不会缩小 capacity
    }
}
```

# 测试驱动开发（TDD）流程

```text
1. 写一个失败的测试（RED）
2. 写最简代码让测试通过（GREEN）  
3. 重构代码（REFACTOR）
4. 重复
```

# 测试覆盖率

```bash
g++ -coverage main.cpp -o main
./main
gcov main.cpp   # 生成 .gcov 文件
lcov -c -d . -o coverage.info
genhtml -o report coverage.info
```

# 工程实践

```cpp
// ✅ 测试应当：
// - 独立（不依赖外部状态）
// - 可重复（每次运行结果一致）
// - 快速（毫秒级）
// - 有好的失败信息（EXPECT_EQ(a, b) 比 EXPECT_TRUE(a==b) 好）

// ❌ 不要测试：
// - 内部实现细节（应测试公开接口的行为）
// - 第三方库的行为
// - 简单到不可能出错的代码

// ✅ 测试文件组织
// 建议：测试文件与源文件一一对应
// src/module.cpp → tests/module_test.cpp
```

> [!tip]- **工程要点**：好的单元测试是“**活的文档**”——阅读测试代码就能理解模块的预期行为。CMake/CTest 项目通常用 `ctest --test-dir build --output-on-failure` 执行已注册测试；具体 target 名称由项目定义。测试是工程质量的基线，但也要避免过度 mock 而失去真实集成覆盖。

# 30 秒回答

单元测试验证一个明确行为并隔离外部依赖；集成测试验证模块协作；E2E 只覆盖少量关键用户路径。Mock 应替换不可控、昂贵或难复现的边界，不应用来断言内部调用细节。失败信息、可重复性和快速反馈比单纯覆盖率数字更重要。

---

测试与调试配合保障代码质量，详见 → [Debugging gdb & Sanitizers (调试工具)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/05-Debugging%20gdb%20&%20Sanitizers%20(调试工具)%20⭐.md)

---

# Observability Logging Metrics and Tracing (可观测性)

> [!note] 本节重点：核心考点：spdlog 日志库、Prometheus metrics 暴露、OpenTelemetry 链路追踪、C++ 服务可观测性的三板斧

# 可观测性三大支柱

```
可观测性（Observability）
├── 日志（Logging）     → spdlog / loguru          → 发生了什么
├── 指标（Metrics）     → Prometheus + grafana     → 趋势与告警
└── 链路追踪（Tracing） → OpenTelemetry + Jaeger   → 请求全链路
```

三者互补，缺一不可。

---

# 日志（Logging）：spdlog

C++ 中最常用的高性能日志库，header-only 可选，支持异步、多 sinks、格式化。

## 快速上手

```cpp
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

int main() {
    // 控制台日志（彩色输出）
    auto console = spdlog::stdout_color_mt("console");
    console->set_level(spdlog::level::info);
    console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

    // 文件日志（按大小轮转，保留 5 个文件，每个 10MB）
    auto file = spdlog::rotating_logger_mt("file", "logs/server.log", 10 << 20, 5);

    // 异步日志（日志写入在后台线程，不阻塞业务）
    spdlog::init_thread_pool(8192, 1);  // 队列 8192，1 个后台线程
    auto async_logger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(
        "async_log", "logs/async.log", 10 << 20, 5);

    // 使用
    spdlog::info("Server started on port {}", 8080);
    spdlog::warn("Connection timeout: client_id={}", clientId);
    spdlog::error("Database query failed: {}", err.what());

    // 每 5 分钟打印一条请求量日志
    static std::atomic<uint64_t> req_count{0};
    // 在请求处理中：req_count++;
    // 定时输出：
    spdlog::info("Request count last 5min: {}", req_count.load());
    req_count = 0;
}
```

## 日志最佳实践

| 级别 | 用途 | 频率 |
|------|------|------|
| `ERROR` | 需要立即处理的故障（DB 连接失败） | 告警级别 |
| `WARN` | 值得关注但不影响运行（慢查询 > 1s） | 每分钟数次 |
| `INFO` | 关键生命周期事件（启动、关闭、配置变更） | 低频 |
| `DEBUG` | 调试用，线上关闭 | 线下开启 |

> **不要打每一行请求的 INFO 日志**，高 QPS 下日志本身会成为性能瓶颈。关键路径用 Metrics 计数替代日志。

---

# 指标（Metrics）：Prometheus

C++ 服务暴露 HTTP `/metrics` 端点，Prometheus 定期拉取。

## Prometheus C++ Client 集成

```cpp
#include <prometheus/counter.h>
#include <prometheus/histogram.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

using namespace prometheus;

// 全局 Registry
auto registry = std::make_shared<Registry>();

// 定义指标
auto& req_total = BuildCounter()
    .Name("http_requests_total")
    .Help("Total HTTP requests")
    .Register(*registry)
    .Add({{"method", "GET"}, {"path", "/api/user"}});

auto& req_duration = BuildHistogram()
    .Name("http_request_duration_seconds")
    .Help("Request duration in seconds")
    .Register(*registry)
    .Add({}, Histogram::BucketBoundaries{0.005, 0.01, 0.025, 0.05, 0.1,
          0.25, 0.5, 1, 2.5, 5, 10});

// 启动 HTTP 暴露端点（端口 8080）
Exposer exposer{"0.0.0.0:8080"};
exposer.RegisterCollectible(registry);

// 在请求处理中使用
void handleRequest() {
    auto start = std::chrono::steady_clock::now();

    req_total.Increment();  // 计数 +1

    // ... 处理请求 ...

    auto dur = std::chrono::steady_clock::now() - start;
    req_duration.Observe(std::chrono::duration<double>(dur).count());
}
```

## 核心指标（RED 方法）

| 指标 | 类型 | 说明 |
|------|------|------|
| `requests_total` | Counter | 请求总量（按 method/path/status 分） |
| `request_duration_seconds` | Histogram | 请求延迟分布（P50/P90/P99） |
| `requests_in_flight` | Gauge | 当前正在处理的请求数 |
| `errors_total` | Counter | 错误请求数 |
| `up` | Gauge | 服务存活（1=正常，0=挂） |

---

# 链路追踪（Tracing）：OpenTelemetry

跨多个微服务追踪一次请求的完整路径。

## OpenTelemetry C++ 集成

```cpp
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/exporters/ostream/span_exporter.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>

namespace trace = opentelemetry::trace;
namespace sdktrace = opentelemetry::sdk::trace;

// 初始化（使用 Jaeger Exporter）
void initTracing() {
    auto exporter = std::make_unique<sdktrace::OStreamSpanExporter>();
    auto processor = std::make_unique<sdktrace::SimpleSpanProcessor>(std::move(exporter));
    auto provider = std::make_unique<sdktrace::TracerProvider>(std::move(processor));
    trace::Provider::SetTracerProvider(std::move(provider));
}

// 在请求入口创建 Span
void handleRequest(const Request& req) {
    auto tracer = trace::Provider::GetTracerProvider()->GetTracer("user-service");

    // 从 HTTP header 提取父 Span context（跨服务传播）
    auto ctx = /* 从 req.headers() 中提取 traceparent */;
    auto span = tracer->StartSpan("handle_get_user", ctx);

    // 添加属性
    span->SetAttribute("user_id", req.user_id);
    span->AddEvent("db.query.start");

    auto scope = tracer->WithActiveSpan(span);
    User user = db.query(req.user_id);

    span->AddEvent("db.query.end");
    span->End();
}
```

## gRPC 自动追踪

```cpp
// 使用 OpenTelemetry gRPC 拦截器自动追踪
// 无需手动在每个 RPC 中创建 span
auto channel = grpc::CreateChannel(target, creds);
auto tracing_channel = opentelemetry::trace::Propagation::GetPropagation()
    .Inject(channel);
auto stub = UserService::NewStub(tracing_channel);
```

---

# 三板斧整合架构

```
客户端请求
    │
    ▼
┌────────────────────────┐
│  接入层                │
│  - 创建 Span（Tracing）│
│  - 记录 Request Count  │
│  - 计时 Duration       │
└───────┬───────────────┘
        │
        ▼
┌─────────────────────────┐
│  业务逻辑               │
│  - INFO 日志（关键事件）│
│  - ERROR 日志（异常）   │
│  - 嵌套 Span（子操作）  │
└───────┬───────────────┘
        │
        ▼
┌───────────────────────┐
│  后端服务/DB          │
│  - 慢查询 Warning     │
│  - 连接池 Metrics     │
└───────────────────────┘

输出：
  /metrics  → Prometheus + Grafana（监控面板）
  stdout    → ELK/Loki（日志检索）
  Jaeger    → 链路查询（慢请求定位）
```

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| 日志级别使用 | ERROR=故障、WARN=异常、INFO=事件、DEBUG=调试 |
| 为什么异步日志 | 日志 IO 不阻塞业务线程，spdlog 异步模式下写日志 ≈ 0 开销 |
| Metrics 三大类型 | Counter（累计）、Gauge（瞬时）、Histogram（分布） |
| P99 延迟计算 | Histogram bucket 统计，PromQL: `histogram_quantile(0.99, ...)` |
| Trace 传播方式 | HTTP: `traceparent` header, gRPC: metadata, 跨线程: Context 传递 |
| 三大支柱缺一不可 | 日志查单次异常、Metrics 看趋势告警、Tracing 串起全链路 |

> [!tip]- **工程要点**：C++ 后端服务可观测性三板斧——spdlog（日志）、prometheus-cpp（指标）、opentelemetry-cpp（追踪）。优先级：日志 > 指标 > 追踪。至少先上日志和基础指标（QPS、延迟、错误率），再逐步补充链路追踪。不要试图一开始就全量追踪，先追踪关键接口（用户登录、下单等核心链路）。

---

可观测性与调试工具共同保障服务稳定性，详见 → [Debugging gdb & Sanitizers (调试工具)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/05-Debugging%20gdb%20&%20Sanitizers%20(调试工具)%20⭐.md)



# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **04-Testing and Observability (测试与可观测性)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
