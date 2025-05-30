// private.h - context API
#ifndef PRIVATE_H
#define PRIVATE_H

#define _XOPEN_SOURCE
#include <ucontext.h>

/*
 * uthread_ctx_alloc_stack - Allocate the stack for a context
 * @stack_ptr: A pointer to a pointer pointing to the memory address of the will-be allocated stack
 * Return: 0 if success, -1 otherwise
 */
int uthread_ctx_alloc_stack(void **stack_ptr);

/*
 * uthread_ctx_destroy_stack - Deallocate a context's stack
 * @stack_ptr: A pointer to the beginning of the stack address
 * Return: None
 */
void uthread_ctx_destroy_stack(void *stack_ptr);

/*
 * uthread_ctx_init - Initialize a context to host a thread
 * @ctx: context of a thread
 * @stack_ptr: Pointer to the context's stack
 * @func: Function for the context to execute
 * @arg: Argument passed to the function
 * Return: 0 if success, -1 otherwise
 */
int uthread_ctx_init(ucontext_t *ctx, void *stack_ptr, void (*func)(void *), void *arg);

/*
 * uthread_ctx_switch - Switch from current context to the next context
 * @prev: Current context
 * @next: Next context
 * Return: 0 if success, -1 otherwise
 */
int uthread_ctx_switch(ucontext_t *prev, ucontext_t *next);

#endif //PRIVATE_H