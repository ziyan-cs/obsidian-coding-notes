#linux #thread #pthread #concurrency #system-programming

## ⚡ TL;DR（快速决策）

- 线程编程的核心是：**在同一进程内组织多个执行流共享资源并并发执行**
- Linux/C++ 系统编程中常见线程模型包括：
    - `pthread`
    - C++ `std::thread`
- 若只抓重点，应优先记住：
    - 线程共享进程资源
    - 并发带来同步问题
    - 创建线程容易，写对线程程序更难

## 🧩 Core Idea（核心本质）

- 多线程的价值在于提升并发处理能力和资源共享效率
- 但共享越多，同步与可见性问题越突出
- 一句话理解：
    - **线程编程是“共享资源上的并发执行”。**

## 🏗️ 关键内容

1. 线程创建

- 创建新的执行流

1. 线程同步

- 互斥锁、条件变量、信号量等

1. 线程结束与回收

- `join` / `detach`

1. 共享数据安全

- 保证并发访问正确性

## 🔧 Usage Patterns（可复用代码模板）

```cpp
#include <thread>
using namespace std;

void work() {}

int main() {
    thread t(work);
    t.join();
}
```

## ⚠️ Pitfalls（高频错误）

- 误以为线程天然安全
- 忘记 `join` 或 `detach`
- 共享变量无同步保护
- 锁范围设计不合理导致死锁或性能下降

## 🚀 Performance / Tips（理解深化）

- 多线程优化不是“线程越多越快”
- 线程数、上下文切换、缓存一致性、锁竞争都会影响性能
- 正确性、可维护性和性能需要同时权衡

## 🧪 Common Scenarios（常见使用场景）

- 线程池
- 并发任务处理
- 后台工作线程
- 高并发服务设计基础

## 🧾 Minimal Template（最小理解模板）

```
创建线程 -> 并发执行 -> 同步共享资源 -> join/detach 收尾
```

## 📌 One-liner Summary（一句话总结）

- **线程编程的核心，是在共享进程资源的前提下安全组织多个执行流并发执行。**