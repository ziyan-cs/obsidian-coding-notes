
> **核心考点**：镜像 vs 容器、核心命令、Dockerfile 写法、网络与卷

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
docker stop mycontainer                # 优雅停止（发 SIGTERM）
docker kill mycontainer                # 强制停止（发 SIGKILL）
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
# 最终镜像从 ~1.4GB 缩小到 ~80MB
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