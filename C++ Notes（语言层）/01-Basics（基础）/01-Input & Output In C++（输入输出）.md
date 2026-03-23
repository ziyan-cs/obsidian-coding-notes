#cpp #file
## 一页速览

- **控制台输入**：`cin`
- **控制台输出**：`cout`
- **整行读取**：`getline(cin, s)`
- **错误输出**：`cerr`
- **文件读**：`ifstream`
- **文件写**：`ofstream`
- **最常考 / 最常错**：`cin` 和 `getline` 混用、输出格式控制、文件打开方式

## 一、先总后分：输入输出怎么分

### 1. 控制台输入输出

- 面向键盘 / 屏幕
- 常用：`cin`、`cout`、`cerr`

### 2. 文件输入输出

- 面向磁盘文件
- 常用：`ifstream`、`ofstream`、`fstream`

### 3. 快速判断怎么用

- 读一个数字 / 单词：`cin >> x`
- 读一整行：`getline(cin, s)`
- 普通输出：`cout << x`
- 读文件：`ifstream`
- 写文件：`ofstream`

---

## 二、控制台输入输出

### 1. `cin` / `cout`

```cpp
int a;
cin >> a;
cout << a << '\\n';
```

- `cin >>`：按空白符分隔读取
- `cout <<`：连续输出多个内容

### 2. `getline`

```cpp
string s;
getline(cin, s);
```

- 用于读取**一整行**，包含空格
- 适合姓名、句子、路径等输入

### 3. `cerr`

```cpp
cerr << "error\\n";
```

- 用于错误信息输出
- 和普通输出区分更清楚

---

## 三、必须记住的坑

### 1. `cin` 和 `getline` 混用

```cpp
int n;
cin >> n;
cin.ignore(numeric_limits<streamsize>::max(), '\\n');

string s;
getline(cin, s);
```

- `cin >>` 后会留下换行符
- 不清理缓冲区，`getline` 容易直接读到空行

### 2. `endl` 和 `\\n`

- `endl`：换行 + 刷新缓冲区
- `\\n`：只换行，效率更高
- 日常 / 刷题优先 `\\n`

### 3. 整数与浮点输出

```cpp
cout << 5 / 2 << '\\n';                  // 2
cout << static_cast<double>(5) / 2 << '\\n'; // 2.5
```

- 输出结果不只取决于 `cout`
- 也取决于表达式本身的类型

---

## 四、输出格式控制

```cpp
#include <iomanip>
double pi = 3.14159;
cout << fixed << setprecision(2) << pi; // 3.14
```

### 高频写法

- `fixed`：固定小数形式
- `setprecision(n)`：保留 `n` 位小数

### 速记

- 保留两位小数：`fixed << setprecision(2)`

---

## 五、文件输入输出

### 1. 三个文件流类

```cpp
#include <fstream>
```

- `ifstream`：读文件
- `ofstream`：写文件
- `fstream`：读写文件

### 2. 常见打开方式

- `ios::in`：读
- `ios::out`：写
- `ios::app`：追加
- `ios::trunc`：清空后重写
- `ios::binary`：二进制方式

### 3. 最常用模板

#### 写文件

```cpp
ofstream ofs("a.txt", ios::out);
if (!ofs.is_open()) return 0;
ofs << "hello" << '\\n';
ofs.close();
```

#### 读文件

```cpp
ifstream ifs("a.txt", ios::in);
if (!ifs.is_open()) return 0;
string line;
while (getline(ifs, line)) {
	cout << line << '\\n';
}
ifs.close();
```

### 4. 文件部分只记结论

- 文本文件最常用：`<<`、`>>`、`getline`
- 二进制文件最常用：`write`、`read`
- 读写前先判断是否打开成功
- 用完及时 `close()`

---

## 六、考试 / 刷题速记

1. `cin >>` 读到空白就停
2. `getline` 读取整行
3. `cin` 后接 `getline` 先 `ignore`
4. 输出优先用 `\\n`
5. 保留小数：`fixed << setprecision(n)`
6. 文件读写：`ifstream` / `ofstream`
7. 文件打开失败先判断 `is_open()`

---

## 七、最简模板

### 控制台输入输出

```cpp
int x;
string s;
cin >> x;
cin.ignore(numeric_limits<streamsize>::max(), '\\n');
getline(cin, s);
cout << x << '\\n' << s << '\\n';
```

### 文件输入输出

```cpp
ifstream ifs("a.txt");
string line;
while (getline(ifs, line)) {
	cout << line << '\\n';
}
```

<aside> 📌

这一节真正要背熟的，不是所有函数细节，而是：**什么时候用 `cin`，什么时候用 `getline`，什么时候要清缓冲区，什么时候用文件流。**

</aside>