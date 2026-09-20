---
study_stage: backlog
tags: [ai/inference, backend/performance]
---

> [!abstract] 学习定位
> 推理服务在吞吐、首 token 延迟、逐 token 速度、显存和公平性之间取舍；批处理与缓存必须用真实流量验证。

> [!summary] 核心摘要
>
> 请求经过 admission、排队、batch、prefill、decode 和流式返回。限制上下文、输出和队列，才能避免少数超长请求拖垮全部租户。

# 请求生命周期

~~~text
admission -> queue -> batch -> prefill -> decode -> stream
      |        |        |        |         |
    quota    wait     GPU use   prompt    tokens
~~~

关键指标包括 time to first token、inter-token latency、端到端 P95/P99、吞吐、队列等待、拒绝率和显存占用。

# 批处理

静态 batch 等待凑齐请求，简单但增加延迟；连续批处理在序列完成后动态加入新请求，提高利用率但调度更复杂。超长序列会占用 KV cache 并影响其他请求，需要长度分桶、配额或抢占策略。

# 缓存

结果缓存只适合确定性足够、权限和版本可纳入 key 的请求。prompt/KV cache 可减少重复前缀计算，但消耗显存并涉及租户数据隔离。

缓存 key 至少考虑模型版本、模板、参数、数据权限与工具状态；不能让一个租户命中另一个租户的私有上下文。

# 过载与降级

队列有界，超限时快速拒绝、切换较小模型、缩短可选上下文或异步处理。重试要有随机退避和总预算，防止故障时流量放大。

# 计算与显存约束

LLM 推理通常分为两个性质不同的阶段：prefill 并行处理整个 prompt，矩阵计算密集；decode 每轮为每个序列生成一个 token，需要反复读取权重和 KV cache，更容易受显存带宽与调度影响。只用“单次推理耗时”无法解释两者瓶颈。

单个序列的 KV cache 规模可用下式建立量级直觉：

~~~text
KV bytes ≈ 2 × layers × tokens × kv_heads × head_dim × bytes_per_element
~~~

`2` 表示 key 与 value。实际值还受 GQA/MQA、分页管理、量化、对齐与框架实现影响，因此公式用于容量预估，最终必须看服务端指标。上下文越长，单请求占用越久，可同时容纳的序列越少。

用 Little's Law 建立队列直觉：稳定状态下 `in_flight ≈ arrival_rate × latency`。到达率接近服务能力后，队列等待会非线性增长；此时继续接受请求只会把超时和重试推迟，而不会创造吞吐。

# 调度、公平与准入

准入控制应在占用大量 token 或 GPU 时间之前完成：校验租户配额、prompt/output 上限、deadline 与并发。按长度分桶能减少 padding 和长短请求互相拖累；租户级队列或加权公平调度可避免一个批量任务独占 KV cache。

取消必须贯穿网关、调度器和模型执行层。客户端断开后仍继续 decode 会浪费最昂贵的资源。若底层无法立即抢占，也要停止后续排队和流式发送，并记录被浪费的 token。

一个入门容量实验：固定模型与硬件，用短问短答、长输入短答、短输入长答三组请求分别测单并发与高并发。记录排队时间、TTFT、每输出 token 间隔、完成率与显存峰值；逐步提高 batch 上限，找出吞吐改善而 P95 仍满足交互目标的区间。若只记录总 tokens/s，容易把长输入与长输出的瓶颈混为一谈。

一个入门容量实验：固定模型与硬件，用短问短答、长输入短答、短输入长答三组请求分别测单并发与高并发。记录排队时间、TTFT、每输出 token 间隔、完成率与显存峰值；逐步提高 batch 上限，找出吞吐改善而 P95 仍满足交互目标的区间。若只记录总 tokens/s，容易把长输入与长输出的瓶颈混为一谈。

# 容量实验

使用接近真实流量的 prompt 长度、输出长度和到达间隔分布，而不是只压固定短请求。实验至少扫描并发、batch 上限、队列延迟和上下文长度，并分别记录：

- TTFT、inter-token latency 与端到端 P50/P95/P99；
- input/output tokens/s、请求吞吐、队列时间和拒绝率；
- GPU 利用率、显存/KV cache 占用、抢占或重计算次数；
- 不同租户与长短请求的延迟分布，而不只看总体平均值；
- 冷启动、模型加载、缓存命中和未命中场景。

先定义用户侧 SLO，再选择可接受的 batch 等待和并发上限。若优化只提高总 tokens/s，却让交互请求频繁超时，它不是有效的服务优化。

> [!info]- 官方参考
> - [NVIDIA Triton: Dynamic Batching and Concurrent Model Execution](https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/tutorials/Conceptual_Guide/Part_2-improving_resource_utilization/README.html)

> [!question]- 理解检查
> 为什么扩大 batch 可能提高总吞吐却恶化交互用户体验？应该同时观察哪些指标？
