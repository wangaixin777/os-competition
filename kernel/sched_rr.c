#include "include/types.h"
#include "include/param.h"
#include "include/memlayout.h"
#include "include/riscv.h"
#include "include/spinlock.h"
#include "include/proc.h"
#include "include/sched.h"

// ========== 新增：声明外部全局进程表数组 ==========
extern struct proc proc[NPROC];
// ================================================
/*
 * RR 调度器初始化
 * 原版 RR 无需额外初始化，空实现即可
 */
static void
rr_init(void)
{
    // 基线轮转调度无特殊初始化
}

/*
 * 选出下一个可运行进程
 * 完全复现基线 pick_next_proc 逻辑：遍历进程表，返回第一个 RUNNABLE 进程
 */
static struct proc*
rr_pick_next(void)
{
    struct proc *p;
    for(p = proc; p < &proc[NPROC]; p++) {
        if(p->state == RUNNABLE)
            return p;
    }
    return 0;
}

/*
 * 进程入队
 * 原版 RR 无显式就绪队列，靠 state 字段标记，空实现即可
 */
static void
rr_enqueue(struct proc *p)
{
    // 基线通过 state=RUNNABLE 标记就绪，无需额外队列操作
}

/*
 * 进程出队
 * 同上，空实现
 */
static void
rr_dequeue(struct proc *p)
{
    // 基线通过 state 变更标记离队，无需额外队列操作
}

/*
 * 时钟滴答处理
 * 复现基线逻辑：时钟中断触发 yield 主动让出
 */
static void
rr_time_tick(void)
{
    yield();
}

// RR 调度器实例（全局只读）
const struct sched_ops rr_scheduler = {
    .init = rr_init,
    .pick_next = rr_pick_next,
    .enqueue = rr_enqueue,
    .dequeue = rr_dequeue,
    .time_tick = rr_time_tick,
};
