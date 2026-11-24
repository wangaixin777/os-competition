## 1. PCB 与就绪进程

进程控制块（PCB）定义在 `kernel/include/proc.h` 的 `struct proc` 中。`state` 记录进程状态，`chan` 记录阻塞时等待的事件通道，`context` 保存进程切换时所需的内核寄存器。每个 CPU 也有自己的调度器上下文 `c->context`，用于在调度器与进程之间切换。

当前基线通过进程表中的 `RUNNABLE` 状态识别就绪进程。`kernel/proc.c` 中的 `pick_next_proc()` 每次从进程表头部扫描，返回第一个处于 `RUNNABLE` 状态的进程。扫描完仍无候选时，返回 `0`。

## 2. 选取进程与上下文切换

`scheduler()` 选出候选进程后，获取 `p->lock` 并复查该进程是否仍处于 `RUNNABLE` 状态。复查通过后，将其标记为 `RUNNING`，更新当前 CPU 的 `c->proc`，切换页表，然后调用 `swtch(&c->context, &p->context)` 进入进程。

进程让出 CPU、阻塞或退出时，通过 `sched()` 切回调度器。调度器从 `swtch()` 后继续执行，恢复内核页表、清空 `c->proc`、释放进程锁，然后开始下一轮调度。

## 3. 时钟中断与抢占

`kernel/trap.c` 中的 `devintr()` 识别到时钟中断后，调用 `timer_tick()` 并返回 `2`。`timer_tick()` 更新 `ticks`，唤醒等待时钟的进程，并设置下一次定时中断。

- **用户态路径。** `usertrap()` 在返回用户态前，若中断类型为时钟中断，就调用 `yield()`。
- **内核态路径。** `kerneltrap()` 在收到时钟中断且当前 CPU 上有处于 `RUNNING` 状态的进程时，调用 `yield()`。

`yield()` 获取当前进程的锁，将其状态改回 `RUNNABLE`，再调用 `sched()` 把 CPU 交还给调度器。

## 4. 阻塞与唤醒

`sleep(chan, lk)` 在持有进程锁时记录等待通道，将状态改为 `SLEEPING`，再调用 `sched()`。事件发生时，`wakeup(chan)` 扫描进程表，将处于 `SLEEPING` 且等待通道匹配的进程改回 `RUNNABLE`。进程再次被调度选中后，从 `sleep()` 中挂起的位置继续执行。

```
RUNNING → sleep(chan, lk) → SLEEPING
        → wakeup(chan) → RUNNABLE → scheduler() 选中 → RUNNING
```

接入 MFQ 就绪队列时，还需覆盖 `userinit()`、`fork()`、`yield()`、`wakeup1()` 和 `kill()` 中转为 `RUNNABLE` 的路径。唤醒晋升按原因处理，定时唤醒、等待子进程退出和 `kill()` 唤醒应与设备 I/O 唤醒区分。

## 5. MFQ 与 Aging 试跑参数

| 队列 | 优先级 | 时间片 |
| `Q0` | 最高 | 1 tick |
| `Q1` | 次高 | 2 ticks |
| `Q2` | 次低 | 4 ticks |
| `Q3` | 最低 | 8 ticks |

**Aging 阈值暂定为 32 ticks。** 仅计算进程处于 `RUNNABLE` 状态、在就绪队列中等待 CPU 的时间。低优先级进程达到阈值后向上晋升一级。

时间片和 Aging 阈值均为联调初值，后续根据 QEMU 试跑结果调整。