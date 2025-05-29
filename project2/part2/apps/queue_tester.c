#include <assert.h>
#include <stdio.h>
#include <queue.h>

/* queue test */
void test_create() {
    queue_t qu = queue_create();
    assert(qu != NULL);
    assert(queue_length(qu) == 0);
    queue_destroy(qu);
}



void run_test(const char *test_name, void (*test_func)(void)) {
    printf("Running %s...", test_name);
    test_func();
    printf("PASSED\n");
}

int main() {
    printf("\n===Begin Queue Unit Test===\n");
    run_test("QUEUE_CREATE TEST", test_create);
    printf("All Test Passed\n\n");
    return 0;
}