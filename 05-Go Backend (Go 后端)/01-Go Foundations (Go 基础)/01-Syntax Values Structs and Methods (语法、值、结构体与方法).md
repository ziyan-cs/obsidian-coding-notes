---
study_stage: backlog
tags: [language/go, go/foundations]
---

> [!abstract] 学习定位
> Go 默认复制值，指针用于共享可修改对象；零值、显式控制流和小方法集共同形成可预测的程序结构。

# 程序、声明与零值

Go 文件属于一个 package；可执行程序从 `package main` 的 `main()` 开始。包级名称应少而稳定，业务初始化放在显式函数中，避免复杂 `init()` 隐藏顺序和失败。

变量声明后即有零值：数值为 0、bool 为 false、string 为空、pointer/slice/map/channel/function/interface 为 nil。设计类型时尽量让零值可用，例如 `sync.Mutex` 无需构造；但 nil map 不能写入，nil channel 会永久阻塞。

把下面的程序保存为 `main.go`，执行 `go run main.go`，先观察零值和短声明的实际输出：

~~~go
package main

import "fmt"

func main() {
    var retries int
    var enabled bool
    name := "service" // 只能用于函数内部，左侧至少有一个新变量
    fmt.Printf("name=%s retries=%d enabled=%t\n", name, retries, enabled)
}
~~~

短声明会在当前作用域重用已有变量并创建新变量，容易发生 shadowing。下面是函数内部片段，`load`、`transform` 为示意函数，并非可独立运行的程序：

~~~go
result, err := load()
if err != nil {
    return err
}
if enabled {
    result, err := transform(result) // 新作用域中的 result/err
    _ = result
    _ = err
}
~~~

缩短作用域通常有益，但要确认后续读取的是哪个绑定。错误检查保持靠近调用，不用空白标识符 `_` 丢弃重要 error。

# 基本值与控制流

整数类型不自动互转，网络、文件和数据库边界要检查范围后显式转换。金额不用 float 表达；使用最小货币单位或经过验证的 decimal 方案。string 是只读字节序列，`len` 返回字节数；遍历 string 时 `range` 解码 UTF-8 rune，并给出字节偏移。

下面的代码也是函数内部片段，需导入 `fmt` 与 `unicode/utf8`：

~~~go
text := "中A"
fmt.Println(len(text))         // 4 bytes
fmt.Println(utf8.RuneCountInString(text)) // 2 runes
~~~

Go 只有 `for` 循环；`range` 遍历数组、slice、string、map 和 channel。map 迭代顺序不属于契约，输出需要稳定时先收集并排序 key。`switch` 默认不贯穿 case，type switch 用于有限的动态类型分支，但大量分支往往意味着接口边界不清。

# Struct、指针与方法

struct 是字段组成的值。赋值、传参和返回默认复制整个 struct；若字段包含 slice、map、pointer，则复制的是这些描述符或指针，底层状态仍可能共享。

以下类型与方法是片段，放入 package 文件时需导入 `errors` 与 `strings`：

~~~go
type User struct {
    ID   int64
    Name string
}

func (u User) DisplayName() string { return u.Name }

func (u *User) Rename(name string) error {
    if strings.TrimSpace(name) == "" {
        return errors.New("name is empty")
    }
    u.Name = name
    return nil
}
~~~

值 receiver 获得接收者副本，适合小型不可变语义；指针 receiver 可修改原对象并避免复制大 struct。一个类型的方法通常统一 receiver 风格，特别是只要某方法需要指针，就优先保持一致。

返回局部变量地址是安全的，编译器会进行逃逸分析；但是否分配到堆是实现决策，不根据“看见指针”直接下结论。用 profile 和 `-gcflags=-m` 验证热点。

# 构造、不变量与组合

Go 没有构造函数关键字。需要校验或默认值时提供 `NewType`；零值已安全可用时无需机械创建 constructor。

以下是另一个独立类型片段，需导入 `fmt`：

~~~go
type Limiter struct {
    capacity int
}

func NewLimiter(capacity int) (*Limiter, error) {
    if capacity <= 0 {
        return nil, fmt.Errorf("capacity must be positive: %d", capacity)
    }
    return &Limiter{capacity: capacity}, nil
}
~~~

嵌入（embedding）可提升字段或方法，但不是继承，也没有虚函数覆盖关系。优先显式组合依赖；只有提升的方法确实属于外层类型契约时才嵌入。

# 工程边界与练习

- 不复制含 `sync.Mutex`、`sync.Once` 等不可复制状态的 struct；相关检查可由 `go vet` 发现。
- 方法是否并发安全必须写入契约；pointer receiver 本身不提供同步。
- 导出名称写 doc comment，说明语义、单位、错误和并发规则。
- 配置 struct 与运行状态分开，避免调用者在服务启动后继续修改共享配置。

练习：实现带容量与当前使用量的 `Quota`。构造时校验容量，方法拒绝负数与超限，表驱动测试零值、边界和复制行为；再说明它是否允许多个 goroutine 同时使用。

> [!question]- 理解检查
> 1. struct 被复制后，哪些字段仍可能共享底层状态？
> 2. string 的字节长度为什么不等于字符数量？
> 3. pointer receiver 与并发安全有什么关系？

> [!info]- 官方参考
> - [A Tour of Go](https://go.dev/tour/)
> - [The Go Programming Language Specification](https://go.dev/ref/spec)
> - [Effective Go](https://go.dev/doc/effective_go)
