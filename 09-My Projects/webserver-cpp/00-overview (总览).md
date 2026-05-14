

## 开发环境

| 角色      | 设备 / 系统                | 说明          |
| ------- | ---------------------- | ----------- |
| 服务端     | 虚拟机 · Ubuntu 24.04 LTS | 服务器运行环境     |
| 客户端     | 宿主机 · Windows          | Telnet 连接测试 |
| 压测 / 扩展 | 预留（后续接入 NanoPi R4S）    | 多并发连接测试     |

- 网络拓扑：
	- 宿主机与虚拟机通过 VMware 桥接模式处于同一局域网
	- 将 NanoPi R4S 接入同一交换机，用于多并发场景下的压测与验证。

# 演进路线

- [x] v0.1：TCP echo server —— 纯 socket 实现，跑通完整收发流程
- [x] v0.2：引入 epoll，改造为事件驱动 I/O 多路复用模型
- [ ] v0.3：HTTP/1.1 请求解析，响应静态页面
- [ ] v0.4：线程池，实现多线程并发处理请求
- [ ] v0.5：小根堆定时器，处理超时连接
- [ ] v0.6：异步日志系统
- [ ] v0.7：MySQL 连接池，用户登录 / 注册
- [ ] v0.8：性能压测、架构图、README 整理

## 一、Git / 提交规范 必用词

1. **feat** /fiːt/
    
    新增功能、新模块
2. **fix** /fɪks/
    
    修复 bug、修复逻辑
3. **refactor** /ˌriːˈfæktər/
    
    重构：不改功能，只整理代码结构
4. **docs** /dɒks/
    
    文档相关（README、日志）
5. **chore** /tʃɔːr/
    
    杂项：配置、初始化、工程搭建
6. **reword** /riːˈwɜːd/
    
    修改提交信息
7. **merge** /mɜːrdʒ/
    
    合并分支
8. **commit** /kəˈmɪt/
    
    提交版本
9. **push / pull** /pʊʃ//pʊl/
    
    推送 / 拉取代码

## 二、C++ 类 / 项目结构 高频词

1. **wrapper** /ˈræpər/
    
    包装封装类（你 Socket、Epoll 都是）
2. **module** /ˈmɒdjuːl/
    
    模块
3. **config** /kənˈfɪɡ/
    
    配置
4. **logger** /ˈlɒɡər/
    
    日志器
5. **macro** /ˈmækrəʊ/
    
    宏定义
6. **callback** /ˈkɔːlbæk/
    
    回调函数
7. **interface** /ˈɪntəfeɪs/
    
    接口
8. **implement** /ˈɪmplɪment/
    
    实现
9. **logic** /ˈlɒdʒɪk/
    
    业务逻辑、流程逻辑
10. **flow** /fləʊ/
    
    流程

## 三、网络编程（你 WebServer 专用）

1. **server** /ˈsɜːvər/
    
    服务器
2. **client** /ˈklaɪənt/
    
    客户端
3. **socket** /ˈsɒkɪt/
    
    套接字
4. **epoll** /ˈiːpɒl/
    
    IO 多路复用模型
5. **channel** /ˈtʃænl/
    
    事件通道
6. **acceptor** /əkˈseptər/
    
    连接接收器
7. **event** /ɪˈvent/
    
    事件
8. **loop** /luːp/
    
    循环（EventLoop 事件循环）
9. **listen** /ˈlɪsn/
    
    监听端口
10. **connect** /kəˈnekt/
    
    连接

## 四、commit 写描述常用简单词（你直接套用）

1. **add** /æd/
    
    新增
2. **remove** /rɪˈmuːv/
    
    移除
3. **optimize** /ˈɒptɪmaɪz/
    
    优化
4. **simplify** /ˈsɪmplɪfaɪ/
    
    简化
5. **correct** /kəˈrekt/
    
    修正、改正
6. **update** /ˌʌpˈdeɪt/
    
    更新
7. **handle** /ˈhændl/
    
    处理（事件 / 请求）

## 五、工程 / 编译 常用

1. **build** /bɪld/
    
    构建、编译
2. **compile** /kəmˈpaɪl/
    
    编译
3. **structure** /ˈstrʌktʃə/
    
    结构
4. **architecture** /ˈɑːkɪtektʃə/
    
    架构