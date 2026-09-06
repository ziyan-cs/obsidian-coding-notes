---
tags: [language/go, go/tooling]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 02-Modules Packages and Tooling (模块包与工具链)

> [!abstract] 一句话结论：module 是依赖版本边界，package 是代码命名空间；目录结构应服务于业务边界，而不是照搬 C++ 的头文件层级。

> [!tip] 先按业务组织，再抽公共层
> 对第一个服务，`internal/todo` 这样的业务包比 `utils`、`common` 更容易维护。只有出现稳定、重复且语义明确的能力，再抽取共享 package。

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

## 30 秒回答

`go.mod` 定义 module，也就是依赖和版本的边界；package 是一个目录内共同编译的代码单元。`internal/` 是编译器帮助执行的封装规则：同 module 外的代码不能导入它。项目先维持一个 module、清晰的业务 package，再用 `go mod tidy` 与测试命令保持依赖真实可用。

## 自测

1. `go mod tidy` 为什么不是“万能修复命令”？它会做什么、不做什么？
2. `cmd/api/main.go` 与 `internal/todo/service.go` 分别应承担什么职责？
3. 当一个 package 出现循环 import，优先检查哪种边界设计问题？

## 练习

建立 `todo-api` 骨架，创建一个 `internal/todo` package，并让 `cmd/api` 调用它。解释为什么 handler 不应直接操作数据库细节。
