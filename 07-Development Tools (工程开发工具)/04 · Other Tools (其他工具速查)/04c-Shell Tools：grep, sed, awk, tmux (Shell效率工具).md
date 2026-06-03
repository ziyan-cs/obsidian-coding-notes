
> **核心考点**：各工具的核心用法、组合管道使用、tmux 会话管理

## grep（文本搜索）

```bash
grep "pattern" file.txt              # 基础搜索
grep -r "pattern" ./src/             # 递归搜索目录
grep -i "pattern" file.txt           # 忽略大小写
grep -n "pattern" file.txt           # 显示行号
grep -v "pattern" file.txt           # 反向匹配（不含 pattern 的行）
grep -c "pattern" file.txt           # 只显示匹配行数
grep -l "pattern" *.cpp              # 只显示匹配的文件名
grep -A 3 "pattern" file.txt         # 显示匹配行及后 3 行（After）
grep -B 3 "pattern" file.txt         # 显示匹配行及前 3 行（Before）
grep -C 3 "pattern" file.txt         # 前后各 3 行（Context）

grep -E "error|warn" log.txt         # 扩展正则（等同 egrep）
grep -P "\d{3}-\d{4}" file.txt       # Perl 正则（支持 \d \s 等）
grep --include="*.cpp" -r "TODO" .   # 只搜索指定扩展名

# 实用组合
grep -rn "FIXME" . | wc -l           # 统计 TODO 数量
ps aux | grep nginx | grep -v grep   # 查找进程，排除 grep 自身
```

---

## sed（流编辑器）

```bash
sed 's/old/new/' file.txt            # 替换每行第一个匹配
sed 's/old/new/g' file.txt           # 替换所有匹配（global）
sed 's/old/new/gi' file.txt          # 忽略大小写替换
sed -i 's/old/new/g' file.txt        # 原地修改文件（-i）
sed -i.bak 's/old/new/g' file.txt    # 原地修改并备份为 .bak

sed -n '5,10p' file.txt              # 只打印第 5~10 行（-n 抑制默认输出）
sed '3d' file.txt                    # 删除第 3 行
sed '/pattern/d' file.txt            # 删除匹配行
sed '/^$/d' file.txt                 # 删除空行
sed 's/^/    /' file.txt             # 每行开头加 4 个空格
sed 's/[[:space:]]*$//' file.txt     # 删除行尾空白

# 多命令
sed -e 's/foo/bar/g' -e 's/baz/qux/g' file.txt
sed '/START/,/END/d' file.txt        # 删除 START 到 END 之间的行
```

---

## awk（文本处理与报表）

awk 按行处理文本，每行按分隔符切割成字段（`$1`, `$2`, ...，`$0` 是整行）：

```bash
awk '{print $1}' file.txt            # 打印第一列
awk '{print $1, $3}' file.txt        # 打印第 1、3 列
awk -F: '{print $1}' /etc/passwd     # 以 : 为分隔符，打印用户名
awk 'NR==5' file.txt                 # 打印第 5 行（NR = 行号）
awk 'NR>=5 && NR<=10' file.txt       # 打印第 5~10 行

awk '/pattern/ {print $0}' file.txt  # 打印匹配行
awk '$3 > 100 {print $1, $3}' file.txt  # 条件过滤

# 统计求和
awk '{sum += $1} END {print sum}' file.txt

# 统计字段出现次数
awk '{count[$1]++} END {for (k in count) print k, count[k]}' file.txt

# BEGIN 和 END 块
awk 'BEGIN {print "开始"} {print NR, $0} END {print "共", NR, "行"}' file.txt

# 处理 CSV
awk -F',' '{print $2}' data.csv

# 实用案例：查看进程内存占用前5（RSS 列）
ps aux | awk 'NR>1 {print $6, $11}' | sort -rn | head -5
```

---

## tmux（终端复用）

tmux 允许在一个终端中管理多个会话、窗口、面板，SSH 断开后会话依然保活。

### 会话管理

```bash
tmux                          # 新建会话
tmux new -s mywork            # 新建命名会话
tmux ls                       # 列出所有会话
tmux attach -t mywork         # 重新连接会话（SSH 断线后恢复）
tmux kill-session -t mywork   # 删除会话
```

### 快捷键（前缀键默认 Ctrl+B，记作  `<prefix>`）

```
# 会话
<prefix> d          → detach（离开会话，后台保活）
<prefix> s          → 列出所有会话并切换
<prefix> $          → 重命名当前会话

# 窗口（Window，类似浏览器标签页）
<prefix> c          → 新建窗口
<prefix> n / p      → 下一个 / 上一个窗口
<prefix> 0~9        → 切换到第 N 个窗口
<prefix> ,          → 重命名当前窗口
<prefix> &          → 关闭当前窗口

# 面板（Pane，窗口内分割）
<prefix> %          → 水平分割（左右）
<prefix> "          → 垂直分割（上下）
<prefix> 方向键      → 切换面板
<prefix> z          → 最大化/恢复当前面板
<prefix> x          → 关闭当前面板
<prefix> {  /  }    → 与相邻面板交换位置

# 其他
<prefix> [          → 进入滚动模式（q 退出）
<prefix> ?          → 查看所有快捷键
```

### ~/.tmux.conf 常用配置

```bash
# 修改前缀键为 Ctrl+a（更顺手）
set -g prefix C-a
unbind C-b
bind C-a send-prefix

# 鼠标支持（可用鼠标点击切换面板/调整大小）
set -g mouse on

# 从 1 开始编号（默认从 0）
set -g base-index 1

# 增大历史行数
set -g history-limit 10000

# 更直观的分割快捷键
bind | split-window -h
bind - split-window -v
```

---

## 常用管道组合

```bash
# 统计代码行数（排除空行和注释）
find . -name "*.cpp" | xargs grep -v "^//" | grep -v "^$" | wc -l

# 实时监控日志中的 ERROR
tail -f app.log | grep --color "ERROR"

# 找出占用端口的进程
ss -tlnp | grep 8080
# 或
lsof -i :8080

# 批量替换文件中的字符串
grep -rl "OldName" ./src | xargs sed -i 's/OldName/NewName/g'

# 统计 Nginx 访问日志中各 IP 的请求次数，取前 10
awk '{print $1}' access.log | sort | uniq -c | sort -rn | head -10

# 查找大文件
du -sh * | sort -rh | head -20

# 监控系统调用（调试神器）
strace -p <pid> -e trace=network    # 只追踪网络相关系统调用
strace ./myapp 2>&1 | grep open     # 追踪文件打开操作
```