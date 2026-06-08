---
tags:
  - distributed/nginx
status: 🌱
---

> **核心考点**：反向代理配置、负载均衡策略、location 匹配规则、动静分离、HTTPS 配置

## 反向代理配置

```nginx
server {
    listen 80;
    server_name api.example.com;

    location / {
        proxy_pass http://backend_server;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        proxy_connect_timeout 60s;
        proxy_read_timeout 60s;
        proxy_send_timeout 60s;
    }
}
```

---

## 负载均衡策略

```nginx
upstream backend_server {
    # 1. 轮询（默认）
    server 192.168.1.1:8080;
    server 192.168.1.2:8080;

    # 2. 权重
    server 192.168.1.1:8080 weight=3;
    server 192.168.1.2:8080 weight=1;

    # 3. IP 哈希（保持会话）
    ip_hash;

    # 4. 最少连接
    least_conn;
}
```

| 策略 | 说明 | 适用场景 |
|------|------|---------|
| 轮询（RR） | 默认，按顺序分发 | 无状态服务 |
| 加权轮询 | 指定权重比 | 后端性能不同 |
| IP Hash | 同一 IP 始终到同一后端 | 需要 session 保持 |
| Least Connections | 选连接数最少的 | 请求处理时间不均 |

---

## location 匹配规则

```
优先级从高到低：
1. 精确匹配（=）：location = /login
2. 前缀匹配（^~）：location ^~ /static/
3. 正则匹配（~ 大小写敏感，~* 不敏感）
4. 普通前缀匹配：location /api/
5. 默认 /：兜底

匹配规则：
- 先检查普通前缀匹配，记录最长匹配
- 如果有 ^~，直接使用该匹配
- 否则检查正则匹配（按配置顺序），第一个匹配的正则生效
- 如果没正则匹配，使用最长前缀匹配
```

---

## 动静分离

```nginx
server {
    # 静态资源 → Nginx 直接返回
    location ~* \.(css|js|jpg|png|gif|ico|svg)$ {
        root /var/www/static;
        expires 30d;
        add_header Cache-Control "public, immutable";
        access_log off;
    }

    # 动态请求 → 代理到后端
    location /api/ {
        proxy_pass http://backend_servers;
    }

    # SPA 入口
    location / {
        root /var/www/dist;
        index index.html;
        try_files $uri $uri/ /index.html;
    }
}
```

---

## HTTPS 配置

```nginx
server {
    listen 443 ssl http2;
    server_name example.com;

    ssl_certificate     /etc/nginx/certs/fullchain.pem;
    ssl_certificate_key /etc/nginx/certs/privkey.pem;

    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;
    ssl_session_cache shared:SSL:10m;
    ssl_session_timeout 10m;
}
server {
    listen 80;
    server_name example.com;
    return 301 https://$server_name$request_uri;
}
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| location 匹配顺序 | 精确 > ^~ 前缀 > 正则 > 普通前缀 > / |
| proxy_pass 有无斜杠的区别 | 有斜杠去掉匹配路径，无斜杠全路径传递 |
| upstream 健康检查 | max_fails=3 fail_timeout=30s 自动踢出故障节点 |
| 反向代理 vs 正向代理 | 正向代理代理客户端，反向代理代理服务端 |
| X-Forwarded-For | 传递客户端真实 IP |

> **工程要点**：动静分离可显著提升性能。HTTPS 建议 Let's Encrypt 自动续期。调试用 `nginx -T` 查看合并配置。

---

Nginx 架构与实践详解见 → [Nginx Architecture：Master & Worker Process (架构模型)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/02%20·%20Nginx%20(反向代理与负载均衡)/02a-Nginx%20Architecture：Master%20&%20Worker%20Process%20(架构模型).md) · [Nginx vs webserver：Why Use Both (与自写server的关系)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/02%20·%20Nginx%20(反向代理与负载均衡)/02c-Nginx%20vs%20webserver：Why%20Use%20Both%20(与自写server的关系).md)
