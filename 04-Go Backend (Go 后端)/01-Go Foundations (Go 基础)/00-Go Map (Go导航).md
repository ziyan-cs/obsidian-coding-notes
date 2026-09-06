---
tags: [language/go, career/backend]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Go Map & Setup — 学什么、如何开始

> [!abstract] 一句话结论：先用标准库做出可测试的 HTTP 服务，再学习框架；Go 的价值不在“更像 C++”，而在简单的并发模型和一致的工程工具链。

## 第一阶段路线

```text
安装与 go 命令 → 类型/函数/interface → module/package
→ error/defer/context → goroutine/channel/sync → testing
→ net/http + JSON + middleware → 一个小型 API
```

当前基线为 Go 1.27（验证于 2026-09-05）；Go 只维护最新两个大版本，升级时统一查看官方 release notes。不要将每个新特性当成必学内容。

## 最小环境检查（Windows PowerShell）

```powershell
go version
go env GOROOT GOPATH
mkdir hello-go; cd hello-go
go mod init example/hello-go
```

创建 `main.go`：

```go
package main

import "fmt"

func main() { fmt.Println("hello, Go") }
```

运行：`go run .`。提交代码前至少执行：`gofmt -w .`、`go test ./...`、`go vet ./...`。

## C++ 对照

| C++ | Go |
| --- | --- |
| CMake 管理构建 | `go` 命令和 `go.mod` 管理构建/依赖 |
| header + source | package 内 `.go` 文件直接编译 |
| RAII | `defer` 管理局部清理；不等同于析构 |
| `std::thread` | goroutine，由 runtime 调度 |

## 练习与自测

- [ ] 建立 `example/hello-go`，能解释 `package main` 和 `func main`。
- [ ] 故意写错格式后运行 `gofmt`，观察变化。
- 自测：`go run .` 与 `go build` 的区别？为什么项目需要 `go.mod`？

## Sources

- [官方入门教程](https://go.dev/doc/tutorial/getting-started)
- [Go release history](https://go.dev/doc/devel/release)
- 验证日期：2026-09-05
