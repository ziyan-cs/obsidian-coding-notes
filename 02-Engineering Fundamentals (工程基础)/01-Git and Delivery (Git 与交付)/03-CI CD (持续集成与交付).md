---
study_stage: backlog
tags: [engineering/ci-cd, engineering/delivery, security/supply-chain]
---

> [!abstract] 学习目标
> 把 CI/CD 设计为从源码提交到可验证制品与受控发布的证据链；理解触发器、权限、缓存、制品、环境门禁和回滚，而不是只会复制 YAML。

# CI、Delivery 与 Deployment

- **Continuous Integration**：小批量频繁集成，通过自动构建和验证尽早发现问题。
- **Continuous Delivery**：主干始终具备可发布状态，部署生产仍可人工批准。
- **Continuous Deployment**：所有通过门禁的变更自动进入生产。

三者不是工具品牌。流水线的核心产物是“某个源码版本在明确环境中生成并验证过的不可变制品”。

```text
commit/PR
  → 静态检查与快速测试
  → 多配置构建与测试
  → 安全/依赖检查
  → 生成一次制品 + digest/SBOM/provenance
  → 环境审批与渐进发布
  → 运行期验证
```

# 触发器与事件边界

PR 流水线处理不可信贡献时，应避免让代码获得仓库写权限和 secrets。特别要理解 `pull_request` 与 `pull_request_target` 的信任边界：后者在目标仓库上下文运行，若检出并执行攻击者代码可能泄露凭据。

同一分支的新提交可取消旧运行，减少浪费：

```yaml
concurrency:
  group: ci-${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true
```

触发条件要覆盖默认分支、PR、tag 和手工发布等实际流程，避免只在 feature branch 验证、合并后组合却从未测试。

# 最小权限与依赖固定

GitHub Actions 的 `GITHUB_TOKEN` 权限应显式最小化：

```yaml
permissions:
  contents: read
```

只有需要发布安全报告、包或 provenance 的 job 才增加对应写权限。云部署优先使用 OIDC 短期身份，避免长期云密钥。

第三方 action 也是供应链依赖。高风险环境应固定到完整 commit SHA，并由自动化工具跟踪升级；仅使用可变 tag 会让同一工作流在未来执行不同代码。

# 一份可解释的 C++ CI

```yaml
name: ci

on:
  pull_request:
  push:
    branches: [main]

permissions:
  contents: read

concurrency:
  group: ci-${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true

jobs:
  build-test:
    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-latest]
        compiler: [gcc, clang]
    runs-on: ${{ matrix.os }}
    steps:
      - uses: actions/checkout@<full-commit-sha>
      - name: Configure
        run: cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
      - name: Build
        run: cmake --build build --parallel
      - name: Test
        run: ctest --test-dir build --output-on-failure
```

示例中的 SHA 是有意占位：应从官方 action 发布信息选择实际版本并固定，而不是把笔记里的哈希长期复制到项目。

矩阵只覆盖有价值差异：编译器、操作系统、构建类型或关键依赖版本。组合爆炸时设置一个快速必过集合，再把昂贵覆盖放到夜间/发布流程，但不要让非必过任务永久失败无人处理。

# 缓存不是制品

- **cache**：可删除的性能优化，命中错误不得改变构建语义；key 必须包含锁文件、工具链和相关配置。
- **artifact**：本次运行需要保存、下载或交付的结果，具有明确保留期和摘要。
- **release artifact**：应由受控流程生成一次，随后在环境间提升同一制品，避免每个环境重新构建。

缓存投毒和跨信任边界恢复必须纳入威胁模型。排查疑似缓存错误时，先做无缓存干净构建。

# 门禁、环境与发布

GitHub environment 可绑定生产等目标，并设置审批、分支限制和 secrets。保护规则通过后，job 才能访问对应环境凭据。

可靠发布还需要：

- 数据库迁移的向前/向后兼容计划；
- canary、blue-green 或分批发布策略；
- 健康指标与自动停止条件；
- 回滚/roll-forward 路径和操作者权限；
- 将 commit、workflow run、制品 digest 与部署记录关联。

回滚应用版本不一定能回滚数据变更。破坏性 schema 修改应分阶段：先兼容读写，再迁移数据，最后删除旧结构。

# 制品完整性与 provenance

保存依赖清单/SBOM、构建参数、工具链或 runner 镜像、制品哈希。Artifact attestation 可声明制品由哪个仓库、commit 和 workflow 构建；只有消费端验证 attestation 时才产生安全价值。

# 失败诊断顺序

1. 确认失败 job、step 和首个根因，而非最后一串级联错误；
2. 对比本地与 CI 的工具链、环境变量、工作目录、权限和网络；
3. 检查锁文件、缓存 key、并行与测试隔离；
4. 使用相同容器/命令本地复现；
5. 修复后保留最小回归证据，禁止用无界重试掩盖 flaky test。

# 检查理解

1. Continuous Delivery 与 Deployment 的差别在哪里？
2. 为什么 cache 命中不能成为构建正确性的前提？
3. PR 中执行不可信代码时，为什么权限与 secrets 必须最小化？
4. 为什么应该“一次构建，多环境提升同一制品”？

> [!summary] 本篇结论
> CI/CD 是可追溯的变更证据链：最小权限验证源码，生成并标识不可变制品，通过环境门禁逐步发布，再由运行期指标验收。YAML 只是这一设计的实现。

## 权威依据

- [GitHub Actions workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax)
- [GitHub Actions security](https://docs.github.com/en/actions/how-tos/secure-your-work)
- [GitHub artifact attestations](https://docs.github.com/en/actions/concepts/security/artifact-attestations)

下一步：[01-CMake Project and Targets (CMake 项目与目标)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md)
