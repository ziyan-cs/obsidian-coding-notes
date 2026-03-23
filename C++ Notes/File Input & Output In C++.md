#cpp #file
## 一、先记结论

- **文本文件**：适合保存可读字符内容，如 `.txt`
- **二进制文件**：适合保存结构体、图片、音频等原始数据，效率更高
- **常用文件流类**：`ifstream` 负责读，`ofstream` 负责写，`fstream` 可读可写
- **工作 / 考试最常用**：文本文件读写、打开方式、逐行读取、二进制读写

---

## 二、文件分类

### 1. 文本文件

- 以字符形式存储
- 可以直接用记事本等工具查看
- 常见后缀：`.txt`、`.csv`、`.log`
- 适合保存配置、日志、普通文本内容

### 2. 二进制文件

- 以二进制形式存储
- 人眼无法直接阅读
- 存储效率高，适合保存原始数据
- 常见后缀：`.bin`、`.dat`

---

## 三、常用文件流类

```cpp
#include <fstream>
using namespace std;
```

- `ofstream`：写文件
- `ifstream`：读文件
- `fstream`：读写文件

### 速记

- `o` = output = 写
- `i` = input = 读
- `f` = file

---

## 四、文件打开方式

|打开方式|作用|
|---|---|
|`ios::in`|读方式打开|
|`ios::out`|写方式打开|
|`ios::app`|追加写入，内容写到文件末尾|
|`ios::ate`|打开后定位到文件尾|
|`ios::trunc`|若文件存在则先清空再写|
|`ios::binary`|二进制方式打开|

### 常见组合

```cpp
ios::out | ios::trunc      // 写文件，覆盖原内容
ios::out | ios::app        // 写文件，追加内容
ios::in                    // 读文件
ios::in | ios::binary      // 以二进制方式读文件
ios::out | ios::binary     // 以二进制方式写文件
```

---

## 五、文本文件写操作

### 标准流程

1. 包含头文件 `#include <fstream>`
2. 创建输出流对象 `ofstream ofs;`
3. 打开文件 `ofs.open("文件路径", 打开方式);`
4. 判断是否打开成功
5. 写入数据
6. 关闭文件

### 基础写法

```cpp
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    ofstream ofs;
    ofs.open("test.txt", ios::out);

    if (!ofs.is_open()) {
        cout << "文件打开失败" << endl;
        return 0;
    }

    ofs << "姓名：张三" << endl;
    ofs << "年龄：20" << endl;
    ofs << "专业：计算机" << endl;

    ofs.close();
    return 0;
}
```

### 追加写入写法

```cpp
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    ofstream ofs("log.txt", ios::out | ios::app);

    if (!ofs.is_open()) {
        cout << "文件打开失败" << endl;
        return 0;
    }

    ofs << "新增一条日志信息" << endl;
    ofs.close();
    return 0;
}
```

---

## 六、文本文件读操作

### 标准流程

1. 包含头文件 `#include <fstream>`
2. 创建输入流对象 `ifstream ifs;`
3. 打开文件 `ifs.open("文件路径", ios::in);`
4. 判断文件是否打开成功
5. 读取数据
6. 关闭文件

### 基础写法

```cpp
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main() {
    ifstream ifs;
    ifs.open("test.txt", ios::in);

    if (!ifs.is_open()) {
        cout << "文件打开失败" << endl;
        return 0;
    }

    string line;
    while (getline(ifs, line)) {
        cout << line << endl;
    }

    ifs.close();
    return 0;
}
```

### 4 种常见读取方式

#### 1. 逐字符读取

```cpp
char c;
while ((c = ifs.get()) != EOF) {
    cout << c;
}
```

#### 2. 逐行读取到字符数组

```cpp
char buf[1024];
while (ifs.getline(buf, sizeof(buf))) {
    cout << buf << endl;
}
```

#### 3. 逐行读取到 `string`（最常用）

```cpp
string line;
while (getline(ifs, line)) {
    cout << line << endl;
}
```

#### 4. 按空白分隔读取单词

```cpp
string word;
while (ifs >> word) {
    cout << word << endl;
}
```

### 读取方式怎么选

- **最推荐**：`getline(ifs, line)`
- **需要逐字符处理**：`ifs.get()`
- **需要按单词拆分**：`ifs >> word`

---

## 七、二进制文件写操作

### 关键点

- 必须使用 `ios::binary`
- 通常配合 `write()` 使用
- 常用于写结构体、数组、对象原始数据

### 示例：写入结构体到二进制文件

```cpp
#include <fstream>
#include <iostream>
using namespace std;

struct Person {
    char name[64];
    int age;
};

int main() {
    Person p = {"ZhangSan", 20};

    ofstream ofs("person.bin", ios::out | ios::binary);
    if (!ofs.is_open()) {
        cout << "文件打开失败" << endl;
        return 0;
    }

    ofs.write((const char*)&p, sizeof(p));
    ofs.close();
    return 0;
}
```

---

## 八、二进制文件读操作

### 关键点

- 必须使用 `ios::binary`
- 通常配合 `read()` 使用
- 读取时结构体类型要和写入时一致

### 示例：从二进制文件读取结构体

```cpp
#include <fstream>
#include <iostream>
using namespace std;

struct Person {
    char name[64];
    int age;
};

int main() {
    Person p;

    ifstream ifs("person.bin", ios::in | ios::binary);
    if (!ifs.is_open()) {
        cout << "文件打开失败" << endl;
        return 0;
    }

    ifs.read((char*)&p, sizeof(p));
    cout << "姓名：" << p.name << endl;
    cout << "年龄：" << p.age << endl;

    ifs.close();
    return 0;
}
```

---

## 九、考试高频易错点

- **写文件后要关闭文件**，否则数据可能没真正写入
- **读文件前要判断是否打开成功**
- **文本文件常用 `<<` 和 `>>` / `getline()`**
- **二进制文件常用 `write()` 和 `read()`**
- **二进制读写一定要加 `ios::binary`**
- **`getline()` 更适合读取整行文本**
- **`ios::app` 是追加，`ios::trunc` 是清空后重写**

---

## 十、工作 / 考试速查模板

### 1. 文本写文件模板

```cpp
ofstream ofs("test.txt", ios::out);
if (!ofs.is_open()) return 0;
ofs << "hello" << endl;
ofs.close();
```

### 2. 文本读文件模板

```cpp
ifstream ifs("test.txt", ios::in);
if (!ifs.is_open()) return 0;
string line;
while (getline(ifs, line)) {
    cout << line << endl;
}
ifs.close();
```

### 3. 二进制写文件模板

```cpp
ofstream ofs("data.bin", ios::out | ios::binary);
if (!ofs.is_open()) return 0;
ofs.write((const char*)&obj, sizeof(obj));
ofs.close();
```

### 4. 二进制读文件模板

```cpp
ifstream ifs("data.bin", ios::in | ios::binary);
if (!ifs.is_open()) return 0;
ifs.read((char*)&obj, sizeof(obj));
ifs.close();
```

---

## 十一、最简记忆版

- `ofstream`：写
- `ifstream`：读
- `fstream`：读写
- 文本文件：`<<`、`>>`、`getline()`
- 二进制文件：`write()`、`read()`
- 追加写：`ios::app`
- 覆盖写：`ios::trunc`
- 二进制方式：`ios::binary`