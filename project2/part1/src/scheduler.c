#define _XOPEN_SOURCE
#include <ucontext.h>
#include <stdlib.h>

#define STACK_SIZE 4096
#define MAX_THREADS 100

typedef struct {
    int task_id;
    ucontext_t context;
    char *stack;
} Thread;

static ucontext_t main_context;
static Thread *usrThreads[MAX_THREADS];

// free the Threads in usrThreads and their stacks
void freeThreads() {
    // free Threads
    for (int i = 0; i < MAX_THREADS; i++) {
        if (usrThreads[i] != NULL) {
            if (usrThreads[i]->stack != NULL) {
                // free stack in usrThreads
                free(usrThreads[i]->stack);
                usrThreads[i]->stack = NULL;
            }
            free(usrThreads[i]);
            usrThreads[i] = NULL;
        }
    }
}

/*
returns 1 if schedule does not work
*/
int scheduler_run(void (*task_func)(int),
                  int task_cnt,
                  const int *task_sched)
{
    // check if the thread is within limit
    if (task_cnt > MAX_THREADS) return 1;

    // Loop through task_sched task_cnt times to allocate thread's stack from heap
    for (int id = 0; id < task_cnt; id++) {
        // initialize the Thread object
        usrThreads[id] = (Thread *)calloc(1, sizeof(Thread));
        usrThreads[id]->task_id = id;
        usrThreads[id]->stack = (char *)calloc(STACK_SIZE, sizeof(char));
        getcontext(&(usrThreads[id]->context));
        usrThreads[id]->context.uc_stack.ss_sp = (void *)(usrThreads[id]->stack);
        usrThreads[id]->context.uc_stack.ss_size = STACK_SIZE;
        
        makecontext(&(usrThreads[id]->context), task_func, 1, id);
    }

    // Loop through all task_sched until -1
    int id = 0;
    int task_id = 0;
    while (task_sched[id] != -1) {
        task_id = task_sched[id];
        if (task_id >= task_cnt) {
            freeThreads();
            return 1;
        }
        swapcontext(&main_context, &(usrThreads[task_id]->context));
        id++;
    }

    // free all dynamically allocated stacks
    freeThreads();

    return 0;
}

void scheduler_yield(int task_id)
{
    // switch from current task to the main thread
    swapcontext(&(usrThreads[task_id]->context), &main_context);
}