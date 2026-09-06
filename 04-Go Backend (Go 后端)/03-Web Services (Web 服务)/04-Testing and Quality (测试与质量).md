---
tags: [language/go, go/testing]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 04-Testing and Quality (测试与质量)

> [!abstract] 一句话结论：先把业务规则写成纯函数和 table-driven test，再测试 HTTP；测试应验证可观察行为，而不是内部实现细节。

## 30 秒回答

**核心结论**：一句话结论：先把业务规则写成纯函数和 table-driven test，再测试 HTTP；测试应验证可观察行为，而不是内部实现细节。


## 基线命令

```text
go test ./...
go test -race ./...
go test -cover ./...
go vet ./...
```

## 规则

- 测试文件为 `*_test.go`，函数名以 `TestXxx` 开头。
- 先覆盖正常、空输入、边界、错误和取消；覆盖率不是质量本身。
- 用 `httptest` 测 handler，避免测试中真的监听固定端口。
- 遇到不可控时间、随机数、网络，抽象成依赖并注入可控替身。

## Table-driven test 的目标

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

## 测试层次

| 层次 | 验证对象 | 速度 |
| --- | --- | --- |
| unit | 业务规则、纯函数 | 快，数量最多 |
| integration | DB、缓存、真实序列化 | 较慢，验证边界 |
| HTTP | 路由、状态码、响应体 | 覆盖协议契约 |

## 自测

为“创建 todo，标题不能为空”写至少三个测试：成功、空标题、超长标题。

## 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
04-Testing and Quality (测试与质量)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
