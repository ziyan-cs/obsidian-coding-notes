

# 1. MySQL 数据类型（列类型）

### 数值类型

- 整数类型

```
TINYINT       1字节    -128~127 / 0~255
SMALLINT      2字节    -32768~32767
MEDIUMINT     3字节
INT / INTEGER 4字节    常用 (主键、ID、数量)
BIGINT        8字节    超大数值
```

- 小数类型

```
FLOAT         单精度浮点数
DOUBLE        双精度浮点数
DECIMAL(M,D)  高精度小数 (金额必须用这个)
```

### 字符串类型

```
CHAR(M)       固定长度字符串（0~255）
VARCHAR(M)    可变长度字符串（姓名、标题、内容）
TEXT          大文本（文章、详情）
LONGTEXT      超大文本
BLOB          二进制数据（图片、文件）
```

### 日期时间类型

```sql
DATE          日期：YYYY-MM-DD
TIME          时间：HH:MM:SS
DATETIME      日期+时间：YYYY-MM-DD HH:MM:SS
TIMESTAMP     时间戳（自动更新时间）
YEAR          年份
```

### 常用类型选择（重点）

```sql
1. ID、序号、数量 → INT
2. 状态、性别、小数字 → TINYINT
3. 金额、价格 → DECIMAL
4. 姓名、标题、内容 → VARCHAR
5. 大段文字 → TEXT
6. 生日、注册日期 → DATE
7. 完整时间记录 → DATETIME
```

### 建表示例（标准写法）

```sql
CREATE TABLE user (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL,
    age TINYINT,
    money DECIMAL(10,2),
    content TEXT,
    create_time DATETIME
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

# 2. 字段约束与主键设计


# 3. 表结构优化与规范






















