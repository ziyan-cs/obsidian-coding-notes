#linux #poll #io-multiplexing #system-programming

## ⚡ TL;DR（快速决策）

- `poll` 的核心是：**用更通用的描述结构监听多个文件描述符的 I/O 就绪事件**
- 它和 `select` 思想相同，但接口设计更灵活
- 若只抓重点，应优先记住：
    - 使用 `pollfd` 数组
    - 不受 `select` 那类固定 fd_set 位图接口限制
    - 仍然需要线性扫描所有监听项

## 🧩 Core Idea（核心本质）

- `poll` 仍然属于 I/O 多路复用中的“就绪通知模型”
- 一句话理解：
    - **poll 是对 select 接口形式的改进，而不是事件模型的根本变革。**

## 🏗️ 关键特点

1. 监听集合用数组表达
2. 可监听多类事件
3. 返回后仍需遍历全部项判断谁就绪
4. 在大量连接下仍会产生线性扫描成本

## 🔧 Usage Patterns（可复用理解框架）

1. 用“接口更灵活，但复杂度仍类似”理解 `poll`
2. 用“数组遍历”理解其性能边界

## ⚠️ Pitfalls（高频错误）

- 以为 `poll` 一定能解决高并发扩展性问题
- 不理解其仍有 O(n) 遍历成本
- 只比较 API 形式，不比较事件分发模型

## 🚀 Performance / Tips（理解深化）

- `poll` 比 `select` 更现代、更灵活，但大规模高并发场景仍常进一步走向 `epoll`
- 学 `poll` 时，重点是理解它对 `select` 的改进点和未解决的问题

## 🧪 Common Scenarios（常见使用场景）

- 中等规模多连接监听
- 理解 I/O 多路复用接口演进
- 对比 `select` 与 `epoll`

## 🧾 Minimal Template（最小理解模板）

```
准备 pollfd 数组 -> 调用 poll 等待 -> 扫描返回结果 -> 处理就绪 fd
```

## 📌 One-liner Summary（一句话总结）

- **poll 的核心，是以更灵活的描述结构实现多个文件描述符的 I/O 就绪等待。**