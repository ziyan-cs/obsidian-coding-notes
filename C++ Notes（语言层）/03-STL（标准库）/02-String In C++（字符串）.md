#cpp #STL #容器 #string
## 一页速览

- **`string`**：C++ 里最常用的字符串类型
- **核心特点**：比字符数组更方便、更安全
- **核心优势**：支持拼接、比较、截取、查找等现成操作
- **常见操作**：输入、访问字符、拼接、截取、遍历、查找
- **高频接口**：`size`、`empty`、`substr`、`find`、`push_back`、`pop_back`
- **高频场景**：字符串处理题、模拟题、双指针、哈希前处理
- **最常错**：`cin` 和 `getline` 混用、下标越界、`find` 返回值不会判断、字符和字符串混淆

## 一、先总后分：这一节到底学什么

### 1. `string` 解决什么问题

- C 风格字符数组写起来麻烦
- 字符串长度常常不固定
- `string` 能让字符串处理更自然

### 2. `string` 最核心看什么

- 怎么定义和输入
- 怎么访问字符
- 怎么拼接和截取
- 怎么查找和遍历

### 3. 当前阶段先抓主线

- 会定义 `string`
- 会写最常见的输入输出
- 会操作单个字符和子串
- 会解决基础字符串题

---

## 二、`string` 是什么

### 1. 基本理解

- `string` 是一个字符串类
- 可以理解成“更灵活的字符序列”
- 比字符数组更常用

```cpp
#include <string>
string s = "hello";
```

### 2. 当前阶段怎么理解

- `string` 本质上是对象
- 但平时使用时可以先把它当成“好用的字符串容器”

### 3. 当前阶段先记一句话

- **`string` = C++ 里更方便的字符串类型**

---

## 三、`string` 的定义与初始化

### 1. 空字符串

```cpp
string s;
```

### 2. 直接初始化

```cpp
string s = "hello";
```

### 3. 构造指定内容

```cpp
string s("world");
```

### 4. 重复字符初始化

```cpp
string s(5, 'a');
```

- 结果是 `"aaaaa"`

### 5. 当前阶段先记

- **最常见的就是空串、直接赋值、重复字符构造**

---

## 四、`string` 的输入输出

### 1. `cin >> s`

```cpp
string s;
cin >> s;
```

- 读到空白符就停止
- 适合读一个单词

### 2. `getline(cin, s)`

```cpp
string s;
getline(cin, s);
```

- 读取一整行
- 适合带空格的输入

### 3. `cin` 和 `getline` 混用

```cpp
int n;
cin >> n;
cin.ignore(numeric_limits<streamsize>::max(), '\\n');

string s;
getline(cin, s);
```

- `cin >>` 后会留下换行符
- 不清理缓冲区，`getline` 容易直接读到空行

### 4. 当前阶段先记

- **读单词用 `cin`**
- **读整行用 `getline`**

---

## 五、`string` 的访问方式

### 1. 下标访问字符

```cpp
string s = "hello";
cout << s[0] << '\\n'; // h
cout << s[4] << '\\n'; // o
```

### 2. 头尾字符

```cpp
cout << s.front() << '\\n';
cout << s.back() << '\\n';
```

### 3. 当前阶段怎么理解

- `s[i]`：访问第 `i` 个字符
- `front()`：第一个字符
- `back()`：最后一个字符

### 4. 最常错的地方

- 下标从 `0` 开始
- 最后一个下标是 `size() - 1`

---

## 六、`string` 的常见操作

### 1. 获取长度 `size`

```cpp
cout << s.size() << '\\n';
```

### 2. 判空 `empty`

```cpp
if (s.empty()) {
	cout << "empty" << '\\n';
}
```

### 3. 拼接字符串

```cpp
string a = "hello";
string b = "world";
string c = a + b;
```

### 4. 尾部加入字符

```cpp
s.push_back('!');
```

### 5. 尾部删除字符

```cpp
s.pop_back();
```

### 6. 当前阶段先记

- **做题最常用：`size`、`empty`、`+`、`push_back`、`pop_back`**

