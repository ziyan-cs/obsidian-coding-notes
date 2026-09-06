---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 03-Docker Delivery (Docker 交付)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## 30 秒回答

**03-Docker Delivery (Docker 交付)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## Docker Multi Stage Build (Docker 多阶段构建)

> [!abstract] 核心考点：多阶段构建原理、C++ 二进制最小化、Alpine vs Ubuntu 抉择、CI 集成

## 为什么 C++ 需要多阶段构建

C++ 编译环境需要完整的工具链（cmake、g++、make、依赖库头文件），但**运行时只需要二进制和动态库**。多阶段构建让你用同一个 Dockerfile 完成"编译在一个镜像，跑在另一个更小的镜像"。

## 多阶段构建示例

```dockerfile
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake ninja-build gcc-12 g++-12 \
    libspdlog-dev libfmt-dev libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY CMakeLists.txt vcpkg.json ./
COPY cmake/ ./cmake/

COPY src/ ./src/
COPY proto/ ./proto/

RUN cmake -B release \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++-12 \
    && cmake --build release --target server --parallel $(nproc)

FROM ubuntu:22.04 AS runtime

RUN apt-get update && apt-get install -y \
    libspdlog1 libfmt8 libssl3 ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/release/server /app/server

COPY config/ /app/config/

EXPOSE 8080

HEALTHCHECK --interval=5s --timeout=3s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

CMD ["/app/server"]
```

**多阶段 vs 单阶段镜像大小对比：**

| 方案 | 镜像大小 | 安全风险 |
|------|---------|---------|
| 单阶段（build + run 混在一起） | 1.2-2.5 GB | 含编译器、头文件攻击面大 |
| 多阶段（分离 build/run） | 80-250 MB | 只有运行时必要组件 |

---

## 更极致的方案：Alpine + musl

```dockerfile
FROM alpine:3.18 AS builder
RUN apk add --no-cache cmake ninja g++ linux-headers spdlog-dev fmt-dev


FROM alpine:3.18 AS runtime
RUN apk add --no-cache spdlog fmt libssl3 ca-certificates tzdata

COPY --from=builder /build/release/server /app/server
COPY config/ /app/config/

CMD ["/app/server"]
```

**Alpine 注意：** musl libc 与 glibc 行为差异——DNS 解析、线程栈默认大小、信号处理都不同。**线上推荐用 Ubuntu slim**（更接近开发环境，不稳定因素少）。

---

## 依赖预缓存（CI 加速）

```dockerfile
FROM ubuntu:22.04 AS deps
COPY vcpkg.json CMakeLists.txt ./
RUN vcpkg install

FROM deps AS builder
COPY src/ ./src/
RUN cmake --build build --parallel
```

在 CI 中配合 Docker layer caching：

```yaml
- name: Set up Docker Buildx
  uses: docker/setup-buildx-action@v3
- name: Cache Docker layers
  uses: actions/cache@v4
  with:
    path: /tmp/.buildx-cache
    key: ${{ runner.os }}-buildx-${{ hashFiles('CMakeLists.txt', 'vcpkg.json') }}
- name: Build and push
  uses: docker/build-push-action@v5
  with:
    cache-from: type=local,src=/tmp/.buildx-cache
    cache-to: type=local,dest=/tmp/.buildx-cache
```

---

## 生产 Dockerfile 清单

| 配置 | 说明 |
|------|------|
| 非 root 运行 | `USER appuser`，不用 root 跑服务 |
| HEALTHCHECK | k8s 依赖健康检查做滚动更新 |
| `--rm` 清理 | CI 中构建完删掉中间镜像 |
| `.dockerignore` | 排除 `.git`、`build/`、`test/`、`node_modules` |
| 固定基础镜像 tag | `ubuntu:22.04` 而不是 `ubuntu:latest` |
| ca-certificates | gRPC/HTTPS 调用需要根证书 |
| tzdata | 设置 `TZ=Asia/Shanghai` 统一时区 |

### .dockerignore

```gitignore
.git/
build/
test/
cmake-build-*
.vscode/
.idea/
*.md
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 多阶段构建解决了什么 | 编译环境大 + 运行时不需要编译工具 = 分离 |
| Ubuntu vs Alpine for C++ | Ubuntu slim 更兼容，Alpine 更小但 musl 有坑 |
| 如何加速 Docker 构建 | 依赖层前置 + Docker layer cache + ccache |
| `COPY` 顺序为什么重要 | 经常变动的放后面，利用缓存减少构建时间 |
| 镜像安全 | 非 root、固定 tag、最小化包、定期扫描（trivy） |

> [!tip]- **工程要点**：C++ 服务 Docker 化的核心矛盾——编译环境巨大（2GB+）但运行时很小（几十 MB）。多阶段构建是标准解法。**千万不要**把编译工具链带到生产镜像里。Alpine 踩坑记录：musl 的 `std::thread` 默认栈大小只有 80KB（glibc 是 8MB），多线程服务器跑着跑着就崩了——线上老老实实 Ubuntu slim。

---

## 关联笔记

- [Makefile Basics (Makefile基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04a-Makefile%20Basics%20(Makefile基础).md)
- [Docker Basics：Image & Container (Docker基础)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04b-Docker%20Basics：Image%20&%20Container%20(Docker基础).md)
- [Shell Tools：grep, sed, awk, tmux (Shell效率工具)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04c-Shell%20Tools：grep,%20sed,%20awk,%20tmux%20(Shell效率工具).md)
- [Code Quality & Build Optimization：clang-tidy, ccache, Ninja, Benchmark (代码质量与构建加速)](/04-Engineering%20Tools%20(工程工具)/04-Other%20Tools%20(工具速查)/04d-Code%20Quality%20&%20Build%20Optimization：clang-tidy,%20ccache,%20Ninja,%20Benchmark%20(代码质量与构建加速)%20⭐.md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 03-Docker Delivery (Docker 交付) 的问题、核心机制与边界。

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
