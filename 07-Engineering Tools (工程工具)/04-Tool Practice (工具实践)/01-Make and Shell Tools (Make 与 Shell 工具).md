---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 01-Make and Shell Tools (Make 与 Shell 工具)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## Makefile Basics (Makefile 基础)

> [!abstract] 核心考点：规则语法、变量、自动变量、伪目标、增量构建原理

## 基本语法

```makefile
目标: 依赖列表
	命令（必须用 Tab 缩进，不能用空格）
```

## 最小可用 Makefile

```makefile
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -g
TARGET   = myapp
SRCS     = main.cpp utils.cpp
OBJS     = $(SRCS:.cpp=.o)      # 字符串替换：.cpp → .o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean    # 声明 clean 是伪目标，不是文件名
```

## 自动变量

|变量|含义|
|---|---|
|`$@`|当前规则的目标文件名|
|`$<`|第一个依赖文件|
|`$^`|所有依赖文件（去重）|
|`$*`|模式匹配的词干（如 `%.o: %.cpp` 中的文件名部分）|

## 增量构建原理

Make 通过比较**目标文件与依赖文件的时间戳**决定是否重新构建：

- 若目标不存在 → 构建
- 若依赖比目标新 → 重新构建
- 否则 → 跳过

## 常用变量约定

```makefile
CC       = gcc           # C 编译器
CXX      = g++           # C++ 编译器
CFLAGS   = -Wall -O2     # C 编译选项
CXXFLAGS = -Wall -O2     # C++ 编译选项
LDFLAGS  = -lpthread     # 链接选项
```

## 实用技巧

```makefile
DEPS = $(OBJS:.o=.d)
-include $(DEPS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "Cleaned."

all: myapp mylib

make -n        # dry run，只打印不执行
make -p        # 打印所有内置规则和变量
```

---

## 关联笔记

