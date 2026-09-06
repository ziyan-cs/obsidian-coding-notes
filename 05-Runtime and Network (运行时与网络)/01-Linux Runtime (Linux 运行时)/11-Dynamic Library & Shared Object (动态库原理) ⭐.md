---
tags:
  - linux/syscall
status: 🌱
---

# 11-Dynamic Library & Shared Object (动态库原理)

> [!abstract] 核心考点：动态库与静态库的区别、共享对象 (.so) 的加载与链接、PLT/GOT 与位置无关代码

## 静态库 vs 动态库

| 特性 | 静态库 (.a) | 动态库 (.so) |
|------|------------|-------------|
| 链接时机 | 编译期 | 加载期或运行期 |
| 可执行文件大小 | 大（包含库代码） | 小（仅引用） |
| 部署 | 独立运行 | 依赖目标机器有 .so |
| 更新 | 需要重新链接 | 替换 .so 即可 |
| 内存共享 | 不行（每个进程有副本） | 可以（物理内存共享一份代码） |
| 加载速度 | 快（已包含） | 略慢（需动态链接） |

**静态库创建：**
```bash
gcc -c foo.c -o foo.o
ar rcs libfoo.a foo.o
gcc main.c -L. -lfoo -o prog
```

**动态库创建：**
```bash
gcc -fPIC -shared foo.c -o libfoo.so
gcc main.c -L. -lfoo -o prog       # 编译时链接
LD_LIBRARY_PATH=. ./prog            # 运行时指定路径
```

## 位置无关代码（PIC）

动态库加载时基地址不确定（ASLR），因此代码中的地址引用不能是绝对地址。`-fPIC` 编译的代码使用**相对寻址**：

- **全局偏移表（GOT）**：存放全局变量和函数指针的表，加载时填入实际地址
- **过程链接表（PLT）**：延迟绑定，函数首次调用时才解析地址

```
程序调用函数时：
  1. call 跳转到 PLT 表项
  2. PLT 跳转到 GOT 表项
  3. 首次调用时 GOT 指向 PLT 解析函数
  4. 后续调用直接跳转到目标函数地址
```

## 动态库加载流程

```
1. 可执行文件启动
2. ld.so（动态链接器/加载器）被内核加载
3. ld.so 解析 ELF 中的 .dynamic 段
4. 查找并加载所有依赖的 .so 文件
5. 重定位（填充 GOT/PLT 表项）
6. 执行初始化函数（.init）
7. 跳转到程序入口
```

**查看依赖：** `ldd ./prog` | `readelf -d ./prog`

**加载路径搜索顺序：**
1. `LD_LIBRARY_PATH` 环境变量
2. `/etc/ld.so.cache`（由 ldconfig 更新）
3. `/lib`、`/usr/lib` 等系统目录

## 运行时加载（dlopen）

```c
#include <dlfcn.h>

void* handle = dlopen("./libfoo.so", RTLD_LAZY);
if (!handle) { fprintf(stderr, "%s\n", dlerror()); return; }

void (*func)() = dlsym(handle, "foo_func");
if (func) func();  // 调用动态库中的函数

dlclose(handle);
```

**编译：** `gcc main.c -ldl -o prog`

## PLT/GOT 延迟绑定解析

```
首次调用 func()：
  main  →  call func@plt
  func@plt → jmp *GOT[func]        // GOT 初始指向下一条指令
              push reloc_index      // 压入重定位索引
              jmp  resolver         // 跳转到动态链接器
              resolver: 查找 func 地址 → 写入 GOT[func]
                                     
后续调用 func()：
  main → call func@plt
  func@plt → jmp *GOT[func]         // 直接跳转到 func
```

> [!tip]- **工程要点**：`-fPIC` 对性能有轻微影响（多一次间接寻址），但这是动态库和 ASLR 的必要代价。如果不需要共享，静态库性能更优。`LD_PRELOAD` 环境变量可以劫持系统库函数——这是很多调试/监控工具的底层原理。

动态库原理见 → [System Calls Overview (常用系统调用速查)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/10-System%20Calls%20Overview%20(常用系统调用速查).md) · [Debugging & Tracing (调试追踪)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/12-Debugging%20&%20Tracing：%20strace,%20gdb,%20perf%20(调试追踪)%20⭐.md)
