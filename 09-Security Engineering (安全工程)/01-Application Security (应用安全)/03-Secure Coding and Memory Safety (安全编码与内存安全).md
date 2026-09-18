---
status: learning
confidence: low
content_verified: 2026-09-18
tags: [security/secure-coding, language/cpp]
---

> [!abstract] 学习定位
> 安全编码要求数据在解析、验证、使用和释放的整个生命周期中遵守明确边界。

> [!summary] 核心摘要
>
> 不可信输入先限制大小与结构，再转换为内部类型；C++ 还要用 RAII、边界安全容器和动态分析工具降低悬空指针、越界与整数错误风险。

# 信任边界与输入路径

来自网络、文件、数据库、环境变量、消息队列和模型输出的数据都可能不可信。验证应发生在系统边界，并将原始输入转换成已经满足业务约束的内部类型。

验证至少覆盖长度、编码、范围、枚举值、资源消耗与字段关系。能解析 JSON 不代表数据合法；通过正则也不代表后续 SQL、Shell 或路径操作安全。

# C++ 内存与整数风险

- 优先使用 RAII 和拥有明确所有权的类型，避免裸 new/delete。
- 对长度、偏移和容量运算检查溢出，再进行分配或拷贝。
- 使用 span、容器和迭代器表达范围，但仍要保证底层生命周期。
- 不从不可信长度直接分配巨量内存；设置协议级上限。
- 编译阶段开启合理警告，测试阶段使用 ASan、UBSan、静态分析与 fuzzing。

~~~cpp
std::optional<std::string> read_frame(std::span<const std::byte> input) {
    if (input.size() < 4) return std::nullopt;
    const auto length = decode_u32(input.first<4>());
    if (length > kMaxFrame || length > input.size() - 4) return std::nullopt;
    return decode_text(input.subspan(4, length));
}
~~~

示例先比较上限与剩余长度，再切片；真实协议还要处理字节序、编码、认证和增量读取。

# 注入与危险能力

参数化 SQL 防止输入改变 SQL 结构，但不能替代授权。调用 Shell 时优先使用参数数组和专用 API。文件路径要规范化并限制在允许根目录内，防止路径穿越与符号链接绕过。

反序列化、正则、压缩包和图片解码都可能触发高 CPU、深递归或超大展开；安全边界包含资源预算，而不只是内容正确性。

# 解析器与资源消耗防线

解析分两步：先限制字节数、嵌套深度、字段数和解压后大小，再解释业务语义。整数计算按“校验上限 → 检查加乘溢出 → 分配 → 拷贝”执行；仅把长度改成无符号类型仍可能回绕。压缩包、正则、图片和递归格式还要限制 CPU、墙钟时间和展开比例。

模糊测试以协议不变量为 oracle：任意输入不得崩溃、越界或无限运行；成功解析后重新编码应保持语义；最小失败样本进入回归集。C++ 分别运行 ASan/UBSan、TSan 与静态分析，release 配置也要测试，不能把 sanitizer 无报告当作安全证明。

跨语言 FFI 是额外边界：明确 buffer 所有权、长度、编码、异常/错误码和回调生命周期。Go/Python 的内存安全不能保护错误的 C ABI。

参考：[SEI CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682)、[LLVM LibFuzzer](https://llvm.org/docs/LibFuzzer.html)。

# 失败与验证

安全失败应返回稳定错误码，日志保存 request ID 和分类，但不回显密钥、栈地址或内部 SQL。对解析器与协议边界编写属性测试或 fuzz target，覆盖空输入、截断、超长、重复字段和随机字节。

> [!question]- 理解检查
> 为什么“长度字段是无符号整数”仍不能避免溢出与越界？说明校验、计算、分配和拷贝的顺序。

> [!info]- 官方参考
> - [OWASP Top 10:2025](https://top10.owasp.org/2025/)
