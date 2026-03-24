#cpp #memory #lifetime

## ⚡ TL;DR（快速决策）

- 不确定对象该活多久 → 先想生命周期，不要先想 `new`
- 局部临时数据 → 栈上对象
- 需要跨作用域拥有资源 → 优先 RAII / 智能指针，不要裸 `new/delete`
- 只借用对象，不拥有资源 → 引用 / 裸指针 / `const T&`
- 一看到悬空、泄漏、重复释放、越界 → 先从所有权和生命周期排查

## 🧩 Core Idea（核心本质）

- **本质**：内存管理是在管理“对象何时创建、何时销毁、谁负责释放”
- **关键机制**：C++ 依赖作用域、对象生命周期、RAII、拷贝 / 移动语义共同控制资源
- **核心目标**：让资源获取与释放绑定，避免泄漏、悬空和未定义行为

## 🔧 Usage Patterns（可复用代码模板）

### 1. 栈上对象（默认首选）

```cpp
string s = "hello";
vector<int> nums = {1, 2, 3};
```

### 2. 动态分配与释放（理解用，不是首选）

```cpp
int* p = new int(42);
cout << *p << '\\n';
delete p;
p = nullptr;
```

### 3. RAII：资源绑定到对象生命周期

```cpp
struct FileGuard {
	FILE* fp;
	FileGuard(const char* name) : fp(fopen(name, "r")) {}
	~FileGuard() {
		if (fp) fclose(fp);
	}
};
```

### 4. `unique_ptr` 独占所有权

```cpp
auto p = make_unique<int>(42);
cout << *p << '\\n';
```

### 5. `shared_ptr` 共享所有权

```cpp
auto p = make_shared<int>(42);
auto q = p;
cout << *q << '\\n';
```

### 6. 只借用对象，不转移所有权

```cpp
void print_size(const vector<int>& nums) {
	cout << nums.size() << '\\n';
}
```

### 7. 返回对象而不是返回裸内存

```cpp
vector<int> build_array() {
	return {1, 2, 3};
}
```

### 8. 动态数组优先用容器

```cpp
vector<int> nums(n);
```

## ⚠️ Pitfalls（高频错误）

- 返回局部变量地址 / 引用，会产生悬空引用或悬空指针
- `new` 了却忘记 `delete`，直接内存泄漏
- `delete` 后继续访问指针，变成悬空指针
- 同一块内存重复 `delete`，属于严重错误
- `new[]` 和 `delete[]`，`new` 和 `delete` 不能混用
- 以为“指针不为空就一定有效”，其实悬空指针也可能非空
- 容器扩容后旧迭代器 / 指针 / 引用可能失效
- `shared_ptr` 循环引用会导致资源不释放

## 🚀 Performance / Tips（性能优化）

- 默认优先：

```cpp
直接对象 > 容器 > 智能指针 > 裸 new/delete
```

- 需要动态对象时优先：

```cpp
make_unique<T>()
make_shared<T>()
```

- 大对象传参优先用 `const T&`，减少拷贝
- 频繁扩容的容器优先 `reserve()`，减少重分配
- 刷题和日常代码里，裸 `new/delete` 应尽量少出现

## 🧪 Common Scenarios（常见使用场景）

- **局部计算变量**：栈对象
- **动态长度数据**：`vector` / `string`
- **树 / 链表节点基础理解**：指针
- **资源自动释放**：RAII
- **明确独占所有权**：`unique_ptr`
- **多个对象共享资源**：`shared_ptr`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	auto p = make_unique<int>(42);
	cout << *p << '\\n';

	vector<int> nums = {1, 2, 3};
	cout << nums.size() << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 内存管理的核心不是记住 `new/delete`，而是先想清楚 **对象归谁拥有、会活多久、由谁释放**，然后优先用作用域、容器和 RAII 把释放动作自动化。