#cpp #basics #memory 

## ⚡ TL;DR（快速决策）

- 需要“间接访问对象” → 指针 `*`
- 需要“别名语义”且不想写空值判断 → 引用 `&`
- 只读大对象参数 → `const T&`
- 需要修改外部对象 → `T&` 或指针
- 能不用裸指针就少用裸指针，尤其在基础代码和刷题里

## 🧩 Core Idea（核心本质）

- **本质**：指针保存地址，引用是已有对象的别名
- **关键机制**：指针通过解引用访问目标对象，引用直接绑定到对象本身
- **核心区别**：指针可改指向、可为空，引用通常必须初始化且不能改绑

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础指针

```cpp
int x = 10;
int* p = &x;
cout << *p << '\\n';
```

### 2. 通过指针修改值

```cpp
int x = 10;
int* p = &x;
*p = 20;
cout << x << '\\n';
```

### 3. 最基础引用

```cpp
int x = 10;
int& ref = x;
ref = 20;
cout << x << '\\n';
```

### 4. 函数参数用引用修改外部变量

```cpp
void add_one(int& x) {
	++x;
}
```

### 5. 函数参数用 `const T&` 避免拷贝

```cpp
int get_size(const vector<int>& nums) {
	return nums.size();
}
```

### 6. 指针判空后访问

```cpp
if (p != nullptr) {
	cout << *p << '\\n';
}
```

### 7. 二级指针先有印象

```cpp
int x = 10;
int* p = &x;
int** pp = &p;
cout << **pp << '\\n';
```

### 8. 动态分配先有印象

```cpp
int* p = new int(42);
cout << *p << '\\n';
delete p;
p = nullptr;
```

## ⚠️ Pitfalls（高频错误）

- 野指针最危险，未初始化指针不能直接解引用
- 空指针 `nullptr` 不能直接 `*p`
- 引用必须初始化，不能先声明后绑定
- 引用不是新对象，修改引用就是修改原对象
- 指针和指针指向的值不要混，`p` 是地址，`*p` 才是值
- 返回局部变量地址会悬空
- `new` 后忘记 `delete` 会有内存泄漏风险
- `const` 位置不同，语义也不同，别混掉

## 🚀 Performance / Tips（性能优化）

- 大对象参数默认优先用：

```cpp
const string& s
const vector<int>& nums
```

- 基础业务 / 刷题里，能用引用表达语义时通常比裸指针更清晰
- 用 `nullptr`，不要再用旧式 `NULL`
- 裸指针主要用于：
    - 树 / 链表节点
    - 动态内存
    - 需要显式表示“可能为空”
- 如果只是“传进去改值”，优先先想引用

## 🧪 Common Scenarios（常见使用场景）

- **树 / 链表节点**：大量使用指针
- **函数避免拷贝**：`const T&`
- **函数回写参数**：`T&`
- **可选对象 / 空值语义**：指针
- **理解底层内存模型**：地址、解引用、间接访问

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

void add_one(int& x) {
	++x;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int x = 10;
	int* p = &x;
	cout << *p << '\\n';

	add_one(x);
	cout << x << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 指针解决的是**通过地址间接访问对象**，引用解决的是**给对象起一个别名**，核心不是语法符号，而是先想清楚你到底需要“可空的地址语义”还是“稳定的对象别名”。