---
status: stable
confidence: high
content_verified: 2026-09-17
---

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

# Makefile Basics (Makefile 基础)

> [!note] 本节重点：规则语法、变量、自动变量、伪目标、增量构建原理

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

# 把 Make 看成依赖图执行器

Make 读取规则构造有向依赖图，再从目标递归判断哪些节点过期。recipe 是否执行只依赖目标存在性和时间戳关系；Make 不理解 C++ `#include`，因此必须让编译器生成 `.d` 依赖文件，否则头文件变化可能不会触发重编译。

```makefile
CPPFLAGS := -Iinclude
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -MMD -MP
LDLIBS   := -pthread

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)
```

变量展开和 shell 执行是两层语言：`$(VAR)` 由 Make 展开，`$$name` 才把 `$name` 交给 shell；默认每个 recipe 行可能在独立 shell 中执行，需要共享工作目录/变量时写成同一行或使用 `.ONESHELL` 并理解失败传播。

并行执行 `make -j` 要求规则正确声明真实依赖，多个 recipe 不能无协调地写同一文件。`.PHONY` 只用于不代表文件的动作；把真实产物声明为 phony 会失去增量构建。

诊断顺序：`make -n` 查看将执行的命令，`make --debug=b` 解释为何重建，删除产物做干净构建，修改一个头文件验证影响范围。recipe 返回非零时 Make 默认停止；不要用前缀 `-` 或 `|| true` 隐藏关键失败。

实践：为两源文件项目生成 `.d`，先全量构建，再只改 `.cpp` 和公共头文件，记录哪些对象被重建；随后用 `make -j` 验证规则无竞争。

参考：[GNU Make Manual](https://www.gnu.org/software/make/manual/make.html)。
