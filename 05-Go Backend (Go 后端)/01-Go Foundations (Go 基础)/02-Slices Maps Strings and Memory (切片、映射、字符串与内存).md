---
status: learning
confidence: high
content_verified: 2026-09-18
tags: [language/go, go/memory]
---

> [!abstract] 学习定位
> slice、map 和 string 都是小型描述值而非“完整数据本体”；复制描述值可能继续共享底层存储，容量、别名与生命周期必须显式推理。

# Array 与 Slice

array 的长度属于类型，赋值会复制全部元素；slice 描述一段底层 array，包含指针、长度和容量。复制 slice 只复制描述符，元素仍共享。

~~~go
base := []int{1, 2, 3, 4}
view := base[1:3]
view[0] = 20
fmt.Println(base) // [1 20 3 4]
~~~

`append` 在容量足够时复用底层 array，容量不足时分配新 array；所以 append 后两个 slice 是否共享不能凭直觉判断。始终接收 append 返回值。

~~~go
items = append(items, value)
~~~

API 若要保留调用方数据，应明确是否复制：

~~~go
owned := append([]byte(nil), input...)
// 或 slices.Clone(input)
~~~

子 slice 会让整个大 array 保持可达。读取大文件后仅保留很小切片，可能意外保留全部内存；复制需要长期保存的小片段即可解除引用。

# 长度、容量与分配

`make([]T, length, capacity)` 创建指定长度的可写元素；若只想预留容量，应使用 `make([]T, 0, capacity)`。把预分配长度和 append 混用，会在前面留下零值元素。

预估容量可减少扩容和复制，但错误的大容量也会浪费内存。优化前用 benchmark 的 `-benchmem` 和 profile 查看真实分配，不背运行时增长倍数。

# Map 的语义

nil map 可以读取和删除，但写入会 panic；`make(map[K]V)` 后才能写。读取缺失键返回值类型零值，使用 comma-ok 区分“不存在”和“存在但值为零”。

~~~go
count, ok := counts[key]
if !ok {
    // key does not exist
}
~~~

map 不是并发安全容器。一个 goroutine 写、另一个读也可能 race 或触发运行时错误。简单共享状态使用 mutex；`sync.Map` 只适用于文档描述的特定访问模式，不是通用更快 map。

map 迭代顺序未定义。序列化、测试快照和用户输出需要确定性时排序 key。作为 key 的值必须可比较；slice、map 和 function 不能直接作为 key。

# String、Byte 与 Rune

string 是不可变字节序列，不保证内容一定是合法 UTF-8。`[]byte` 适合原始数据和可变缓冲，`[]rune` 适合按 Unicode code point 处理，但“用户看到的字符”还可能由多个 rune 组成。

~~~go
for offset, r := range text {
    fmt.Printf("byte=%d rune=%U\n", offset, r)
}
~~~

string 与 `[]byte` 转换通常复制数据。高频协议解析避免无意义来回转换，但不要为了减少一次分配使用 unsafe 破坏不可变和生命周期保证。

# 所有权与 API 约定

Go 类型系统不直接表达唯一所有权，因此文档必须说明：函数是否保留参数、返回 slice 能否修改、并发时谁负责同步、buffer 何时可复用。`bytes.Buffer` 或对象池中的 byte slice 交给异步任务前通常需要复制，否则下一次复用会改写尚未消费的数据。

函数返回内部 map/slice 会让调用方绕过不变量。可以返回副本、只提供查询方法，或明确把所有权转移给调用方。

# 实验与排查

1. 创建容量不同的 slice，记录 append 前后首元素地址，观察何时分离。
2. 从 100 MB byte slice 截取 10 字节并长期保存，用 heap profile 观察底层 array 是否仍存活；复制小片段后比较。
3. 对共享 map 写 race 测试，运行 `go test -race`，再用 mutex 修复。
4. 处理含中文、emoji 和非法 UTF-8 的 string，比较字节、rune 与显示字符。

> [!question]- 理解检查
> 1. 复制 slice 后，修改元素和 append 分别可能影响谁？
> 2. 为什么从 map 直接读取零值不能证明 key 存在？
> 3. 小子切片为什么可能保留巨大内存？

> [!info]- 官方参考
> - [Go Slices: usage and internals](https://go.dev/blog/slices-intro)
> - [Strings, bytes, runes and characters in Go](https://go.dev/blog/strings)
> - [Go Memory Model](https://go.dev/ref/mem)
