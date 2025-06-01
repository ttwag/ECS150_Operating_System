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
    void (*usr_func)(void *);
    void *usr_arg;
} uthread_tcb;

typedef struct uthread_scheduler {
    queue_t qu;
    uthread_tcb *idle_thread; //thread id is 0
    uthread_tcb *curr_thread;
    uthread_t thread_cnt;
} uthread_scheduler;

// global thread scheduler
static uthread_scheduler *scheduler;

// static helper declaration
static void uthread_scheduler_destroy(uthread_scheduler *scheduler);
static uthread_tcb *uthread_tcb_create(bool new_context, uthread_scheduler *scheduler, void (*func)(void *), void *arg);
static void uthread_tcb_destroy(uthread_scheduler *scheduler, uthread_tcb *thread);

int uthread_run(bool preempt, void (*func)(void *), void *arg) {
    // initialize a scheduler
    scheduler = (uthread_scheduler *)calloc(1, sizeof(scheduler));
    if (!scheduler) return -1;
    
    // make current thread as the idle thread
    scheduler->idle_thread = uthread_tcb_create(false, scheduler, func, arg);
    scheduler->curr_thread = scheduler->idle_thread;
    if (!scheduler->idle_thread) {
        uthread_scheduler_destroy(scheduler);
        return -1;
    }

    // create and schedule the initial thread
    uthread_t initial_thread = uthread_create(func, arg);
    if (initial_thread == -1) {
        // if creation failed, clean up resources
        uthread_tcb_destroy(scheduler, scheduler->idle_thread);
        uthread_scheduler_destroy(scheduler);
        return -1;
    }

    // idle loop until no more thread
    while (scheduler->thread_cnt > 0) {
        // pop queue to get curr thread
        scheduler->curr_thread = qu
        // push curr thread back to queue

        // run curr thread

    }
    return 0;
}

uthread_t uthread_create(void (*func)(void *), void *arg) {
    // create a new thread and add it to the scheduler
    uthread_tcb *thread = uthread_tcb_create(true, scheduler, func, arg);
    if (!thread) return -1;
    
    // push the tcb to queue
    if (queue_enqueue(scheduler->qu, thread) == -1) {
        // clean up allocated resources if failed to push to queue
        uthread_tcb_destroy(scheduler, thread);
        return -1;
    }
    return thread->id;
}

void uthread_yield(void) {
    // switch back to the idle thread
}

void uthread_exit(void);

uthread_t uthread_self(void) {
    if (!scheduler || !scheduler->curr_thread) return -1;
    return scheduler->curr_thread->id;
}

// static helper definition

static void uthread_func_call(void) {
    if (scheduler->curr_thread) {
        scheduler->curr_thread->
    }
}

static void uthread_scheduler_destroy(uthread_scheduler *scheduler) {
    if (scheduler) {
        queue_destroy(scheduler->qu);
        uthread_tcb_destroy(scheduler, scheduler->idle_thread);
    }
    free(scheduler);
}

static void uthread_tcb_destroy(uthread_scheduler *scheduler, uthread_tcb *thread) {    
    if (thread) {
        uthread_ctx_destroy_stack(thread->stack_ptr);
        if (thread->context) free(thread->context);
        scheduler->thread_cnt--;
    }
    free(thread);
}

// return: pointer to memory if success, NULL otherwise
static uthread_tcb *uthread_tcb_create(bool new_context, uthread_scheduler *scheduler, void (*func)(void *), void *arg) {
    // uthread_t id;
    uthread_tcb *thread = (uthread_tcb *)calloc(1, sizeof(uthread_tcb));
    if (!thread) return NULL;
    
    // finish struct
    thread->id = scheduler->thread_cnt;
    thread->state = UTHREAD_READY;
    scheduler->thread_cnt++;
    scheduler->idle_thread = thread;
    
    if (new_context) {
        ucontext_t *context = (ucontext_t *)calloc(1, sizeof(ucontext_t));
        if (!context) {
            // free the partially created thread
            uthread_tcb_destroy(scheduler, thread);
            return NULL;
        }

        void *stack_ptr = NULL;
        uthread_ctx_alloc_stack(&stack_ptr);
        uthread_ctx_init(context, stack_ptr, func, arg);
        thread->context = context;
    }
    return thread;
}


