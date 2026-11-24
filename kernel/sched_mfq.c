#include "include/types.h"
#include "include/param.h"
#include "include/memlayout.h"
#include "include/riscv.h"
#include "include/spinlock.h"
#include "include/proc.h"
#include "include/sched.h"

// TODO: 后续定义多级就绪队列结构体、Aging阈值、时间片参数

// ========== 新增：声明外部全局进程表数组 ==========
extern struct proc proc[NPROC];
// ================================================

static void
mfq_init(void)
{
    // TODO: 初始化四级就绪队列、统计变量
}

static struct proc*
mfq_pick_next(void)
{
    // TODO: 从高到低遍历多级队列，选第一个可运行进程
    // 临时占位：复用RR逻辑保证编译通过，后续替换
    struct proc *p;
    for(p = proc; p < &proc[NPROC]; p++) {
        if(p->state == RUNNABLE)
            return p;
    }
    return 0;
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
    // TODO: 当前队列时间片递减，耗尽则降级并触发调度
}

// MFQ 调度器实例（全局只读）
const struct sched_ops mfq_scheduler = {
    .init = mfq_init,
    .pick_next = mfq_pick_next,
    .enqueue = mfq_enqueue,
    .dequeue = mfq_dequeue,
    .time_tick = mfq_time_tick,
};
