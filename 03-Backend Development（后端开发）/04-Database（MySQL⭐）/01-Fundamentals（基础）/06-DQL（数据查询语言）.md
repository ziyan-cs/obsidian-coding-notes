
# 1. 数据查询

```sql
# 查：查询数据
select * from [表名];                    -- 查询所有
select [字段1],[字段2] from [表名];      -- 查询指定字段
select * from [表名] where [条件];       -- 条件查询
select * from [表名] order by [字段] asc;-- 排序（asc升序/desc降序）
select * from [表名] limit 10;          -- 限制条数
```