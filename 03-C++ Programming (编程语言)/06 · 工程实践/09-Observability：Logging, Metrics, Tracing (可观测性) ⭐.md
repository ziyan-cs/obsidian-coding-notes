---
tags:
  - cpp
  - engineering
---

> **核心考点**：spdlog 日志库、Prometheus metrics 暴露、OpenTelemetry 链路追踪、C++ 服务可观测性的三板斧

## 可观测性三大支柱

```
可观测性（Observability）
├── Log（Logging）     → spdlog / loguru          → 发生了什么
├── Metrics（Metrics）     → Prometheus + grafana     → 趋势与Alert
└── Tracing（Tracing） → OpenTelemetry + Jaeger   → Request全链路
```

三者互补，缺一不可。

---

## 日志（Logging）：spdlog

C++ 中最常用的高性能日志库，header-only 可选，支持异步、多 sinks、格式化。

### 快速上手

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

### 日志最佳实践

| 级别 | 用途 | 频率 |
|------|------|------|
| `ERROR` | 需要立即处理的故障（DB 连接失败） | 告警级别 |
| `WARN` | 值得关注但不影响运行（慢查询 > 1s） | 每分钟数次 |
| `INFO` | 关键生命周期事件（启动、关闭、配置变更） | 低频 |
| `DEBUG` | 调试用，线上关闭 | 线下开启 |

> **不要打每一行请求的 INFO 日志**，高 QPS 下日志本身会成为性能瓶颈。关键路径用 Metrics 计数替代日志。

---

## 指标（Metrics）：Prometheus

C++ 服务暴露 HTTP `/metrics` 端点，Prometheus 定期拉取。

### Prometheus C++ Client 集成

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

### 核心指标（RED 方法）

| 指标 | 类型 | 说明 |
|------|------|------|
| `requests_total` | Counter | 请求总量（按 method/path/status 分） |
| `request_duration_seconds` | Histogram | 请求延迟分布（P50/P90/P99） |
| `requests_in_flight` | Gauge | 当前正在处理的请求数 |
| `errors_total` | Counter | 错误请求数 |
| `up` | Gauge | 服务存活（1=正常，0=挂） |

---

## 链路追踪（Tracing）：OpenTelemetry

跨多个微服务追踪一次请求的完整路径。

### OpenTelemetry C++ 集成

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

### gRPC 自动追踪

```cpp
// 使用 OpenTelemetry gRPC 拦截器自动追踪
// 无需手动在每个 RPC 中创建 span
auto channel = grpc::CreateChannel(target, creds);
auto tracing_channel = opentelemetry::trace::Propagation::GetPropagation()
    .Inject(channel);
auto stub = UserService::NewStub(tracing_channel);
```

---

## 三板斧整合架构

```
客户端请求
    │
    ▼
┌───────────────────────┐
│  Gateway               │
│  - Create Span（Tracing）│
│  - 记录 Request Count │
│  - 计时 Duration     │
└───────┬───────────────┘
        │
        ▼
┌───────────────────────┐
│  Business Logic              │
│  - INFO Log（关Key事件）│
│  - ERROR Log（Exception）  │
│  - 嵌套 Span（子操作） │
└───────┬───────────────┘
        │
        ▼
┌───────────────────────┐
│  后端服务/DB          │
│  - 慢查询 Warning     │
│  - Connection池 Metrics     │
└───────────────────────┘

输出：
  /metrics  → Prometheus + Grafana（监控面板）
  stdout    → ELK/Loki（日志检索）
  Jaeger    → 链路查询（慢请求定位）
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 日志级别使用 | ERROR=故障、WARN=异常、INFO=事件、DEBUG=调试 |
| 为什么异步日志 | 日志 IO 不阻塞业务线程，spdlog 异步模式下写日志 ≈ 0 开销 |
| Metrics 三大类型 | Counter（累计）、Gauge（瞬时）、Histogram（分布） |
| P99 延迟计算 | Histogram bucket 统计，PromQL: `histogram_quantile(0.99, ...)` |
| Trace 传播方式 | HTTP: `traceparent` header, gRPC: metadata, 跨线程: Context 传递 |
| 三大支柱缺一不可 | 日志查单次异常、Metrics 看趋势告警、Tracing 串起全链路 |

> **工程要点**：C++ 后端服务可观测性三板斧——spdlog（日志）、prometheus-cpp（指标）、opentelemetry-cpp（追踪）。优先级：日志 > 指标 > 追踪。至少先上日志和基础指标（QPS、延迟、错误率），再逐步补充链路追踪。不要试图一开始就全量追踪，先追踪关键接口（用户登录、下单等核心链路）。

---

## 关联笔记

- [Compilation & Linking (编译与链接)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/01-Compilation%20&%20Linking%20(编译与链接)%20⭐.md)
- [Header & Source Organization (头文件与源文件组织)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/02-Header%20&%20Source%20Organization%20(头文件与源文件组织).md)
- [Preprocessor & Macros (预处理与宏)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/03-Preprocessor%20&%20Macros%20(预处理与宏).md)
- [Exception Handling (异常处理)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/04-Exception%20Handling%20(异常处理).md)
- [Debugging gdb & Sanitizers (调试工具)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/05-Debugging%20gdb%20&%20Sanitizers%20(调试工具)%20⭐.md)
