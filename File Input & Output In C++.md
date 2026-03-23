#cpp #file
## 文件分类

- 文本文件：以 ASCII 码形式存储，可直接阅读（如 .txt 格式），内容为人类可识别的字符。
- 二进制文件：以二进制形式存储，需特定程序解析（如 .bin 格式），存储效率更高、数据更完整。

## 文件流类

1. ofstream：写操作
2. ifstream：读操作
3. fstream：读写操作

## 文本文件 —— 写操作流程

1. 包含头文件

```cpp
#include <fstream>
```

1. 创建流对象

```cpp
ofstream ofs;
```

1. 打开文件（含打开方式表）

```cpp
ofs.open("文件路径", 打开方式);
```

打开方式解释

- ios::in：为读文件而打开文件
- ios::out：为写文件而打开文件（默认）
- ios::ate：初始位置：文件尾
- ios::app：追加方式写文件
- ios::trunc：如果文件存在先删除，再创建
- ios::binary：二进制方式

1. 写数据

```cpp
ofs << "写入的数据";
```

1. 关闭文件（含打开失败判断）

```cpp
ofs.close();
```

代码示例

```cpp
#include <fstream>
#include <iostream>
using namespace std;

int main() {
	// 1. 创建流对象并打开文件（默认 ios::out 方式）
	ofstream ofs("example.txt");

	// 2. 检查文件是否成功打开
	if (ofs.is_open()) {
		// 3. 写入数据
		ofs << "Hello, C++ File I/O" << endl;
		ofs << "这是第二行文本" << endl;

		// 4. 关闭文件
		ofs.close();
	} else {
		cout << "文件打开失败！" << endl;
	}

	return 0;
}
```

## 文本文件 —— 读操作流程

1. 包含头文件

```cpp
#include <fstream>
```

1. 创建流对象

```cpp
ifstream ifs;
```

1. 打开文件（含打开失败判断）

```cpp
ifs.open("文件路径", ios::in);
if (!ifs.is_open()) {
	cout << "文件打开失败！" << endl;
	return 0;
}
```

1. 读数据（三种方式）

```cpp
// 方式1：逐字符读
char c;
while ((c = ifs.get()) != EOF) {
	cout << c;
}

// 方法2. 自带函数
char buf[1024];
while (ifs.getline(buf,sizeof(buf))){
	cout << buf << endl;
}

// 方式3：逐行读 (最优解)
string buf;
while (getline(ifs, buf)) {
	cout << buf << endl;
}

// 方式4：逐单词读
string word;
while (ifs >> word) {
	cout << word << endl;
}
```

1. 关闭文件

```cpp
ifs.close();
```