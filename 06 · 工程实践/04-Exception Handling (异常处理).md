> **核心考点**：异常安全保证、栈展开、noexcept 优化、RAII 与异常

## 1. 三种异常安全保证

```cpp
// 1. 基本保证：抛出异常后，对象处于合法状态
// 2. 强保证：抛出异常后，状态回滚（类似事务）
// 3. 不抛出：绝不抛出异常（noexcept）

class Vector {
    int* data_;
    size_t size_;
public:
    // 强保证：使用 copy-and-swap
    void push_back(int val) {
        auto new_data = new int[size_ + 1];
        std::copy(data_, data_ + size_, new_data);
        new_data[size_] = val;
        std::swap(data_, new_data);  // swap 不抛出
        delete[] new_data;
        ++size_;
    }
};
```

## 2. noexcept

```cpp
// noexcept 有两种作用：
// 1. 承诺函数不抛异常
// 2. 告诉编译器进行优化（比如 vector 的 move 操作）

void safe_func() noexcept;  // 抛出异常会调用 std::terminate

// noexcept 是函数类型的一部分（C++17 起）
void (*f1)() noexcept;      // f1 只能指向 noexcept 函数
void (*f2)();               // f2 可指向任何函数（包括 noexcept）

// 条件性 noexcept
void swap(T& a, T& b) noexcept(std::is_nothrow_swappable_v<T>);
```

**Move 构造与 noexcept 的关系**：

```cpp
struct Bad { Bad(Bad&&) { /* 可能抛异常 */ } };
struct Good { Good(Good&&) noexcept { /* ... */ } };

std::vector<Bad> v1;
// push_back 时，vector 扩容会用拷贝而非移动
// （因为移动可能抛异常，无法保证强异常安全）

std::vector<Good> v2;
// push_back 时，vector 扩容会使用移动操作（快得多！）

// 所以：移动构造函数必须标记 noexcept！
```

## 3. 栈展开（Stack Unwinding）

```cpp
struct Cleanup {
    ~Cleanup() { std::cout << "cleanup"; }
};

void func() {
    Cleanup c;  // 在栈上创建
    throw std::runtime_error("error");  // 抛出异常
    // Cleanup::~Cleanup() 在栈展开时被调用
}

int main() {
    try {
        func();
    } catch (const std::exception& e) {
        // c 已经被正确析构了
    }
}
```

**栈展开的过程**：
1. 从 `throw` 处开始，逐层向上查找 `catch` 子句
2. 每退出一层，该层栈上所有对象的析构函数被调用
3. 找到匹配的 `catch` 后，进入异常处理

## 4. 异常安全编程指南

```cpp
// ✅ 使用 RAII 管理资源（异常安全的核心）
std::unique_ptr<Foo> ptr(new Foo());
// 不需要 try-catch，析构函数自动释放

// ✅ 使用智能指针而不是裸 new
std::unique_ptr<int[]> buffer(new int[100]);

// ❌ 危险的裸 new
void bad() {
    Foo* p = new Foo();
    bar();     // 如果 bar() 抛出异常，p 泄漏！
    delete p;
}

// ✅ 异常中立：让异常继续向上传播
void wrapper() {
    // 不需要处理时，不要 catch
}

// ✅ 析构函数/swap/移动构造/移动赋值 应标记 noexcept
~Foo() noexcept;
void swap(Foo&) noexcept;
```

## 5. 异常 vs 错误码

| | 异常 | 错误码 |
|--|------|--------|
| 传播方式 | 自动展开栈 | 手动传递和检查 |
| 性能 | 正常路径无开销，异常路径慢 | 每条路径都要检查 |
| 信息量 | 多（类型 + what()） | 少（一个整数）|
| 被忽略的可能 | ❌ 无法忽略 | ✅ 可能被忘记检查 |
| 适用场景 | **致命/意外错误** | **频繁发生/性能关键的预期错误** |

```cpp
// ✅ 异常适用：意料之外的错误
int divide(int a, int b) {
    if (b == 0) throw std::runtime_error("division by zero");
    return a / b;
}

// ✅ 错误码适用：预期中的失败
std::error_code ec;
auto result = read_file("config.txt", ec);
if (ec) { /* 处理不存在等预期情况 */ }
```

> **工程要点**：编写异常安全代码的核心不是 try-catch，而是 **RAII**。资源在构造时获取，析构时释放——析构函数在栈展开时一定会被调用。现代 C++ 中极少需要写 try-catch，除非要做错误转换或日志记录。
