#include <stdio.h>
#include <scheduler.h>

void task(int id) 
{
    while (1) {
        printf("task %d\n", id);
        scheduler_yield(id);
    }
}

int main() {
    const int schedule[] = {0, 1, 2, 2, 1, 0, -1};
    scheduler_run(task, 3, schedule);
    return 0;
}