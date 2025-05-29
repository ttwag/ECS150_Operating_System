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

void test_destroy() {
    int status = 0;
    
    // test NULL queue
    status = queue_destroy(NULL);
    assert(status == -1);
    
    // test empty queue
    queue_t qu = queue_create();
    status = queue_destroy(qu);
    assert(status == 0);
}

void test_enqueue_dequeue_edge() {
    queue_t qu = queue_create();
    int status = 0;
    void *empty_ptr = NULL;

    // enqueue a NULL queue
    status = queue_enqueue(NULL, NULL);
    assert(status == -1);

    // dequeue a NULL queue
    status = queue_dequeue(NULL, &empty_ptr);
    assert(status == -1);

    // dequeue an empty queue
    status = queue_dequeue(NULL, &empty_ptr);
    assert(status == -1);

    queue_destroy(qu);
}

void test_enqueue_dequeue_func() {
    queue_t qu = queue_create();
    void *data_ptr = NULL;
    int int_list[] = {3, 4, 5, 5, 6};
    char char_list[] = {'b', 'c', 'e', 'e', 'f'};
    char *ptr_list[] = {NULL, NULL, NULL, NULL};

    // test integer data
    for (int i = 0; i < 5; i++) {
        queue_enqueue(qu, &(int_list[i]));
    }

    for (int i = 0; i < 5; i++) {
        queue_dequeue(qu, &data_ptr);
        assert(data_ptr == &(int_list[i]));
    }

    // test char data
    for (int i = 0; i < 5; i++) {
        queue_enqueue(qu, &(char_list[i]));
    }

    for (int i = 0; i < 5; i++) {
        queue_dequeue(qu, &data_ptr);
        assert(data_ptr == &(char_list[i]));
    }

    // NULL pointers
    for (int i = 0; i < 4; i++) {
        queue_enqueue(qu, ptr_list[i]);
    }

    for (int i = 0; i < 4; i++) {
        queue_dequeue(qu, &data_ptr);
        assert(data_ptr == ptr_list[i]);
    }

    queue_destroy(qu);
}

void test_delete() {
    queue_t qu = queue_create();
    int status = 0;
    int *ptr = &status;
    void *data_ptr = NULL;
    char char_list[] = {'b', 'c', 'e', 'e', 'f'};

    // test delete NULL queue
    status = queue_delete(NULL, ptr);
    assert(status == -1);
    status = queue_delete(NULL, NULL);
    assert(status == -1);
    
    // test delete empty queue
    status = queue_delete(qu, NULL);
    assert(status == -1);
    
    // test one deletion
    for (int i = 0; i < 5; i++) {
        queue_enqueue(qu, &(char_list[i]));
    }

    queue_delete(qu, &(char_list[2]));

    assert(queue_length(qu) == 4);
    for (int i = 0; i < 4; i++) {
        queue_dequeue(qu, &data_ptr);
        if (i >= 2) assert(data_ptr == &(char_list[i+1]));
        else assert(data_ptr == &(char_list[i]));
    }
    queue_destroy(qu);
}

static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    assert(data[0] == 2);
    assert(queue_length(q) == 9);
    queue_destroy(q);
}

void test_length() {
    queue_t qu = queue_create();
    void *empty_ptr = NULL;
    int status = 0;

    queue_enqueue(qu, NULL);
    queue_enqueue(qu, NULL);
    queue_enqueue(qu, NULL);
    queue_enqueue(qu, NULL);
    assert(queue_length(qu) == 4);
    queue_dequeue(qu, &empty_ptr);
    queue_dequeue(qu, &empty_ptr);
    queue_dequeue(qu, &empty_ptr);
    queue_dequeue(qu, &empty_ptr);
    assert(queue_length(qu) == 0);

    status = queue_length(NULL);
    assert(status == -1);
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
    run_test("QUEUE_DESTROY TEST", test_destroy);
    run_test("QUEUE_ENQUEUE_DEQUEUE EDGE CASE TEST", test_enqueue_dequeue_edge);
    run_test("QUEUE_ENQUEUE_DEQUEUE TEST", test_enqueue_dequeue_func);
    run_test("QUEUE_DELETE TEST", test_delete);
    run_test("QUEUE_ITERATOR TEST", test_iterator);
    run_test("QUEUE_LENGTH TEST", test_length);
    printf("All Test Passed\n\n");
    return 0;
}