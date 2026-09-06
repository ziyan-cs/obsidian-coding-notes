---
tags:
  - devtools/tools
status: 🌱
---

# Makefile Basics — Makefile基础

> [!important] **核心考点**：规则语法、变量、自动变量、伪目标、增量构建原理

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
# 自动生成依赖文件（头文件改动时也能触发重编）
DEPS = $(OBJS:.o=.d)
-include $(DEPS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

# 静默模式（@开头不打印命令本身）
clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "Cleaned."

# 多目标
all: myapp mylib

# 查看 Makefile 推导过程
make -n        # dry run，只打印不执行
make -p        # 打印所有内置规则和变量
```

---

## 关联笔记

- [Docker Basics：Image & Container (Docker基础)](/08-Development%20Tools%20(工程开发工具)/04-Other%20Tools%20(工具速查)/04b-Docker%20Basics：Image%20&%20Container%20(Docker基础).md)
- [Shell Tools：grep, sed, awk, tmux (Shell效率工具)](/08-Development%20Tools%20(工程开发工具)/04-Other%20Tools%20(工具速查)/04c-Shell%20Tools：grep,%20sed,%20awk,%20tmux%20(Shell效率工具).md)
- [Code Quality & Build Optimization：clang-tidy, ccache, Ninja, Benchmark (代码质量与构建加速)](/08-Development%20Tools%20(工程开发工具)/04-Other%20Tools%20(工具速查)/04d-Code%20Quality%20&%20Build%20Optimization：clang-tidy,%20ccache,%20Ninja,%20Benchmark%20(代码质量与构建加速)%20⭐.md)
- [Docker Multi-stage Build for C++：Deploy Optimization (Docker多阶段构建与部署)](/08-Development%20Tools%20(工程开发工具)/04-Other%20Tools%20(工具速查)/04e-Docker%20Multi-stage%20Build%20for%20C++：Deploy%20Optimization%20(Docker多阶段构建与部署).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/08-Development%20Tools%20(工程开发工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
