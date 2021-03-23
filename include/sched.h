#pragma once

#include "common.h"

#define THREAD_CPU_CONTEXT      0

#ifndef __ASSEMBLER__

#define THREAD_SIZE             4096
#define NR_TASKS                64

#define FIRST_TASK  task[0]
#define LAST_TASK   task[NR_TASKS-1]

#define TASK_RUNNING            0

extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];
extern int nr_tasks;


// Processes & Tasks should only use registers x19-x30 for persistency
struct cpu_context {
    u64 x19;
    u64 x20;
    u64 x21;
    u64 x22;
    u64 x23;
    u64 x24;
    u64 x25;
    u64 x26;
    u64 x27;
    u64 x28;
    u64 fp;
    u64 sp;
    u64 pc;
};

struct task_struct {
    struct cpu_context cpu_context;
    u64 state;
    u64 counter;
    u64 priority;
    u64 preempt_count;
    u64 id; // starts with 1
};

extern void sched_init(void);
extern void schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct *next);
extern void cpu_switch_to(struct task_struct *prev, struct task_struct *next);

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state, etc*/ 0,0,1,0,1 \
}

#endif

