#cpp #IO #文件操作 #fstream
## 文件分类：
1. 文本文件
2. 二进制文件
## 文件流类：
1. ofstream： 写操作
2. ifstream： 读操作
3. fstream： 读写操作
## C++ 文本文件 —— 写操作流程
### 1. *包含头文件*
	#include <fstream>
### 2. *创建流对象*
	ofstream ofs;
### 3. *打开文件*（含打开方式表）
	ofs.open("文件路径", 打开方式);

|  **打开方式**   |    **解释**     |
| :---------: | :-----------: |
|   ios::in   |   为读文件而打开文件   |
|  ios::out   | 为写文件而打开文件（默认） |
|  ios::ate   |   初始位置：文件尾    |
|  ios::app   |    追加方式写文件    |
| ios::trunc  | 如果文件存在先删除，再创建 |
| ios::binary |     二进制方式     |

### 4. *写数据*
	ofs << "写入的数据";
### 5. *关闭文件*
	ofs.close();
>代码实例：
```c++
#include <fstream> 
using namespace std;
int main(){
	// 1. 创建流对象并打开文件（默认ios::out方式）
	ofstream ofs("example.txt");
	// 2. 检查文件是否成功打开
	if(ofs.is_open()){
		// 3. 写入代码
		ofs << "Hello,C++ File I/O" << endl;
		ofs << "这是第二行文本" << endl;
		// 4. 关闭文件
		ofs.close();
	}
	return 0;
}
```
## 文本文件 - 读操作流程 
1. 包含头文件 2. 创建流对象 3. 打开文件（含打开成功判断） 4. 读数据（三种方式） 5. 关闭文件 > 代码实例