---

## 七、`string` 的遍历

### 1. 下标遍历

```cpp
for (int i = 0; i < s.size(); ++i) {
	cout << s[i] << '\\n';
}
```

### 2. 范围 `for` 遍历

```cpp
for (char ch : s) {
	cout << ch << '\\n';
}
```

### 3. 引用遍历

```cpp
for (char& ch : s) {
	ch = toupper(ch);
}
```

### 4. 当前阶段怎么理解

- 只读遍历：`for (char ch : s)`
- 要修改字符：`for (char& ch : s)`

---

## 八、子串与查找

### 1. 截取子串 `substr`

```cpp
string s = "abcdef";
cout << s.substr(2, 3) << '\\n'; // cde
```

- 从下标 `2` 开始
- 长度是 `3`

### 2. 查找 `find`

```cpp
string s = "hello world";
cout << s.find("world") << '\\n';
```

### 3. 找不到怎么办

```cpp
if (s.find("abc") == string::npos) {
	cout << "not found" << '\\n';
}
```

### 4. 当前阶段先记

- **`find` 找到返回位置**
- **找不到返回 `string::npos`**

---

## 九、`string` 和字符数组的区别

|对比项|字符数组|`string`|
|---|---|---|
|长度|通常固定|可变|
|使用难度|较麻烦|更方便|
|常用操作|较少|很多现成接口|
|推荐程度|基础了解|刷题高频使用|

### 当前阶段先记结论

- **做题默认优先想 `string`**
- 除非题目明确要求字符数组风格处理

---

## 十、`string` 最常见应用

### 1. 翻转字符串

```cpp
reverse(s.begin(), s.end());
```

### 2. 排序字符串字符

```cpp
sort(s.begin(), s.end());
```

### 3. 判断回文串

- 常配合双指针
- 左右两边往中间比较

### 4. 统计字符出现

- 常配合数组、哈希表、`map`

### 5. 当前阶段怎么理解

- `string` 经常和双指针、排序、哈希一起出现
- 字符串题本质还是“对字符序列做操作”

---

## 十一、`string` 与函数

### 1. 值传递

```cpp
void printStr(string s) {
	...
}
```

- 会拷贝一份字符串
- 字符串长时开销更大

### 2. 引用传递

```cpp
void printStr(const string& s) {
	cout << s << '\\n';
}
```

- 不会整体拷贝
- 只读时最推荐

### 3. 当前阶段先记

- **函数参数里，大多数字符串优先写 `const string&`**

---

## 十二、现代 C++ 的基础习惯

### 1. 刷题里优先考虑 `string`

- 它比字符数组更自然、更安全

### 2. 看到空格先想 `getline`

- 不要默认所有输入都能用 `cin >>`

### 3. 注意字符和字符串的区别

- 字符：`'a'`
- 字符串：`"a"`

### 4. 以后会继续补

- `insert`
- `erase`
- 迭代器
- KMP 等字符串算法
- 但当前阶段先把基础操作学稳

---

## 十三、高频易错点

1. `cin >> s` 读到空白就停
2. `getline` 读取整行
3. `cin` 和 `getline` 混用时记得清缓冲区
4. `string` 下标从 `0` 开始
5. 最后一个字符位置是 `size() - 1`
6. `find` 找不到返回 `string::npos`
7. `pop_back()` 删除最后一个字符，但不返回值
8. 字符和字符串不要混写

---

## 十四、最简模板

### 定义模板

```cpp
#include <string>
string s;
```

### 输入模板

```cpp
string s;
getline(cin, s);
```

### 遍历模板

```cpp
for (int i = 0; i < s.size(); ++i) {
	cout << s[i] << '\\n';
}
```

### 查找模板

```cpp
if (s.find("abc") != string::npos) {
	cout << "found" << '\\n';
}
```

<aside> 📌

这一节真正要掌握的，不只是“`string` 是什么”，而是看到题目时能立刻判断：**这里是在处理单词、整行、单个字符，还是在对整段字符序列做查找、截取和修改。**

</aside>