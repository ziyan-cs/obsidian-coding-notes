---
tags: [language/go, go/tooling]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Modules, Packages & Tooling — 依赖和代码组织

> **一句话结论**：module 是依赖版本边界，package 是代码命名空间；目录结构应服务于业务边界，而不是照搬 C++ 的头文件层级。

## 三个概念

- **module**：由 `go.mod` 定义的一组 packages 和依赖版本。
- **package**：同一目录内、同一个 `package` 名的 Go 文件组成的编译单元。
- **import path**：引用 package 的路径，例如 `example.com/app/internal/user`。

```text
todo-api/
├── go.mod
├── cmd/api/main.go          # 组装与启动
├── internal/todo/service.go # 仅本 module 可导入
└── internal/httpapi/handler.go
```

## 常用命令

| 命令 | 用途 | 什么时候用 |
| --- | --- | --- |
| `go mod init <path>` | 创建 module | 新项目一次 |
| `go mod tidy` | 同步声明与实际依赖 | 新增/删除 import 后 |
| `go run .` | 编译并运行 main package | 开发时 |
| `go test ./...` | 测试所有 package | 每次改动后 |
| `go fmt ./...` | 统一格式 | 提交前 |
| `go vet ./...` | 静态检查常见问题 | 提交前 |

## 常见坑

- 不要手改 `go.sum` 来“解决”依赖问题；先理解是版本、网络还是 import 错误。
- `internal/` 外的 package 无法导入其下目录，这是刻意的封装边界。
- 不在同一项目中随意创建多个 module；初学项目先保持一个 `go.mod`。

## 练习

建立 `todo-api` 骨架，创建一个 `internal/todo` package，并让 `cmd/api` 调用它。解释为什么 handler 不应直接操作数据库细节。
