> **核心考点**：POSIX 线程 pthread_create/join/detach API、线程属性设置、线程生命周期管理

## 线程的本质

线程是进程内的**执行流**，共享进程的地址空间、文件描述符、信号处理，但拥有独立的：

- 栈（Stack）
- 寄存器（包括 PC、SP）
- 线程本地存储（TLS）
- errno

---

## 创建与等待

```c
#include <pthread.h>
// 编译：gcc -o prog prog.c -lpthread

void *worker(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d running\n", id);
    return (void *)(long)id;    // 返回值
}

int main() {
    pthread_t tid;
    int arg = 42;

    // 创建线程
    pthread_create(&tid, NULL, worker, &arg);

    // 等待线程结束，获取返回值
    void *retval;
    pthread_join(tid, &retval);
    printf("Thread returned: %ld\n", (long)retval);
    return 0;
}
```

---

## 线程属性

```c
pthread_attr_t attr;
pthread_attr_init(&attr);

// 设置栈大小（默认通常 8MB）
pthread_attr_setstacksize(&attr, 2 * 1024 * 1024);   // 2MB

// 分离状态（detached：线程结束自动回收，不需要 join）
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

pthread_create(&tid, &attr, worker, NULL);
pthread_attr_destroy(&attr);
```

---

## 分离（detach）与合并（join）

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

## 线程本地存储（TLS）

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