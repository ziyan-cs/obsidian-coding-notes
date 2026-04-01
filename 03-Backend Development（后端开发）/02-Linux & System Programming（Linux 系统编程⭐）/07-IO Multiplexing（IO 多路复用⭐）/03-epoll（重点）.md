#linux #epoll #io-multiplexing #high-concurrency #event-driven

## ⚡ TL;DR（快速决策）

- `epoll` 的核心是：**以事件驱动方式高效管理大量文件描述符的 I/O 就绪通知**
- 它是 Linux 高并发网络编程中的关键机制
- 若只抓重点，应优先记住：
    - 红黑树管理关注对象
    - 就绪链表返回活跃事件
    - 支持 LT / ET 模式
    - 更适合大规模并发连接

## 🧩 Core Idea（核心本质）

- 与 `select` / `poll` 每轮扫描全部 fd 不同，`epoll` 更强调“只返回就绪事件”
- 一句话理解：
    - **epoll 是 Linux 面向高并发事件驱动 I/O 的核心接口。**

## 🏗️ 关键概念

1. `epoll_create`

- 创建 epoll 实例

1. `epoll_ctl`

- 注册、修改、删除关注的 fd

1. `epoll_wait`

- 等待并获取就绪事件

1. LT / ET

- LT（水平触发）：只要还有数据未处理，就持续通知
- ET（边缘触发）：状态变化时通知一次，对程序要求更高

## 🔧 Usage Patterns（可复用理解框架）

1. 用“关注集合管理 + 就绪事件返回”理解 `epoll`
2. 用“只处理活跃 fd”理解高并发优势
3. 用 LT / ET 理解不同编程风格

## ⚠️ Pitfalls（高频错误）

- 把 `epoll` 神化成“绝对更快”，忽略业务和实现细节
- 使用 ET 模式却没配合非阻塞和循环读写
- 不理解它优化的是事件分发模型，而不只是 API 写法

## 🚀 Performance / Tips（理解深化）

- `epoll` 是 Linux 高并发服务的核心基础设施之一
- 它常与非阻塞 Socket、线程池、Reactor 模型共同使用
- 学 `epoll` 时，重点是理解事件驱动思维，而不仅是记接口名

## 🧪 Common Scenarios（常见使用场景）

- 高并发服务器
- Reactor 事件循环
- 长连接管理
- 网络中间件和代理服务

## 🧾 Minimal Template（最小理解模板）

```
创建 epoll 实例 -> 注册 fd -> 等待就绪事件 -> 仅处理活跃 fd
```

## 📌 One-liner Summary（一句话总结）

- **epoll 的核心，是以事件驱动方式高效管理和分发大量文件描述符的 I/O 就绪事件。**