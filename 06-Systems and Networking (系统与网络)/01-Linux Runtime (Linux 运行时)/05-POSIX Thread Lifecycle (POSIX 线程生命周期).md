---
study_stage: backlog
---

> [!abstract] 学习目标：掌握 pthread 创建、等待、分离、取消与线程资源回收边界。

> [!note] 本节重点：POSIX 线程 pthread_create/join/detach API、线程属性设置、线程生命周期管理

# 线程的本质

线程是进程内的**执行流**，共享进程的地址空间、文件描述符、信号处理，但拥有独立的：

- 栈（Stack）
- 寄存器（包括 PC、SP）
- 线程本地存储（TLS）
- errno

---

# 创建与等待

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 编译：cc -std=c11 -Wall -Wextra -pthread prog.c -o prog

static void *worker(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d running\n", id);
    return NULL;
}

int main(void) {
    pthread_t tid;
    int arg = 42;
    int rc = pthread_create(&tid, NULL, worker, &arg);
    if (rc != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }
    rc = pthread_join(tid, NULL);
    if (rc != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

---

# 线程属性

```c
pthread_attr_t attr;
pthread_attr_init(&attr);

// 设置栈大小（默认值依平台与资源限制而异）
pthread_attr_setstacksize(&attr, 2 * 1024 * 1024);   // 2MB

// 分离状态（detached：线程结束自动回收，不需要 join）
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

pthread_create(&tid, &attr, worker, NULL);
pthread_attr_destroy(&attr);
```

---

# 分离（detach）与合并（join）

```c
// join：主线程等待子线程，获取返回值
pthread_join(tid, &retval);

// detach：线程结束后自动释放资源，无法 join
pthread_detach(tid);
// 或在线程内部自我分离：
pthread_detach(pthread_self());
```

> 若线程既没有 join 也没有 detach，线程结束后资源泄漏（类似僵尸进程）。

---

# 线程本地存储（TLS）

每个线程有独立的副本，互不干扰：

```c
// C++11
thread_local int counter = 0;    // 每个线程有自己的 counter

// POSIX
pthread_key_t key;
pthread_key_create(&key, free);         // 创建 key，free 是析构函数
pthread_setspecific(key, malloc(100));  // 设置当前线程的值
void *val = pthread_getspecific(key);   // 获取当前线程的值
```


---

# 生命周期状态与错误处理

`pthread_create` 成功后，joinable 线程最终必须恰好被 `pthread_join` 或 `pthread_detach` 一次。线程函数返回只结束该线程，不释放 joinable 线程的全部实现资源；detached 线程自动回收，但调用方无法再等待结果。

所有 pthread API 返回错误码而非依赖 `errno`，必须检查：

```c
int rc = pthread_create(&tid, NULL, worker, arg);
if (rc != 0) {
    fprintf(stderr, "pthread_create: %s\n", strerror(rc));
    return EXIT_FAILURE;
}
```

传入参数的生命周期必须覆盖线程读取时间。循环中把同一个栈变量地址传给多个线程会产生竞态；为每个任务分配独立参数，或由受同步保护的队列传递。线程返回值也必须指向仍有效的对象，不能返回局部变量地址。

# 取消、清理和进程退出

`pthread_cancel` 是协作式请求，默认在 cancellation point 执行延迟取消。异步取消可能在持锁或资源半更新时终止线程，通常不应使用。若必须支持取消，使用 cleanup handler/RAII，明确哪些函数是取消点，并保证锁和内存释放。

主线程从 `main` 返回会终止整个进程；需要让其他线程继续可调用 `pthread_exit`，但工程上通常由显式 shutdown 协议通知、join 并收敛。fork、多线程和信号处理有额外限制，应独立学习。

线程栈默认值依平台、资源限制和属性实现而异，不能假定固定 8 MiB。`pthread_attr_getstacksize` 查询，设置时考虑 `PTHREAD_STACK_MIN`、递归深度和 guard page。

实验：创建 N 个线程并 join；故意复用循环变量观察错误；改为独立参数；再实现 stop flag + condition variable 的关闭流程，并用 sanitizer 检查竞态。

参考：[pthreads(7)](https://man7.org/linux/man-pages/man7/pthreads.7.html)、[pthread_create(3)](https://man7.org/linux/man-pages/man3/pthread_create.3.html)。
