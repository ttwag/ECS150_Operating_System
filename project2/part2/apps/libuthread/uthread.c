#include <stdlib.h>
#include <uthread.h>
#include <private.h>
#include <queue.h>

// Thread states
typedef enum {
    UTHREAD_READY,
    UTHREAD_RUNNING,
    UTHREAD_BLOCKED,
    UTHREAD_ZOMBIE
} uthread_state_t;

typedef struct uthread_tcb {
    uthread_t id;
    uthread_state_t state;
    ucontext_t *context;
    void *stack_ptr;
} uthread_tcb;

typedef struct uthread_scheduler {
    queue_t qu;
    uthread_tcb *idle_thread;
    uthread_t thread_count;
} uthread_scheduler;

// global thread scheduler
static uthread_scheduler *scheduler;

int uthread_run(bool preempt, void (*func)(void *), void *arg) {
    // initialize a scheduler with the current thread as idle thread
    scheduler = (uthread_scheduler *)calloc(1, sizeof(scheduler));
    if (!scheduler) return -1;
    scheduler->idle_thread = (uthread_tcb *)calloc(1, sizeof(uthread_tcb));
    if (!scheduler->idle_thread) {
        uthread_scheduler_destroy(scheduler);
        return -1;
    }
    
    scheduler->qu = queue_create();
    scheduler->idle_thread->state = 
    scheduler->thread_count = 1;

    // create an initial thread and schedule it
    uthread_t initial_thread = uthread_create(func, arg);
    if (initial_thread == -1) {
        uthread_scheduler_destroy(scheduler);
    }
    queue_enqueue(scheduler, );

    // idle loop until no more thread
    

}

uthread_t uthread_create(void (*func)(void *), void *arg) {
    // create a new thread and add it to the scheduler

}

void uthread_yield(void) {
    // switch back to the idle thread
}

void uthread_exit(void);

uthread_t uthread_self(void);


// return: 0 if success, -1 otherwise
static int uthread_tcb_init(uthread_scheduler *scheduler, uthread_state_t state, void *stack_ptr) {
    // uthread_t id;
    uthread_tcb *thread = (uthread_tcb *)calloc(1, sizeof(uthread_tcb));
    if (!thread) return -1;
    
    // finish struct
    scheduler->idle_thread = thread;

    
    ucontext_t *context;
    
}

static void uthread_tcb_destroy(uthread_tcb *thread) {
    if (thread) {
        if (thread->context) free(thread->context);
        uthread_ctx_destroy_stack(thread->stack_ptr);
    }
    free(thread);
}

static void uthread_scheduler_destroy(uthread_scheduler *scheduler) {
    if (scheduler) {
        queue_destroy(scheduler->qu);
        free_uthread_tcb(scheduler->idle_thread);
    }
    free(scheduler);
}
