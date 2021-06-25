/*
* All the scheduling class methods:
*/
const struct sched_class fair_sched_class = {
.next = &idle_sched_class,
// 一个task变为可运行状态，将之添加到运行队列中
.enqueue_task = enqueue_task_fair,
// 执行enqueue_task的逆操作
.dequeue_task = dequeue_task_fair,
// 一个task自愿放弃CPU，但仍为可运行状态
.yield_task = yield_task_fair,
.yield_to_task = yield_to_task_fair,
// 检查一个新成为可运行态的task是否应该抢占当前运行的进程
.check_preempt_curr = check_preempt_wakeup,
// 挑选下一个占据CPU的进程，由schedule()调用
.pick_next_task = pick_next_task_fair,
.put_prev_task = put_prev_task_fair,
#ifdef CONFIG_SMP
.select_task_rq = select_task_rq_fair,
.migrate_task_rq = migrate_task_rq_fair,
.rq_online = rq_online_fair,
.rq_offline = rq_offline_fair,
.task_dead = task_dead_fair,
.set_cpus_allowed = set_cpus_allowed_common,
#endif
// task改变调度策略
.set_curr_task = set_curr_task_fair,
// 每次时钟中断时，由scheduler_tick()调用
.task_tick = task_tick_fair,
// 一个新的task被创建
.task_fork = task_fork_fair,
/* ... */
};