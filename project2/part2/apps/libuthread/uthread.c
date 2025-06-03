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
static void uthread_func_wrap(void *arg);
static void uthread_scheduler_destroy(uthread_scheduler *scheduler);
static uthread_tcb *uthread_tcb_create(bool new_context, uthread_scheduler *scheduler, void (*func)(void *), void *arg);
static void uthread_tcb_destroy(uthread_scheduler *scheduler, uthread_tcb *thread);

int uthread_run(bool preempt, void (*func)(void *), void *arg) {
    if (!func) return -1;

    // initialize a scheduler
    scheduler = (uthread_scheduler *)calloc(1, sizeof(scheduler));
    if (!scheduler) return -1;
    
    scheduler->qu = queue_create();
    if (!scheduler->qu) {
        uthread_scheduler_destroy(scheduler);
        return -1;
    }

    // make current thread as the idle thread
    scheduler->idle_thread = uthread_tcb_create(false, scheduler, func, arg);
    if (!scheduler->idle_thread) {
        uthread_scheduler_destroy(scheduler);
        return -1;
    }
    scheduler->curr_thread = scheduler->idle_thread;

    // create and schedule the initial thread
    uthread_t initial_thread = uthread_create(func, arg);
    if (initial_thread == -1) {
        // if creation failed, clean up resources
        uthread_tcb_destroy(scheduler, scheduler->idle_thread);
        uthread_scheduler_destroy(scheduler);
        return -1;
    }

    // idle loop until no more thread
    while (scheduler->thread_cnt > 1) {
        // pop queue to get next thread
        queue_dequeue(scheduler->qu, (void **)&(scheduler->curr_thread));
        
        // switch to next thread
        scheduler->idle_thread->state = UTHREAD_BLOCKED;
        scheduler->curr_thread->state = UTHREAD_RUNNING;
        uthread_ctx_switch(scheduler->idle_thread->context, scheduler->curr_thread->context);

        // switched back from next thread
        if (scheduler->curr_thread->state == UTHREAD_ZOMBIE) {
            // destroy thread
            uthread_tcb_destroy(scheduler, scheduler->curr_thread);
        }
        else if (scheduler->curr_thread->state == UTHREAD_BLOCKED) {
            // push next thread back to queue
            if (queue_enqueue(scheduler->qu, scheduler->curr_thread) == -1) {
                uthread_tcb_destroy(scheduler, scheduler->curr_thread);
            }
        }
        scheduler->curr_thread = scheduler->idle_thread;
    }

    // free resources
    uthread_scheduler_destroy(scheduler);
    return 0;
}

uthread_t uthread_create(void (*func)(void *), void *arg) {
    // create a new thread and add it to the scheduler
    if (!func) return -1;
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
    if (!scheduler || !scheduler->curr_thread || !scheduler->idle_thread) return;
    
    // switch back to the idle thread
    scheduler->curr_thread->state = UTHREAD_BLOCKED;
    uthread_ctx_switch(scheduler->curr_thread->context, scheduler->idle_thread->context);
}

void uthread_exit(void) {
    if (!scheduler || !scheduler->curr_thread || !scheduler->idle_thread) return;
    
    // mark current thread to be deleted
    scheduler->curr_thread->state = UTHREAD_ZOMBIE;
    uthread_ctx_switch(scheduler->curr_thread->context, scheduler->idle_thread->context);
}

uthread_t uthread_self(void) {
    if (!scheduler || !scheduler->curr_thread) return -1;
    return scheduler->curr_thread->id;
}

// static helper definition

static void uthread_func_wrap(void *arg) {
    if (!scheduler) return;

    if (scheduler->curr_thread) {
        scheduler->curr_thread->usr_func(scheduler->curr_thread->usr_arg);
    }
    uthread_exit();
}

static void uthread_scheduler_destroy(uthread_scheduler *scheduler) {
    if (scheduler) {
        queue_destroy(scheduler->qu);
        if (scheduler->curr_thread == scheduler->idle_thread) {
            uthread_tcb_destroy(scheduler, scheduler->curr_thread);
        }
        uthread_tcb_destroy(scheduler, scheduler->idle_thread);
    }
    free(scheduler);
}

static void uthread_tcb_destroy(uthread_scheduler *scheduler, uthread_tcb *thread) {    
    if (thread) {
        // delete from queue
        queue_delete(scheduler->qu, thread);

        // deallocate the memory
        uthread_ctx_destroy_stack(thread->stack_ptr);
        if (thread->context) free(thread->context);
        scheduler->thread_cnt--;
    }
    free(thread);
}

// return: pointer to memory if success, NULL otherwise
static uthread_tcb *uthread_tcb_create(bool new_context, uthread_scheduler *scheduler, void (*usr_func)(void *), void *usr_arg) {
    // uthread_t id;
    uthread_tcb *thread = (uthread_tcb *)calloc(1, sizeof(uthread_tcb));
    if (!thread) return NULL;
    
    // finish struct
    thread->id = scheduler->thread_cnt;
    thread->state = UTHREAD_READY;
    thread->usr_func = usr_func;
    thread->usr_arg = usr_arg;
    scheduler->thread_cnt++;
    
    if (new_context) {
        ucontext_t *context = (ucontext_t *)calloc(1, sizeof(ucontext_t));
        if (!context) {
            // free the partially created thread
            uthread_tcb_destroy(scheduler, thread);
            return NULL;
        }

        void *stack_ptr = NULL;
        uthread_ctx_alloc_stack(&stack_ptr);
        uthread_ctx_init(context, stack_ptr, uthread_func_wrap, NULL);
        thread->context = context;
        thread->stack_ptr = stack_ptr;
    }
    return thread;
}


