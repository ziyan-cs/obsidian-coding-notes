---
status: learning
confidence: low
verified: 2026-09-17
tags: [language/go, go/testing]
---

> [!abstract] 一句话结论：先把业务规则写成纯函数和 table-driven test，再测试 HTTP；测试应验证可观察行为，而不是内部实现细节。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：用 table-driven test 覆盖输入、输出与边界；把纯业务规则从 HTTP 细节中抽出；HTTP 测试只验证路由、序列化和状态码等协议行为。

# 基线命令

```text
go test ./...
go test -race ./...
go test -cover ./...
go vet ./...
```

# 规则

- 测试文件为 `*_test.go`，函数名以 `TestXxx` 开头。
- 先覆盖正常、空输入、边界、错误和取消；覆盖率不是质量本身。
- 用 `httptest` 测 handler，避免测试中真的监听固定端口。
- 遇到不可控时间、随机数、网络，抽象成依赖并注入可控替身。

# Table-driven test 的目标

```go
cases := []struct { name, title string; wantErr bool }{
    {"valid", "learn Go", false},
    {"empty", "", true},
}
for _, tc := range cases {
    t.Run(tc.name, func(t *testing.T) {
        _, err := CreateTodo(tc.title)
        if (err != nil) != tc.wantErr { t.Fatalf("err = %v", err) }
    })
}
```

测试表格不是为了少写代码，而是让输入域、预期行为和失败案例并排可读。对并发代码再运行 `go test -race`；race detector 发现的是运行到的 data race，不是并发正确性的数学证明。

# 测试层次

| 层次 | 验证对象 | 速度 |
| --- | --- | --- |
| unit | 业务规则、纯函数 | 快，数量最多 |
| integration | DB、缓存、真实序列化 | 较慢，验证边界 |
| HTTP | 路由、状态码、响应体 | 覆盖协议契约 |

> [!question]- 自测：先回答再展开
> 为“创建 todo，标题不能为空”写至少三个测试：成功、空标题、超长标题。

> [!info]- 延伸阅读
> - 下一步：[05-Observability and Operations (可观测性与运行)](/04-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/05-Observability%20and%20Operations%20(可观测性与运行).md)
