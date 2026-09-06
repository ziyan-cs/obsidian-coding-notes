---
tags: [language/go, go/testing]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 04-Testing and Quality (测试与质量)

> [!abstract] 一句话结论：先把业务规则写成纯函数和 table-driven test，再测试 HTTP；测试应验证可观察行为，而不是内部实现细节。

```go
func Add(a, b int) int { return a + b }

func TestAdd(t *testing.T) {
    cases := []struct{ a, b, want int }{
        {1, 2, 3}, {-1, 1, 0},
    }
    for _, tc := range cases {
        if got := Add(tc.a, tc.b); got != tc.want {
            t.Fatalf("Add(%d,%d)=%d, want %d", tc.a, tc.b, got, tc.want)
        }
    }
}
```

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

## 自测

为“创建 todo，标题不能为空”写至少三个测试：成功、空标题、超长标题。
