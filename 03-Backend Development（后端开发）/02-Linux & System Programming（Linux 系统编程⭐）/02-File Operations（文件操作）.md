#linux #file-operations #system-call #fd #io

## ⚡ TL;DR（快速决策）

- 文件操作的核心是：**通过文件描述符和系统调用完成文件的打开、读写、定位与关闭**
- 在 Unix/Linux 中，很多资源都可抽象为“文件”来处理
- 若只抓重点，应优先记住：
    - `open`
    - `read`
    - `write`
    - `lseek`
    - `close`

## 🧩 Core Idea（核心本质）

- 应用程序并不直接控制磁盘，而是通过内核提供的文件接口进行访问
- 文件描述符（fd）是进程持有打开文件的入口句柄
- 一句话理解：
    - **文件操作就是通过文件描述符使用内核管理的 I/O 资源。**

## 🏗️ 核心系统调用

1. `open`

- 打开文件并返回文件描述符

1. `read`

- 从文件描述符读取数据到用户缓冲区

1. `write`

- 将用户缓冲区数据写入文件描述符

1. `lseek`

- 调整当前文件偏移量

1. `close`

- 关闭文件描述符并释放内核资源

## 🔧 Usage Patterns（可复用代码模板）

```cpp
int fd = open("a.txt", O_RDONLY);
char buf[1024];
ssize_t n = read(fd, buf, sizeof(buf));
close(fd);
```

## ⚠️ Pitfalls（高频错误）

- 打开文件后忘记关闭
- 不检查系统调用返回值
- 混淆文本文件逻辑和底层字节流
- 忽略部分读写、阻塞和错误码处理

## 🚀 Performance / Tips（理解深化）

- 文件操作不仅用于普通文件，也广泛适用于管道、Socket、设备文件等对象
- 理解文件描述符后，Linux I/O 模型会更容易串起来
- 系统编程里，正确处理错误和返回值与功能本身同样重要

## 🧪 Common Scenarios（常见使用场景）

- 读写配置文件
- 日志输出
- 二进制文件处理
- 理解标准输入输出重定向

## 🧾 Minimal Template（最小理解模板）

```
open -> read/write -> lseek(可选) -> close
```

## 📌 One-liner Summary（一句话总结）

- **文件操作的核心，是通过文件描述符和系统调用完成内核管理下的 I/O 访问**