// private.h - Internal definitions
#define _XOPEN_SOURCE
#include <ucontext.h>
#include "uthread.h"

// Just declare the context functions you'll use
int uthread_ctx_alloc_stack(void **stack_ptr);
void uthread_ctx_destroy_stack(void *stack_ptr);
int uthread_ctx_init(ucontext_t *ctx, void *stack_ptr, func_t func, void *arg);
int uthread_ctx_switch(ucontext_t *prev, ucontext_t *next);

// Thread states
typedef enum {
    UTHREAD_READY,
    UTHREAD_RUNNING,
    UTHREAD_BLOCKED,
    UTHREAD_ZOMBIE
} uthread_state_t;