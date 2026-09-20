---
study_stage: backlog
---

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

> [!summary] 核心摘要
>
> 镜像封装应用与运行依赖，容器是镜像的运行实例。交付时用多阶段构建和非 root 用户缩小镜像与攻击面，把配置、密钥和持久数据放在镜像之外。


这篇先建立三层概念：镜像是只读层与元数据组成的交付物；容器是镜像加可写层、进程和隔离配置的一次运行；编排器负责在节点上启动与替换容器。容器共享宿主内核，依靠 namespace、cgroup 等机制隔离与限额，不是完整虚拟机。镜像层不可变，容器可写层随容器删除而消失；持久数据要使用卷或外部存储。[Docker 容器概览](https://docs.docker.com/get-started/docker-overview/) · [Docker 存储](https://docs.docker.com/engine/storage/)

# 为什么 C++ 需要多阶段构建

C++ 编译环境需要工具链和开发头文件，运行时至少要有二进制、它实际依赖的动态库以及所需证书/时区数据。多阶段构建只把所需产物拷入运行阶段，不会自动把所有依赖一并复制；`ldd`、启动测试和镜像扫描仍不可省。跨阶段尽量保持 libc/ABI 兼容。

# 多阶段构建示例

下面模板假设项目有 `CMakeLists.txt`，目标名为 `server`，输出为 `/build/server`，且只依赖 glibc/libstdc++。若实际链接 OpenSSL、fmt、protobuf 等，必须在运行阶段安装对应 ABI 的运行库或复制适合分发的产物；不要把这段当成适用于任意项目的即贴即跑 Dockerfile。

```dockerfile
# syntax=docker/dockerfile:1
FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN cmake -S . -B /build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target server --parallel 2 \
    && ldd /build/server

FROM debian:bookworm-slim AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 ca-certificates \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build --chown=10001:10001 /build/server /app/server
USER 10001:10001
EXPOSE 8080
ENTRYPOINT ["/app/server"]
```

`EXPOSE` 只记录意图，不会自动发布端口。运行时配置与 Secret 由环境、只读卷或秘密系统注入，不要 `COPY config/` 把生产凭据烘焙进镜像。镜像体积与安全收益必须以实际依赖、扫描结果和运行测试衡量，不能背固定大小数字。[Docker 多阶段构建](https://docs.docker.com/build/building/multi-stage/)

---

# 运行时兼容性、缓存与验证

`alpine` 使用 musl，许多常见 Debian/Ubuntu 镜像使用 glibc。镜像小不等于更安全或更快：若把在 glibc 环境编译的可执行文件直接复制到 Alpine，动态加载器和依赖库可能缺失。选择基础镜像时看 ABI、补丁供应、证书、排障与目标平台；没有“线上统一推荐 Ubuntu slim”的定律。

构建缓存的关键是**先固定依赖输入，再复制高频变化的源码**，但 CMake/vcpkg 的依赖图、toolchain 和 lockfile 必须真实接入，不能写 `RUN vcpkg install` 却没有安装 vcpkg，也不能跳过 CMake 配置直接 `cmake --build build`。上面的最小模板用 `COPY . .` 换取易懂；项目稳定后再拆依赖层、用 BuildKit cache mount，并验证缓存命中不会漏掉头文件和构建选项变更。

```bash
docker build --pull -t cpp-server:local .
docker run --rm -p 8080:8080 cpp-server:local
docker image inspect cpp-server:local
```

运行前确认程序确实监听容器内 `0.0.0.0:8080`，能在 SIGTERM 后排空并退出，日志写 stdout/stderr，所需目录在只读根文件系统或挂载卷下仍可用。上述 `ENTRYPOINT` 采用 exec 形式让程序成为容器主进程并直接接收终止信号；若用 shell 包装，要处理信号转发与子进程回收。构建成功还应做启动、依赖、权限、证书、架构与漏洞扫描测试。

---

# 生产 Dockerfile 清单

| 配置 | 说明 |
|------|------|
| 非 root 运行 | 以无特权 UID/GID 启动，确认文件和挂载权限 |
| 健康检查 | Dockerfile `HEALTHCHECK` 由 Docker Engine 使用；Kubernetes 要在 Pod 中配置 startup/readiness/liveness probes，不能只依赖 Dockerfile 指令 |
| `.dockerignore` | 排除无关的大型/敏感文件，但不要误排除构建所需的测试、CMake 或依赖清单 |
| 基础镜像固定 | 部署产物用不可变 digest；基础镜像定期主动更新并复测，固定 tag 本身不保证内容永远不变 |
| ca-certificates | gRPC/HTTPS 调用需要根证书 |
| 时区 | 日志和跨系统时间优先使用 UTC；需要本地时区转换时再明确依赖 tzdata |

## .dockerignore

```gitignore
.git/
build/
cmake-build-*
.vscode/
.idea/
*.pem
*.key
```

`.dockerignore` 是示例，私钥还应在密钥管理与构建上下文中被彻底排除；不要以为加一条忽略规则就完成了秘密治理。

# 验收实验

1. 构建镜像后，用实际运行环境执行 `ldd` 或等效依赖检查，确认没有 `not found`，并验证启动、HTTP 请求和 HTTPS 证书链。
2. 检查进程 UID、镜像中的工具链与配置文件，确认没有把编译器、私钥或生产配置带进运行镜像。
3. 发送 SIGTERM，观察是否停止接新请求、处理在途请求并在停止期限内退出；再验证容器重建后持久数据仍来自卷或外部服务。
4. 修改源文件重新构建，记录依赖层是否命中缓存；更新基础镜像后重新运行全部测试，而不是永久锁死旧镜像。
