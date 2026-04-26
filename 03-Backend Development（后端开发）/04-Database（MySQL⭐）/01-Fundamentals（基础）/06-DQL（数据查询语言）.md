
# 数据查询

```sql
# 查：查询数据
select * from [表名];                   -- 查询所有
select [字段1],[字段2] from [表名];      -- 查询指定字段
select [字段1] as [别名] from [表名];    -- 查询返回别名
select [字段1 + 字段2] from [表名];      -- 查询字段运算结果

select distinct * from [表名];          -- 去重查询
select * from [表名] where [条件];       -- 条件查询
select * from [表名] order by [字段] asc;-- 排序（asc升序/desc降序）


```