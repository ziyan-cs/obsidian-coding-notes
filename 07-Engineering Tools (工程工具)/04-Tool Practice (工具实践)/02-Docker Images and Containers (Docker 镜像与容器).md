---
tags:
  - devtools/tools
status: learning
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 02-Docker Images and Containers (Docker 镜像与容器)

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
# 使用具体版本号，避免 latest 带来的不确定性
FROM ubuntu:22.04

# 合并 RUN 命令，减少镜像层数
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        libssl-dev && \
    rm -rf /var/lib/apt/lists/*         # 清理缓存，减小镜像体积

WORKDIR /app                            # 设置工作目录

# 先复制依赖文件，利用 Docker 层缓存
COPY CMakeLists.txt .
COPY src/ src/

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

# 多阶段构建（最终镜像只包含运行时需要的文件）
FROM ubuntu:22.04 AS runtime
COPY --from=0 /app/build/myapp /usr/local/bin/

EXPOSE 8080
CMD ["myapp", "--port", "8080"]
```

### 多阶段构建（Multi-stage Build）

```dockerfile
# 阶段1：编译（包含编译器等大型工具）
FROM gcc:12 AS builder
WORKDIR /build
COPY . .
RUN g++ -O2 -o myapp main.cpp

# 阶段2：运行（只有最小运行时）
FROM debian:bookworm-slim
COPY --from=builder /build/myapp /usr/local/bin/
CMD ["myapp"]
# 最终镜像通常只保留运行时需要的内容；实际体积取决于基础镜像和依赖
```

---

## 数据卷（Volume）

```bash
# 命名卷（Docker 管理，数据持久化）
docker run -v mydata:/app/data myapp

# 绑定挂载（指定宿主机路径，开发时常用）
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

# 同一自定义网络内的容器可以通过容器名互相访问
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

## 关联笔记

- [Makefile Basics (Makefile基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04a-Makefile%20Basics%20(Makefile基础).md)
- [Shell Tools：grep, sed, awk, tmux (Shell效率工具)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04c-Shell%20Tools：grep,%20sed,%20awk,%20tmux%20(Shell效率工具).md)
- [Code Quality & Build Optimization：clang-tidy, ccache, Ninja, Benchmark (代码质量与构建加速)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04d-Code%20Quality%20&%20Build%20Optimization：clang-tidy,%20ccache,%20Ninja,%20Benchmark%20(代码质量与构建加速)%20⭐.md)
- [Docker Multi-stage Build for C++：Deploy Optimization (Docker多阶段构建与部署)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04e-Docker%20Multi-stage%20Build%20for%20C++：Deploy%20Optimization%20(Docker多阶段构建与部署).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
