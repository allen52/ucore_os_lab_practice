#include <list.h>
#include <sync.h>
#include <proc.h>
#include <sched.h>
#include <assert.h>

void
wakeup_proc(struct proc_struct *proc) {
    assert(proc->state != PROC_ZOMBIE && proc->state != PROC_RUNNABLE);
    proc->state = PROC_RUNNABLE;
}

void
schedule(void) {//调度函数
    bool intr_flag;
    list_entry_t *le, *last;
    struct proc_struct *next = NULL;
    local_intr_save(intr_flag);
    {
        current->need_resched = 0;//
        last = (current == idleproc) ? &proc_list : &(current->list_link);//当前是idle进程 last<=proclist 否则 last<=current的下一个
        le = last;
        do {
            if ((le = list_next(le)) != &proc_list) {
                next = le2proc(le, list_link);
                if (next->state == PROC_RUNNABLE) {
                    break;
                }
            }
        } while (le != last);
        if (next == NULL || next->state != PROC_RUNNABLE) {//若不存在下一个要执行的进程，将next置为idle
            next = idleproc;
        }
        next->runs ++;//运行次数？

        if (next != current) {
            proc_run(next);//运行下一进程
        }
    }
    local_intr_restore(intr_flag);
}

