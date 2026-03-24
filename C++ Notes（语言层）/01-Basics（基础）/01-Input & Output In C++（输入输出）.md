#cpp #basics #io

## ⚡ TL;DR（快速决策）

- 读数字、单词、基础类型 → `cin >>`
- 读整行文本 → `getline`
- `cin >>` 和 `getline` 混用 → 先 `cin.ignore(...)`
- 大输入量 → `ios::sync_with_stdio(false); cin.tie(nullptr);`
- 需要格式化输出 → `fixed << setprecision(n)`

## 🧩 Core Idea（核心本质）

- **本质**：C++ 输入输出是基于 _stream_ 的缓冲系统
- **关键机制**：`cin` / `cout` 操作的是缓冲区，不是每次都直接和终端交互
- **核心区别**：`cin >>` 以空白符为分隔，`getline` 读取整行

## 🔧 Usage Patterns（可复用代码模板）

### 1. 基础输入输出

```cpp
int x;
string s;
cin >> x >> s;
cout << x << ' ' << s << '\\n';
```

### 2. 读取整行

```cpp
string line;
getline(cin, line);
```

### 3. 混用 `cin >>` 和 `getline`

```cpp
int n;
cin >> n;
cin.ignore(numeric_limits<streamsize>::max(), '\\n');

string line;
getline(cin, line);
```

### 4. 多组输入

```cpp
int x;
while (cin >> x) {
	cout << x << '\\n';
}
```

### 5. 高性能 IO

```cpp
ios::sync_with_stdio(false);
cin.tie(nullptr);
```

### 6. 格式化输出

```cpp
double x = 3.1415926;
cout << fixed << setprecision(2) << x << '\\n';
```

### 7. 文件输入输出

```cpp
ifstream ifs("input.txt");
ofstream ofs("output.txt");

string line;
while (getline(ifs, line)) {
	ofs << line << '\\n';
}
```

## ⚠️ Pitfalls（高频错误）

- `cin >>` 后直接 `getline`，通常会先读到残留换行
- 高频输出时反复用 `endl`，会多做一次刷新，性能差于 `\\n`
- `cin >> s` 读不到空格后的内容
- 没开快速 IO 时，大数据输入输出可能卡常
- 浮点输出不设精度，结果可能不符合题目要求
- 文件流打开失败不检查，可能直接读写空内容

## 🚀 Performance / Tips（性能优化）

- 大输入量默认先写：

```cpp
ios::sync_with_stdio(false);
cin.tie(nullptr);
```

- 输出换行优先用 `\\n`，不要默认用 `endl`
- 竞赛 / 刷题里，`cin/cout` 开了快速 IO 后通常够用
- 需要严格格式控制时，优先想到 `<iomanip>`

## 🧪 Common Scenarios（常见使用场景）

- **刷题标准输入**：`cin >>`
- **读取一句话 / 含空格字符串**：`getline`
- **读到 EOF**：`while (cin >> x)`
- **高频读写**：快速 IO
- **日志 / 文件处理**：`ifstream` / `ofstream`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int x;
	cin >> x;
	cout << x << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 输入输出的核心不是记 API，而是先判断：**你读的是单词、整行，还是大规模数据流**，然后选对 `cin >>`、`getline` 和缓冲策略。