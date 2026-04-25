

# 1. MySQL 数据类型（列类型）

### 数值类型

- 整数类型

```sql
|  类型        | 占用空间 |  无符号范围    |  有符号范围       |
|-------------|---------|---------------|-----------------|
| Bit(M)      | M 位     | 0 ~ 2^M-1    | -2^(M-1) ~ 2^(M-1)-1 |
| TINYINT     | 1 字节   | 0 ~ 255      | -128 ~ 127           |
| SMALLINT    | 2 字节   | 0 ~ 65535    | -32768 ~ 32767       |
| MEDIUMINT   | 3 字节   | 0 ~ 2²⁴-1    | -2²³ ~ 2²³-1         |
| INT/INTEGER | 4 字节   | 0 ~ 2³²-1    | -2³¹ ~ 2³¹-1         |
| BIGINT      | 8 字节   | 0 ~ 2⁶⁴-1    | -2⁶³ ~ 2⁶³-1         |

> 无符号 (UNSIGNED) 可以扩大正数范围, 但不能存储负数
> 主键、ID、普通计数优先用 INT, 超大场景用 BIGINT, 状态/性别用 TINYINT
```

- 小数类型

```sql
|  类型         | 占用空间 |  说明  |
|--------------|---------|--------|
| FLOAT        | 4 字节   | 单精度浮点数 |
| DOUBLE       | 8 字节   | 双精度浮点数 |
| DECIMAL(M,D) | 可变     | 定点数，M=总长度，D=小数位数|
 
> 涉及金额、价格、精度敏感的数据, 必须使用 DECIMAL, 避免浮点数精度丢失
> DECIMAL M省略默认10, D省略默认0 (M最大63, D最大20)
```

### 字符串类型

```SQL
|  类型       |  最大长度       |  说明  |
|------------|----------------|--------|
| CHAR(M)    | 0 ~ 255 (字符)  | 固定长度字符串 |
| VARCHAR(M) | 0 ~ 65535      | 可变长度字符串 |
| TEXT       | 0 ~ 65535      | 大文本 |
| LONGTEXT   | 0 ~ 2³²-1      | 超大文本 |
| BLOB       | 0 ~ 65535      | 二进制数据 |
| LONGBLOB   | 0 ~ 2³²-1      | 超大二进制数据 |

> VARCHAR 会额外占用 1~2 字节记录字段长度, 超过 255 字符优先用 TEXT
```

### 日期时间类型

```sql
|  类型       |  格式               |  说明  |
|------------|---------------------|--------|
| DATE       | YYYY-MM-DD          | 仅日期  |
| TIME       | HH:MM:SS            | 仅时间  |
| DATETIME   | YYYY-MM-DD HH:MM:SS | 日期+时间 |
| TIMESTAMP  | YYYY-MM-DD HH:MM:SS | 时间戳,自动更新 |
| YEAR       | YYYY                | 年份 |
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






















