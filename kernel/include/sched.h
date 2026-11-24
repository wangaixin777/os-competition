#ifndef SCHED_H
#define SCHED_H

#include "proc.h"
#include "types.h"   // 新增：先加载 uint64 等基础类型定义
#include "proc.h"    // 同目录直接写文件名，不要加 include/ 前缀

// 调度器模式编号
#define SCHED_MODE_RR   0   // 轮转调度（基线原版）
#define SCHED_MODE_MFQ  1   // 多级反馈队列（新增）

// 默认调度模式：默认 RR，保证基线兼容
#define DEFAULT_SCHED_MODE SCHED_MODE_RR

/*
 * 调度器统一操作接口
 * 所有调度算法（RR / MFQ）都实现这套函数，上层统一调用
 * 对应实验指导书「机制与策略分离」：上层机制不变，下层策略可替换
 */
struct sched_ops {
    // 初始化调度器（系统启动时调用一次）
    void (*init)(void);

    // 策略核心：选出下一个可运行的进程（对应原 pick_next_proc）
    struct proc* (*pick_next)(void);

    // 进程加入就绪队列
    void (*enqueue)(struct proc *p);

    // 进程脱离就绪队列
    void (*dequeue)(struct proc *p);

    // 每个时钟滴答的处理（时间片递减、调度触发）
    void (*time_tick)(void);
};

// 全局当前调度器实例
extern const struct sched_ops *current_scheduler;

// 调度器初始化入口
void scheduler_init(void);

#endif
