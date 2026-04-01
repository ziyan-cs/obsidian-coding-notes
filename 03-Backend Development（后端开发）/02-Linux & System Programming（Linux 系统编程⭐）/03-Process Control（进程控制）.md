#linux #process-control #fork #exec #wait #system-programming

## ⚡ TL;DR（快速决策）

- 进程控制的核心是：**创建、替换、等待和终止进程**
- Linux 进程编程中最经典的一组接口是：
    - `fork`
    - `exec`
    - `wait` / `waitpid`
- 若只抓重点，应优先记住：
    - `fork` 复制当前进程
    - `exec` 用新程序替换当前进程映像
    - `wait` 回收子进程资源

## 🧩 Core Idea（核心本质）

- 进程控制本质上是在操作系统提供的执行流与资源容器上进行管理
- Shell 启动外部程序，本质就依赖这套机制
- 一句话理解：
    - **fork 产生新进程，exec 装入新程序，wait 完成父子协同。**

## 🏗️ 核心接口

1. `fork`

- 创建子进程
- 返回后父子进程从同一位置继续执行，但返回值不同

1. `exec`

- 用新程序替换当前进程映像
- 成功后通常不再返回

1. `wait` / `waitpid`

- 父进程等待子进程结束并回收资源

1. `exit`

- 正常结束进程

## 🔧 Usage Patterns（可复用代码模板）

```cpp
pid_t pid = fork();
if (pid == 0) {
    execlp("ls", "ls", "-l", NULL);
    exit(1);
} else {
    wait(NULL);
}
```

## ⚠️ Pitfalls（高频错误）

- 不理解 `fork` 后父子进程都会继续执行
- `exec` 失败时没处理错误
- 父进程不 `wait`，导致僵尸进程
- 混淆进程创建和程序替换两个动作

## 🚀 Performance / Tips（理解深化）

- 理解进程控制后，更容易理解 Shell、守护进程、子进程管理和服务编排
- 实际工程里，正确处理父子协同和资源回收非常关键

## 🧪 Common Scenarios（常见使用场景）

- Shell 执行命令
- 子进程任务派发
- 守护进程和后台进程
- 进程生命周期管理

## 🧾 Minimal Template（最小理解模板）

```
fork 创建子进程 -> 子进程 exec 新程序 -> 父进程 wait 回收
```

## 📌 One-liner Summary（一句话总结）

- **进程控制的核心，是通过 fork、exec 和 wait 等机制管理进程的创建、执行与回收。**