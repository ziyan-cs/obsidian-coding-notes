> **核心考点**：线程的创建/汇合/分离、std::thread 与 POSIX pthread 的关系、线程生命周期管理

## std::thread 基础

```cpp
#include <thread>

// 创建线程——任何可调用对象
void worker(int id) { /* ... */ }
std::thread t1(worker, 42);                 // 函数 + 参数
std::thread t2([](int id) { /* ... */ }, 1); // Lambda

// RAII 包装：确保线程被 join 或 detach
class ThreadGuard {
    std::thread& t;
public:
    explicit ThreadGuard(std::thread& t_) : t(t_) {}
    ~ThreadGuard() {
        if (t.joinable()) t.join();
    }
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
};
```

## 线程生命周期管理

```cpp
std::thread t(worker, 42);

t.join();   // 阻塞等待线程结束，之后 t 不再 joinable
// 或
t.detach(); // 分离，线程在后台运行，t 不再关联线程

// 重要：析构前必须 join 或 detach
// 否则 std::thread::~thread() 会调用 std::terminate()！
```

**关键规则**：
- 每个 `std::thread` 对象在析构前必须调用 `join()` 或 `detach()`
- `joinable()` 检查线程是否可被 join
- `detach` 后的线程无法再获取其状态

## 参数传递陷阱

```cpp
// ❌ 危险：传递引用时忘记用 std::ref
void modify(int& x) { x = 42; }
int val = 0;
std::thread t(modify, val);    // 编译错误或拷贝！thread 会拷贝参数
std::thread t(modify, std::ref(val));  // ✅ 正确传递引用

// ❌ 危险：传入临时对象的指针
void process(const Data& d);
Data d;
std::thread t(process, std::cref(d));  // ✅ 确保 d 在线程执行期间存活
```

## 线程与 POSIX pthread 的关系

```cpp
// std::thread 底层封装了 pthread（Linux/macOS）或 Windows Threads

// 获取原生句柄
std::thread t(worker, 1);
pthread_t handle = t.native_handle();  // Linux 返回 pthread_t
pthread_setname_np(handle, "worker-1"); // 设置线程名称（调试用）
t.detach();

// 硬件并发
unsigned int n = std::thread::hardware_concurrency();  // 逻辑 CPU 核心数
```

## std::jthread (C++20)

```cpp
// C++20 引入：自动 join + 可取消
std::jthread jt([](std::stop_token st) {
    while (!st.stop_requested()) {
        // 工作循环
    }
});
// jt 析构时自动 join()

// 请求停止
jt.request_stop();
```

## 线程 ID 与异常安全

```cpp
// 获取当前线程 ID
std::cout << std::this_thread::get_id();

// 异常安全：在线程中捕获所有异常
try {
    std::thread t([&] {
        try {
            throw std::runtime_error("error");
        } catch (...) {
            // 处理异常
        }
    });
    t.join();
} catch (...) {
    // 不能在此捕获线程内的异常！
}
```

> **工程要点**：线程是稀缺资源。创建线程的开销大约为几微秒（栈分配 + 系统调用）。**不要为短任务创建线程**——用线程池。一个进程的线程数通常不超过 `hardware_concurrency`。
