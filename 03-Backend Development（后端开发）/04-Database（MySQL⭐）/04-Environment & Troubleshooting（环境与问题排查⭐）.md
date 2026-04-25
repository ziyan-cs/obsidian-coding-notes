

# 远程连接 MySQL

```sql
# 命令行远程连接 MySQL
mysql -h [主机IP] -P [端口] -u [用户名] -p

# 授权远程登录（在 MySQL 内执行）
grant all privileges on *.* to '用户名'@'%' identified by '密码';
flush privileges;
```
