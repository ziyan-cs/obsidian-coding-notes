#cpp #basics #input-output #cin #cout



 

# 1. I/O 基础

- `cin`：标准输入
- `cout`：标准输出
- `cerr`：标准错误，输出控制台
- `clog`：输出控制台，可被重定向到文件，常用于调试日志
- `ios::sync_with_stdio(false);`
- `cin.tie(nullptr);`

# 3. 核心 I/O 操作

### 3.1 输入流 `istream` 核心操作

- `cin.get(char& ch)`：读取单个字符
- `cin.getline(char* buf, int size)`：读取一行字符到字符数组
- `cin.ignore(int n = 1, int delim = EOF)`：跳过 n 个字符，遇到 `delim` 停止
- `cin.peek()`：查看下一个字符（不读取）
- `cin.putback(char ch)`：将字符放回输入缓冲区

### 3.2 输出流 `ostream` 核心操作

- `cout << endl`：**换行** + **刷新缓冲区**
- `cout << flush`：强制 **刷新缓冲区**
- `cout.setf(ios::format)`：设置输出格式
	- 如 `cout.setf(ios::left)` 左对齐，`cout.setf(ios::uppercase)` 大写输出
- `cout.unsetf(ios::format)`：取消格式设置
# 4. 文件 I/O

### 4.1 文件输入 `ifstream` 读文件

```cpp
#include <fstream>
#include <string>
using namespace std;

int main() {
    ifstream ifs("test.txt"); // 打开文件用于读取
    if (!ifs.is_open()) { // 判断文件是否成功打开
        cerr << "错误：文件打开失败！" << endl;
        return -1;
    }
    
    // 方式1：逐行读取
    string line;
    while (getline(ifs, line)) { // 读取整行，包括空格
        cout << line << endl;
    }
    
    // 方式2：按字符/单词读取
    char ch;
    while (ifs >> ch) { // 自动跳过空白符（空格/换行）
        cout << ch;
    }
    
    ifs.close(); // 关闭文件（必须！）
    return 0;
}
```

### 4.2 文件输出 `ofstream` 写文件

```cpp
#include <fstream>
#include <string>
using namespace std;

int main() {
    ofstream ofs("output.txt", ios::out | ios::trunc); 
    // ios::out 打开写入，ios::trunc 清空原有内容（默认）
    // 若追加内容：ios::app
    if (!ofs.is_open()) {
        cerr << "错误：文件创建失败！" << endl;
        return -1;
    }
    
    // 写入数据
    ofs << "Hello, 文件输出！" << endl;
    ofs << "数字：" << 123 << "，字符串：" << "test" << endl;
    
    ofs.close(); // 关闭文件
    return 0;
}
```


