#include "include/types.h"
#include "include/param.h"
#include "include/memlayout.h"
#include "include/riscv.h"
#include "include/spinlock.h"
#include "include/proc.h"
#include "include/sched.h"

struct proc *pick_next_proc(void);

static void
mfq_init(void)
{
    // TODO: 初始化四级就绪队列、统计变量
}

static void
mfq_enqueue(struct proc *p)
{
    // TODO: 按优先级/时间片状态加入对应级别的队列
}

static void
mfq_dequeue(struct proc *p)
{
    // TODO: 从对应队列中移除进程
}

static void
mfq_time_tick(void)
{
    sched_tick();
}

// MFQ 调度器实例（全局只读）
const struct sched_ops mfq_scheduler = {
    .init = mfq_init,
    .pick_next = pick_next_proc,
    .enqueue = mfq_enqueue,
    .dequeue = mfq_dequeue,
    .time_tick = mfq_time_tick,
};
