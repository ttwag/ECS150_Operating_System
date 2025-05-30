// uthread.h - What applications will use
#include <stdbool.h>

typedef void (*func_t)(void *);
typedef int uthread_t;

int uthread_run(bool preempt, func_t func, void *arg);
uthread_t uthread_create(func_t func, void *arg);
void uthread_yield(void);
void uthread_exit(void);
uthread_t uthread_self(void);