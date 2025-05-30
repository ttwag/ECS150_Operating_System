// uthread.h - thread API
#ifndef UTHREAD_H
#define UTHREAD_H

#include <stdbool.h>

// thread type
typedef int uthread_t;

/*
 * uthread_run - Initialize the threading system
                - Register the current thread as the idle thread
                - Creates an initial thread to execute func(arg)
                - Scheduler schedules the remaining threads in round robin 
                - Once all threads are completed, return to the idle thread
                - Non-preemptive: child thread must call uthread_yield() to yield control
 * @preempt: Enable preemptive scheduling
 * @func: Function for initial thread to execute
 * @arg: Argument passed to func
 * Return: 0 if success, -1 otherwise
 */
int uthread_run(bool preempt, void (*func)(void *), void *arg);


/*
 * uthread_create - Create a new thread with func(arg) for the scheduler to schedule
 * @func: Function for the new thread to execute
 * @arg: Argument passed to func
 * Return: Thread ID of the new thread, -1 if not successful
 */
uthread_t uthread_create(void (*func)(void *), void *arg);


/*
 * uthread_yield - Yield control to the next thread in round-robin queue
 * Return: None
 */
void uthread_yield(void);


/*
 * uthread_exit - Exit and delete the current thread and yield to the next thread in round-robin queue
 * Return: None
 */
void uthread_exit(void);

/*
 * uthread_self - shows the identification number of the current thread
 * Return: Thread ID of the current thread
 */
uthread_t uthread_self(void);

#endif //UTHREAD_H