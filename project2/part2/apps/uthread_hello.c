/*
 * Generated by Claude Sonnet 4.0
 * Simple test program that creates a single thread 
 * that displays "Hello world!"
 */

#include <stdio.h>
#include <uthread.h>

void hello_thread(void *arg)
{
    printf("Hello world!\n");
}

int main(void)
{
    int retval;
    
    retval = uthread_run(false, hello_thread, NULL);
    
    if (retval) {
        printf("Error: uthread_run failed\n");
        return -1;
    }
    
    return 0;
}