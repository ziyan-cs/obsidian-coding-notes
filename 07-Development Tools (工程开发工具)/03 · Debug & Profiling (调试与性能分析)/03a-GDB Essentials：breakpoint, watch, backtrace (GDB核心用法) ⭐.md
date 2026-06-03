
> **核心考点**：GDB 启动方式、断点控制、变量观察、调用栈分析、多线程调试

## 启动方式

```bash
gdb ./myapp                        # 直接调试程序
gdb ./myapp core                   # 调试 core dump
gdb -p 12345                       # attach 到正在运行的进程
gdb --args ./myapp arg1 arg2       # 带参数启动
```

>编译时必须加 `-g` 保留调试信息，建议同时加 `-O0` 禁止优化：
> ```bash
> g++ -g -O0 -o myapp main.cpp
> ```

---

## 断点（Breakpoint）

```bash
# 设置断点
b main                    # 在函数入口打断点
b src/main.cpp:42         # 在指定文件第 42 行
b MyClass::method         # 在成员函数
b main.cpp:42 if x > 10  # 条件断点（满足条件才停）

# 管理断点
info breakpoints          # 列出所有断点（简写 i b）
disable 2                 # 禁用编号为 2 的断点
enable 2                  # 启用
delete 2                  # 删除
delete                    # 删除所有断点

# 临时断点（触发一次后自动删除）
tbreak main.cpp:50
```

---

## 执行控制

```bash
run                       # 启动程序（简写 r）
run arg1 arg2             # 带参数运行
continue                  # 继续运行到下一个断点（简写 c）
next                      # 单步执行，不进入函数（简写 n）
step                      # 单步执行，进入函数（简写 s）
finish                    # 执行完当前函数并返回
until 60                  # 运行到第 60 行（跳出循环用）
return                    # 强制从当前函数返回（可指定返回值）
kill                      # 终止程序
quit                      # 退出 GDB（简写 q）
```

---

## 查看变量与内存

```bash
print x                   # 打印变量 x（简写 p）
print *ptr                # 打印指针指向的值
print arr[0]@5            # 打印数组前 5 个元素
print (int)var            # 强制类型转换后打印

display x                 # 每次停下来自动打印 x
undisplay 1               # 取消 display

info locals               # 打印当前栈帧所有局部变量
info args                 # 打印当前函数参数

x/10xw 0x7fff1234        # 查看内存：10个单元，十六进制，word(4字节)
# 格式：x/[数量][格式][单位]  格式:x=hex,d=dec,s=str  单位:b=1,h=2,w=4,g=8
```

---

## 观察点（Watchpoint）

当变量值改变时自动停下，用于追踪"某个变量是在哪里被修改的"：

```bash
watch x                   # 当 x 的值改变时停下
watch *0x7fff1234         # 监视内存地址
rwatch x                  # 当 x 被读取时停下
awatch x                  # 当 x 被读或写时停下
info watchpoints          # 查看所有观察点
```

---

## 调用栈（Backtrace）

```bash
backtrace                 # 打印调用栈（简写 bt）
bt full                   # 打印调用栈 + 每帧局部变量
bt 5                      # 只显示最近 5 帧

frame 3                   # 切换到第 3 帧（简写 f 3）
info frame                # 显示当前帧详情
up                        # 上移一帧（调用者）
down                      # 下移一帧（被调用者）
```

---

## 多线程调试

```bash
info threads              # 列出所有线程
thread 3                  # 切换到线程 3
thread apply all bt       # 打印所有线程的调用栈（排查死锁必备）
thread apply all bt full  # 所有线程的完整调用栈 + 局部变量

set scheduler-locking on  # 只运行当前线程，其他线程冻结（单步调试用）
set scheduler-locking off # 恢复所有线程运行
```

---

## 常用 TUI 模式

```bash
gdb -tui ./myapp          # 启动带源码窗口的 TUI 模式
layout src                # 显示源码窗口
layout asm                # 显示汇编窗口
layout split              # 源码 + 汇编同时显示
Ctrl+L                    # 刷新屏幕（TUI 花屏时用）
```