- [Docker Basics：Image & Container (Docker基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04b-Docker%20Basics：Image%20&%20Container%20(Docker基础).md)
- [Shell Tools：grep, sed, awk, tmux (Shell效率工具)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04c-Shell%20Tools：grep,%20sed,%20awk,%20tmux%20(Shell效率工具).md)
- [Code Quality & Build Optimization：clang-tidy, ccache, Ninja, Benchmark (代码质量与构建加速)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04d-Code%20Quality%20&%20Build%20Optimization：clang-tidy,%20ccache,%20Ninja,%20Benchmark%20(代码质量与构建加速)%20⭐.md)
- [Docker Multi-stage Build for C++：Deploy Optimization (Docker多阶段构建与部署)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04e-Docker%20Multi-stage%20Build%20for%20C++：Deploy%20Optimization%20(Docker多阶段构建与部署).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

---

## Docker Images and Containers (Docker 镜像与容器)

> [!abstract] 核心考点：镜像 vs 容器、核心命令、Dockerfile 写法、网络与卷

> [!warning] 删除与清理命令要先看范围
> `prune`、`rm`、`rmi` 会改变本机资源；先列出目标并确认没有重要容器、镜像或卷。学习阶段用 `--rm` 和明确的容器名，避免积累难以辨认的残留。

## 核心概念

```
Dockerfile → (build) → Image → (run) → Container
镜像是模板（只读），容器是运行中的实例（可读写）
```

|概念|类比|说明|
|---|---|---|
|Image（镜像）|类 / 程序|只读的文件系统层叠快照|
|Container（容器）|对象 / 进程|镜像的运行实例，有独立的可写层|
|Registry|App Store|存储和分发镜像（Docker Hub、私有仓库）|
|Layer（层）|Git commit|镜像由多个只读层叠加而成，共享相同层|

---

## 镜像操作

```bash
docker pull ubuntu:22.04               # 拉取镜像
docker images                          # 列出本地镜像
docker image inspect ubuntu:22.04      # 查看镜像详情
docker rmi ubuntu:22.04                # 删除镜像
docker image prune                     # 清理悬空镜像（无标签）
docker build -t myapp:1.0 .            # 从当前目录 Dockerfile 构建
docker push myrepo/myapp:1.0           # 推送到仓库
docker save -o myapp.tar myapp:1.0     # 导出为 tar 文件
docker load -i myapp.tar               # 从 tar 导入
```

---

## 容器操作

```bash
docker run -it ubuntu:22.04 bash       # 交互式运行
docker run -d -p 8080:80 nginx         # 后台运行，端口映射 宿主:容器
docker run --name mycontainer myapp    # 指定容器名
docker run --rm myapp                  # 运行结束后自动删除容器

docker ps                              # 查看运行中的容器
docker ps -a                           # 查看所有容器（含已停止）
docker stop mycontainer                # 先请求停止；超时后强制结束（信号依平台/配置而异）
docker kill mycontainer                # 立即发送指定终止信号，默认是 KILL
docker rm mycontainer                  # 删除容器
docker container prune                 # 删除所有已停止的容器

docker exec -it mycontainer bash       # 进入运行中的容器
docker logs -f mycontainer             # 实时查看日志
docker stats                           # 查看所有容器资源使用
docker cp file.txt mycontainer:/tmp/   # 复制文件到容器
docker inspect mycontainer             # 查看容器详细信息
```

---

## Dockerfile 最佳实践

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        libssl-dev && \
    rm -rf /var/lib/apt/lists/*         # 清理缓存，减小镜像体积

WORKDIR /app                            # 设置工作目录

COPY CMakeLists.txt .
COPY src/ src/

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

FROM ubuntu:22.04 AS runtime
COPY --from=0 /app/build/myapp /usr/local/bin/

EXPOSE 8080
CMD ["myapp", "--port", "8080"]
```

### 多阶段构建（Multi-stage Build）

```dockerfile
FROM gcc:12 AS builder
WORKDIR /build
COPY . .
RUN g++ -O2 -o myapp main.cpp

FROM debian:bookworm-slim
COPY --from=builder /build/myapp /usr/local/bin/
CMD ["myapp"]
```

---

## 数据卷（Volume）

```bash
docker run -v mydata:/app/data myapp

docker run -v /host/path:/container/path myapp
docker run -v $(pwd):/app myapp          # 挂载当前目录

docker volume ls                         # 列出卷
docker volume inspect mydata             # 查看卷详情
docker volume prune                      # 删除未使用的卷
```

---

## 网络

```bash
docker network ls                        # 列出网络
docker network create mynet              # 创建自定义网络
docker run --network mynet myapp         # 指定网络

docker run --network mynet --name db postgres
docker run --network mynet myapp         # myapp 可以通过 "db" 访问数据库
```

## 30 秒回答

镜像（image）是可复用的只读模板，容器（container）是它启动后的运行实例和可写层。Dockerfile 描述如何构建镜像，volume 负责持久数据，network 负责容器通信。开发中把构建、配置、数据和日志的边界说清楚，比背命令更重要。

## 自测

1. 为什么容器删除后，未挂载在 volume 的数据通常不能作为持久数据依赖？
2. `docker stop` 和 `docker kill` 分别适合什么场景？
3. 多阶段构建为什么能降低最终镜像的攻击面和体积？

---

## 关联笔记（补充 2）

- [Makefile Basics (Makefile基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04a-Makefile%20Basics%20(Makefile基础).md)
- [Shell Tools：grep, sed, awk, tmux (Shell效率工具)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04c-Shell%20Tools：grep,%20sed,%20awk,%20tmux%20(Shell效率工具).md)
- [Code Quality & Build Optimization：clang-tidy, ccache, Ninja, Benchmark (代码质量与构建加速)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04d-Code%20Quality%20&%20Build%20Optimization：clang-tidy,%20ccache,%20Ninja,%20Benchmark%20(代码质量与构建加速)%20⭐.md)
- [Docker Multi-stage Build for C++：Deploy Optimization (Docker多阶段构建与部署)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04e-Docker%20Multi-stage%20Build%20for%20C++：Deploy%20Optimization%20(Docker多阶段构建与部署).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

---

## Shell Productivity Tools (Shell 效率工具)

> [!abstract] 核心考点：各工具的核心用法、组合管道使用、tmux 会话管理

## grep（文本搜索）

```bash
grep "pattern" file.txt              # 基础搜索
grep -r "pattern" ./src/             # 递归搜索目录
grep -i "pattern" file.txt           # 忽略大小写
grep -n "pattern" file.txt           # 显示行号
grep -v "pattern" file.txt           # 反向匹配（不含 pattern 的行）
grep -c "pattern" file.txt           # 只显示匹配行数
grep -l "pattern" *.cpp              # 只显示匹配的文件名
grep -A 3 "pattern" file.txt         # 显示匹配行及后 3 行（After）
grep -B 3 "pattern" file.txt         # 显示匹配行及前 3 行（Before）
grep -C 3 "pattern" file.txt         # 前后各 3 行（Context）

grep -E "error|warn" log.txt         # 扩展正则（等同 egrep）
grep -P "\d{3}-\d{4}" file.txt       # Perl 正则（支持 \d \s 等）
grep --include="*.cpp" -r "TODO" .   # 只搜索指定扩展名

grep -rn "FIXME" . | wc -l           # 统计 TODO 数量
ps aux | grep nginx | grep -v grep   # 查找进程，排除 grep 自身
```

---

## sed（流编辑器）

```bash
sed 's/old/new/' file.txt            # 替换每行第一个匹配
sed 's/old/new/g' file.txt           # 替换所有匹配（global）
sed 's/old/new/gi' file.txt          # 忽略大小写替换
sed -i 's/old/new/g' file.txt        # 原地修改文件（-i）
sed -i.bak 's/old/new/g' file.txt    # 原地修改并备份为 .bak

sed -n '5,10p' file.txt              # 只打印第 5~10 行（-n 抑制默认输出）
sed '3d' file.txt                    # 删除第 3 行
sed '/pattern/d' file.txt            # 删除匹配行
sed '/^$/d' file.txt                 # 删除空行
sed 's/^/    /' file.txt             # 每行开头加 4 个空格
sed 's/[[:space:]]*$//' file.txt     # 删除行尾空白

sed -e 's/foo/bar/g' -e 's/baz/qux/g' file.txt
sed '/START/,/END/d' file.txt        # 删除 START 到 END 之间的行
```

---

## awk（文本处理与报表）

awk 按行处理文本，每行按分隔符切割成字段（`$1`, `$2`, ...，`$0` 是整行）：

```bash
awk '{print $1}' file.txt            # 打印第一列
awk '{print $1, $3}' file.txt        # 打印第 1、3 列
awk -F: '{print $1}' /etc/passwd     # 以 : 为分隔符，打印用户名
awk 'NR==5' file.txt                 # 打印第 5 行（NR = 行号）
awk 'NR>=5 && NR<=10' file.txt       # 打印第 5~10 行

awk '/pattern/ {print $0}' file.txt  # 打印匹配行
awk '$3 > 100 {print $1, $3}' file.txt  # 条件过滤

awk '{sum += $1} END {print sum}' file.txt

awk '{count[$1]++} END {for (k in count) print k, count[k]}' file.txt

awk 'BEGIN {print "开始"} {print NR, $0} END {print "共", NR, "行"}' file.txt

awk -F',' '{print $2}' data.csv

ps aux | awk 'NR>1 {print $6, $11}' | sort -rn | head -5
```

---

## tmux（终端复用）

tmux 允许在一个终端中管理多个会话、窗口、面板，SSH 断开后会话依然保活。

### 会话管理

```bash
tmux                          # 新建会话
tmux new -s mywork            # 新建命名会话
tmux ls                       # 列出所有会话
tmux attach -t mywork         # 重新连接会话（SSH 断线后恢复）
tmux kill-session -t mywork   # 删除会话
```

### 快捷键（前缀键默认 Ctrl+B，记作  `<prefix>`）

```
<prefix> d          → detach（离开会话，后台保活）
<prefix> s          → 列出所有会话并切换
<prefix> $          → 重命名当前会话

<prefix> c          → 新建窗口
<prefix> n / p      → 下一个 / 上一个窗口
<prefix> 0~9        → 切换到第 N 个窗口
<prefix> ,          → 重命名当前窗口
<prefix> &          → 关闭当前窗口

<prefix> %          → 水平分割（左右）
<prefix> "          → 垂直分割（上下）
<prefix> 方向键      → 切换面板
<prefix> z          → 最大化/恢复当前面板
<prefix> x          → 关闭当前面板
<prefix> {  /  }    → 与相邻面板交换位置

<prefix> [          → 进入滚动模式（q 退出）
<prefix> ?          → 查看所有快捷键
```

### ~/.tmux.conf 常用配置

```bash
set -g prefix C-a
unbind C-b
bind C-a send-prefix

set -g mouse on

set -g base-index 1

set -g history-limit 10000

bind | split-window -h
bind - split-window -v
```

---

## 常用管道组合

```bash
find . -name "*.cpp" | xargs grep -v "^//" | grep -v "^$" | wc -l

tail -f app.log | grep --color "ERROR"

ss -tlnp | grep 8080
lsof -i :8080

grep -rl "OldName" ./src | xargs sed -i 's/OldName/NewName/g'

awk '{print $1}' access.log | sort | uniq -c | sort -rn | head -10

du -sh * | sort -rh | head -20

strace -p <pid> -e trace=network    # 只追踪网络相关系统调用
strace ./myapp 2>&1 | grep open     # 追踪文件打开操作
```

---

## 关联笔记（补充 3）

- [Makefile Basics (Makefile基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04a-Makefile%20Basics%20(Makefile基础).md)
- [Docker Basics：Image & Container (Docker基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04b-Docker%20Basics：Image%20&%20Container%20(Docker基础).md)
- [Code Quality & Build Optimization：clang-tidy, ccache, Ninja, Benchmark (代码质量与构建加速)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04d-Code%20Quality%20&%20Build%20Optimization：clang-tidy,%20ccache,%20Ninja,%20Benchmark%20(代码质量与构建加速)%20⭐.md)
- [Docker Multi-stage Build for C++：Deploy Optimization (Docker多阶段构建与部署)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04e-Docker%20Multi-stage%20Build%20for%20C++：Deploy%20Optimization%20(Docker多阶段构建与部署).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 01-Make and Shell Tools (Make 与 Shell 工具) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
