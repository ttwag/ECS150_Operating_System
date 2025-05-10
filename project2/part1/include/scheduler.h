#ifndef SCHEDULER_H
#define SCHEDULER_H

int scheduler_run(void (*task_func)(int),
                  int task_cnt,
                  const int *task_sched);
void scheduler_yield(int task_id);

#endif