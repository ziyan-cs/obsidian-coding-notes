

# 0. 核心

# 1. 文件配置

### 1.1 标准 CMakeLists.txt 模板

- 文件名 `CMakeLists.txt`
- 选择编译方案：要注释掉另外两种

```cpp
# 1. 指定最低支持的 CMake 版本（兼容主流环境，3.10最稳妥）
cmake_minimum_required(VERSION 3.10)

# 2. 定义项目名称，会自动设置${PROJECT_NAME}变量
project("项目名")

# 3. 强制指定使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. 开启编译命令导出（方便VSCode等IDE代码补全、跳转）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 5. 把对应目录加入头文件搜索路径
include_directories(include)

# 方案1：单文件编译(目标名和源文件名一致)
add_executable("文件名" src/"文件名".cpp)

# 方案2：多文件自动编译(自动收集src目录下所有.cpp文件)
file(GLOB_RECURSE SOURCES src/ *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

# 方案3：通用可执行文件（固定 main 入口）
add_executable(main src/main.cpp)
```

```cpp
# 补充：链接库（例如链接 pthread 线程库） 
target_link_libraries( "目标名" pthread)
```

### 1.2 项目目录结构

```cpp
Project根目录/
├── build/          # 编译目录（必须单独创建）
├── include/        # 头文件目录（.h/.hpp）
├── src/            # 源文件（.cpp）
└── CMakeLists.txt  # 配置文件
```

### 1.3 日常编辑命令


```cpp
# 1. 进入编译目录
cd build

# 2. 生成构建文件（.. 指代上级目录下的 CMakeLists.txt）
cmake ..

# 3. 编译
make

# 4. 运行可执行文件
./main
```
