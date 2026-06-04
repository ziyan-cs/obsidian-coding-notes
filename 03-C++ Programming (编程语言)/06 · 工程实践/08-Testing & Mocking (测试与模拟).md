---
tags:
  - cpp
  - engineering
---

> **核心考点**：单元测试框架、测试金字塔、Mock 对象、TDD 基础、CI 集成

## 测试金字塔

```text
         ╱╲
        ╱ E2E ╲         少量：E2E Test（User场景）
       ╱────────╲
      ╱ Integration Test  ╲     适量：模块间协作测试
     ╱──────────────╲
    ╱  Unit Test       ╲   大量：函数/类级别的测试
   ╱────────────────────╲
```

| 层级 | 速度 | 数量 | 维护成本 |
|------|------|------|---------|
| 单元测试 | 毫秒级 | 多 | 低 |
| 集成测试 | 秒级 | 中 | 中 |
| E2E 测试 | 分钟级 | 少 | 高 |

## Google Test 基础

```cpp
#include <gtest/gtest.h>

// 测试一个函数
int add(int a, int b) { return a + b; }

TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(add(1, 2), 3);
    EXPECT_EQ(add(10, 20), 30);
}

TEST(AddTest, NegativeNumbers) {
    EXPECT_EQ(add(-1, -2), -3);
    EXPECT_EQ(add(-5, 5), 0);
}

// 测试 Fixture（多个测试共享设置）
class MyTest : public ::testing::Test {
protected:
    void SetUp() override { /* 每个 TEST_F 前执行 */ }
    void TearDown() override { /* 每个 TEST_F 后执行 */ }
    Database db_;  // 共享成员
};

TEST_F(MyTest, CanInsert) {
    EXPECT_TRUE(db_.insert("key", "value"));
}

TEST_F(MyTest, CanQuery) {
    db_.insert("key", "value");
    EXPECT_EQ(db_.query("key"), "value");
}
```

## 断言速查

| 断言 | 用途 |
|------|------|
| `EXPECT_TRUE(cond)` / `EXPECT_FALSE(cond)` | 布尔条件 |
| `EXPECT_EQ(a, b)` / `EXPECT_NE(a, b)` | 相等/不等 |
| `EXPECT_LT(a, b)` / `EXPECT_GT(a, b)` | 小于/大于 |
| `EXPECT_STREQ(s1, s2)` | C 字符串相等 |
| `EXPECT_THROW(stmt, exception_type)` | 预期抛异常 |
| `EXPECT_NO_THROW(stmt)` | 预期不抛异常 |
| `EXPECT_NEAR(a, b, eps)` | 浮点数近似相等 |

`ASSERT_*` 版本在失败时**终止当前测试**（而非继续）。`EXPECT_*` 则继续执行后续行。

## GMock

```cpp
#include <gmock/gmock.h>

// 1. 定义接口
class Database {
public:
    virtual ~Database() = default;
    virtual bool save(const std::string& key, int value) = 0;
    virtual int load(const std::string& key) = 0;
};

// 2. 使用 GMock 生成 Mock
class MockDatabase : public Database {
public:
    MOCK_METHOD(bool, save, (const std::string&, int), (override));
    MOCK_METHOD(int, load, (const std::string&), (override));
};

// 3. 测试中使用
TEST(ServiceTest, SaveData) {
    MockDatabase mock_db;
    EXPECT_CALL(mock_db, save("key", 42))
        .Times(1)
        .WillOnce(testing::Return(true));

    Service svc(&mock_db);
    EXPECT_TRUE(svc.saveData("key", 42));
}
```

### GMock 匹配器

```cpp
using ::testing::_;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Return;

EXPECT_CALL(mock, method(_, Ge(10)))      // 任何第一个参数 + >=10 的第二个参数
    .Times(AtLeast(1))
    .WillRepeatedly(Return(0));

EXPECT_CALL(mock, method(Eq("exact"), _))
    .Times(Exactly(2));                   // 精确调用 2 次
```

## Catch2 轻量测试框架

```cpp
// Catch2 是 header-only，更轻量
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("vectors can be resized") {
    std::vector<int> v;
    v.resize(10);
    REQUIRE(v.size() == 10);
    REQUIRE(v.capacity() >= 10);

    SECTION("resizing bigger changes size and capacity") {
        v.resize(15);
        REQUIRE(v.size() == 15);
    }

    SECTION("resizing smaller changes size but not capacity") {
        v.resize(5);
        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 10);  // vector 不会缩小 capacity
    }
}
```

## 测试驱动开发（TDD）流程

```text
1. 写一个失败的测试（RED）
2. 写最简代码让测试通过（GREEN）  
3. 重构代码（REFACTOR）
4. 重复
```

## 测试覆盖率

```bash
# GCC 覆盖率工具
g++ -coverage main.cpp -o main
./main
gcov main.cpp   # 生成 .gcov 文件
# 更直观的工具：lcov + genhtml
lcov -c -d . -o coverage.info
genhtml -o report coverage.info
```

## 工程实践

```cpp
// ✅ 测试应当：
// - 独立（不依赖外部状态）
// - 可重复（每次运行结果一致）
// - 快速（毫秒级）
// - 有好的失败信息（EXPECT_EQ(a, b) 比 EXPECT_TRUE(a==b) 好）

// ❌ 不要测试：
// - 内部实现细节（应测试公开接口的行为）
// - 第三方库的行为
// - 简单到不可能出错的代码

// ✅ 测试文件组织
// 建议：测试文件与源文件一一对应
// src/module.cpp → tests/module_test.cpp
```

> **工程要点**：好的单元测试是"**活的文档**"——阅读测试代码就能理解模块的预期行为。在 CI 中集成测试（`cmake --build . --target test`），确保每次提交都自动运行。**测试不是可选品**，是工程质量的基线。

---

## 关联笔记

- [Compilation & Linking (编译与链接)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/01-Compilation%20&%20Linking%20(编译与链接)%20⭐.md)
- [Header & Source Organization (头文件与源文件组织)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/02-Header%20&%20Source%20Organization%20(头文件与源文件组织).md)
- [Preprocessor & Macros (预处理与宏)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/03-Preprocessor%20&%20Macros%20(预处理与宏).md)
- [Exception Handling (异常处理)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/04-Exception%20Handling%20(异常处理).md)
- [Debugging gdb & Sanitizers (调试工具)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/05-Debugging%20gdb%20&%20Sanitizers%20(调试工具)%20⭐.md)
