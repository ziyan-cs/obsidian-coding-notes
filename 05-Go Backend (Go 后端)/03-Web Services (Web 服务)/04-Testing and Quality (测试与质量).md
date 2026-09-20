---
study_stage: backlog
tags: [language/go, go/testing]
---

> [!note] 方法论坐标
> 测试策略与证据标准见 [Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)；本篇聚焦 table-driven test、httptest、race、fuzz 与 Go benchmark。

> [!abstract] 学习定位
> 先把业务规则写成纯函数和 table-driven test，再测试 HTTP；测试应验证可观察行为，而不是内部实现细节。

> [!summary] 核心摘要
>
> 用 table-driven test 覆盖输入、输出与边界；把纯业务规则从 HTTP 细节中抽出；HTTP 测试只验证路由、序列化和状态码等协议行为。

# 子测试、并行与清理

每个 case 使用 `t.Run`，失败信息包含输入、实际和预期。只有测试之间完全隔离时才调用 `t.Parallel()`；共享环境变量、固定端口、全局变量或数据库记录会导致随机污染。循环变量捕获语义随 Go 版本演进，仍应让每个 case 的数据边界清晰并以项目版本验证。

临时文件使用 `t.TempDir()`，清理使用 `t.Cleanup()`；测试 helper 调用 `t.Helper()`，使错误定位到调用处。不要用 `time.Sleep` 猜并发完成，使用 channel、WaitGroup、context 或可控 fake clock。

# HTTP 与依赖测试

`httptest.NewRecorder` 适合直接测试 handler；需要真实连接、redirect、streaming 或 client 行为时使用 `httptest.NewServer`。响应断言包括 status、header、body schema 和错误 code，不依赖 map 序列化字段顺序。

在消费者 package 定义小接口并传入 fake repository/clock。不要 mock 所有内部方法；业务状态和输出才是主要契约。数据库隔离、事务与 SQL 方言必须用对应数据库做集成测试，内存 fake 不能证明真实语义。

# Race、Fuzz 与 Benchmark

race detector 发现实际运行到的 data race，配合高并发测试和重复运行；它不能证明没有死锁、泄漏或逻辑竞态。fuzz target 要快速、确定，并断言不变量，例如“解析后再编码可读”“非法帧不 panic”。失败 corpus 保存到测试数据形成回归。

```go
package todo

import (
    "errors"
    "strconv"
    "testing"
)

func ParseID(text string) (int64, error) {
    id, err := strconv.ParseInt(text, 10, 64)
    if err != nil || id < 1 {
        return 0, errors.New("invalid id")
    }
    return id, nil
}

func FuzzParseID(f *testing.F) {
    f.Add("42")
    f.Add("-1")
    f.Fuzz(func(t *testing.T, text string) {
        id, err := ParseID(text)
        if err == nil && id < 1 {
            t.Fatalf("successful parse returned %d", id)
        }
    })
}
```

benchmark 在循环外准备数据，使用 `b.ResetTimer` 或新版计时 API 排除 setup；报告 allocs，并用 `-count` 与统计工具比较多次结果。防止编译器消除无用工作，也不要用微基准替代端到端负载测试。

# CI 质量门禁

固定 Go 版本，依次运行格式化检查、vet、测试、race（可按成本拆分）、构建与漏洞检查。失败保存测试日志和最小复现，不让 flaky test 通过自动重跑被掩盖。覆盖率用于发现未触达代码，不作为单一目标。

练习：为 todo service 建立表驱动规则测试、fake repository 测试、httptest 协议测试、真实数据库事务测试、race worker 测试和一个 fuzz parser；说明每层能发现什么、发现不了什么。
# 一份完整的表驱动测试

以下可独立保存为 `todo_test.go`；真实项目将 `CreateTodo` 放入业务文件，测试文件只保留测试代码。

```go
package todo

import (
    "errors"
    "strings"
    "testing"
)

func CreateTodo(title string) (string, error) {
    title = strings.TrimSpace(title)
    if title == "" || len([]rune(title)) > 80 {
        return "", errors.New("invalid title")
    }
    return title, nil
}

func TestCreateTodo(t *testing.T) {
    cases := []struct {
        name, input, want string
        wantErr bool
    }{
        {"valid", " learn Go ", "learn Go", false},
        {"empty", " ", "", true},
        {"too long", strings.Repeat("x", 81), "", true},
    }
    for _, tc := range cases {
        t.Run(tc.name, func(t *testing.T) {
            got, err := CreateTodo(tc.input)
            if (err != nil) != tc.wantErr || got != tc.want {
                t.Fatalf("CreateTodo(%q) = %q, %v; want %q, error=%v",
                    tc.input, got, err, tc.want, tc.wantErr)
            }
        })
    }
}
```

从 `go test ./...` 开始；并发代码额外运行 `go test -race ./...`，静态检查运行 `go vet ./...`。fuzz 需单独按目标执行，例如 `go test -fuzz=FuzzParseID -fuzztime=30s`；生成的反例要进入回归语料。测试表格的目标是并排呈现输入、预期和边界，不是追求行数最少。

> [!info]- 延伸阅读
> - 下一步：[05-Observability and Operations (可观测性与运行)](/05-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/05-Observability%20and%20Operations%20(可观测性与运行).md)

