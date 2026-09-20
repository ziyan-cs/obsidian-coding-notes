---
study_stage: backlog
tags: [engineering/build, engineering/automation]
---

> [!abstract] 学习目标
> 把 GNU Make 看作按文件依赖执行的增量构建器，把 shell 看作独立解释层；能够维护正确的头文件依赖、并行构建和错误传播，而不靠手动删除 build 目录“修复”依赖图。

# Make 管理有向依赖图

规则由目标、先决条件和 recipe 组成：

```makefile
target: normal-prerequisites | order-only-prerequisites
	recipe
```

普通依赖不仅决定执行顺序，也参与时间戳过期判断；仅顺序依赖保证先构建，却不因其时间戳改变而触发目标重建。目标不存在也会触发构建。

Make 本身不解析 C++ 的 `#include`；如果头文件未纳入依赖图，增量构建可能错误地复用旧对象文件。

# 一份可实践的 GNU Makefile

项目有 `src/main.cpp`、`src/util.cpp` 和 `include/util.hpp`。以下 recipe 行**必须以 Tab 开头**；此例针对 GNU Make 与 GCC/Clang，Windows PowerShell 原生命令环境不保证能直接运行：

```makefile
CXX      ?= c++
CPPFLAGS := -Iinclude
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -MMD -MP
LDLIBS   :=
OBJS     := build/main.o build/util.o
DEPS     := $(OBJS:.o=.d)

.PHONY: all clean test
all: build/app

build/app: $(OBJS)
	$(CXX) $(OBJS) $(LDLIBS) -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p $@

-include $(DEPS)

test: build/app
	./build/app

clean:
	rm -f $(OBJS) $(DEPS) build/app
```

`-MMD -MP` 让编译器生成用户头文件依赖文件；`-include` 允许首次构建时 `.d` 尚不存在。`build` 目录作为 order-only prerequisite 避免其 mtime 改变时强制重编对象。输出 `build/app` 的规则依赖于对象文件，从而保持并行构建安全。

若目标名恰好为现存文件 `test` 或 `clean`，`.PHONY` 防止 Make 误以为任务已经完成；真实产物不能随意标为 phony，否则破坏增量构建。

# 自动变量、变量展开与 shell

| 表达式 | 含义 |
|---|---|
| `$@` | 当前目标 |
| `$<` | 第一个普通依赖 |
| `$^` | 所有普通依赖，去重 |
| `$(@D)` | 目标所在目录 |
| `$(VAR)` | Make 变量，在 recipe 执行前由 Make 展开 |
| `$$name` | 传给 shell 的 `$name` |

`:=` 创建立即展开变量；`=` 创建递归展开变量；`?=` 仅在尚未定义时赋值。变量与 recipe 是两层语言，Make 的 `$(...)` 与 shell 的 `$...` 不可混淆。

默认每行 recipe 在独立 shell 进程中执行：

```makefile
broken:
	cd src
	pwd          # 一般并不在 src
```

需共享工作目录时写成同一 recipe 行，例如 `cd src && command`；`.ONESHELL` 会改变每行 shell 规则，启用时要重新检查非零退出码的传播。

# Shell 自动化的失败语义

对 Bash 脚本可以使用：

```bash
#!/usr/bin/env bash
set -euo pipefail

source_dir=${1:?provide source directory}
cmake -S "$source_dir" -B build -G Ninja
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

`pipefail` 是 Bash 等 shell 的能力，不属于所有 `/bin/sh`。即使使用 `set -e`，在条件测试、`if`、`&&` 等上下文中行为也有例外；关键外部操作应明确处理返回码，而不是把 `set -e` 当通用异常系统。所有路径变量都应正确引用，避免空格、通配符和意外分词。

自动化脚本不应默认删除用户数据；若要清理，仅操作显式且已经检查的生成目录。不要用 `|| true` 隐藏真正的构建或测试失败。

# 并行与增量正确性

`make -j` 可加快独立任务执行，但要求每个生成文件有明确唯一的生产者、所有消费者声明真实依赖，不能由多个 recipe 同时写一个输出。

诊断流程：

```bash
make -n           # 预览将执行的 recipe，仍可能展开某些 Make 功能
make --debug=b    # 查看目标为什么被重建
make -j           # 验证声明的并行关系
```

修改 `src/main.cpp` 应只重编 `main.o` 和最终程序；修改 `include/util.hpp` 应重编确实包含它的对象；单独修改 `util.cpp` 不应重编 `main.o`。用输出证据验证图，不要仅依据“运行成功”判断依赖正确。

# 与 CMake 的分工

CMake 更适合跨平台项目声明目标和配置依赖；其生成器可选择 Ninja、Make、Visual Studio 等。直接手写 GNU Makefile 有助于理解图和增量构建，也适合简单 Unix 项目；不应把 GNU Make 专用命令塞进所有平台的 CMake 项目。

# 检查理解

1. 为什么缺少头文件依赖会导致增量构建产生错误制品？
2. `build` 目录为何适合作为 order-only prerequisite？
3. recipe 中 `$(VAR)` 与 `$$VAR` 的求值者分别是谁？
4. 为什么 `make -j` 能揭示串行构建时隐藏的依赖缺失？

> [!summary] 本篇结论
> Make 的正确性来自完整依赖图与唯一输出所有权；shell 的正确性来自清晰引用、退出状态与破坏性操作边界。速度只有建立在正确的增量和并行语义上才有意义。

## 权威依据

- [GNU Make Manual](https://www.gnu.org/software/make/manual/make.html)
- [GNU Make prerequisites](https://www.gnu.org/software/make/manual/html_node/Prerequisite-Types.html)
- [Bash Manual](https://www.gnu.org/software/bash/manual/bash.html)

下一步：[01-Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)
