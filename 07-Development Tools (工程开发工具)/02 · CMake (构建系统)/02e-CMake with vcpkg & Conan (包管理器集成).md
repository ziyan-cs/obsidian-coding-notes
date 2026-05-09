## 为什么需要包管理器

手动管理 C++ 依赖的痛点：

- 不同平台安装路径不同
- 版本冲突难以解决
- 源码构建耗时且繁琐

---

## vcpkg（Microsoft，与 CMake 深度集成）

```bash
# 安装 vcpkg
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh

# 安装库
./vcpkg/vcpkg install fmt spdlog openssl

# 配置 CMake 时传入 toolchain file
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

之后 CMakeLists.txt 中正常使用 `find_package`，vcpkg 自动接管：

```cmake
find_package(fmt CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE fmt::fmt)
```

**Manifest 模式**（推荐，版本锁定）：在项目根目录创建 `vcpkg.json`：

```json
{
  "name": "my-project",
  "version": "1.0.0",
  "dependencies": [
    "fmt",
    { "name": "boost-filesystem", "version>=": "1.83.0" }
  ]
}
```

---

## Conan（跨平台，配置灵活）

```bash
pip install conan
conan profile detect   # 检测当前编译器环境
```

创建 `conanfile.txt`：

```ini
[requires]
fmt/10.2.1
spdlog/1.13.0

[generators]
CMakeDeps
CMakeToolchain
```

```bash
mkdir build && cd build
conan install .. --output-folder=. --build=missing
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .
```

CMakeLists.txt 中：

```cmake
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(myapp PRIVATE fmt::fmt spdlog::spdlog)
```

---

## vcpkg vs Conan 对比

| |vcpkg|Conan|
|---|---|---|
|维护方|Microsoft|JFrog / 社区|
|CMake 集成|极深（toolchain file 即可）|需要 generator 文件|
|二进制缓存|支持（GitHub Actions 缓存）|支持（Conan Center）|
|配置灵活性|一般|高（支持不同 ABI、编译选项）|
|库数量|2000+|1800+|
|适合场景|Windows/跨平台、CMake 项目|企业级、多平台、精细控制|