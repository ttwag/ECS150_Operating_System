/*queue.c*/

#include <stdlib.h>
#include "queue.h"

/*
 * node_t - Node type
 *
 * A node_t is a node in a doubly-linked list.
 */
typedef struct Node {
    struct Node *next;
    struct Node *prev;
    void *data;
} Node;

/*
 * queue_t - Queue type
 *
 * A queue is a FIFO that stores data. It has two sentinel nodes, front and end.
 */
typedef struct Queue {
    int length;
    Node *front;
    Node *end;
} Queue;

queue_t queue_create(void) {
    Queue *queue_ptr = (Queue *)calloc(1, sizeof(Queue));
    if (queue_ptr != NULL) {
        queue_ptr->length = 0;
        // allocate the sentinel nodes
        queue_ptr->front = (Node *)calloc(1, sizeof(Node));
        queue_ptr->end = (Node *)calloc(1, sizeof(Node));
        
        // if either sentinel allocations failed, deallocate any partially allocated resources
        if (!queue_ptr->front || !queue_ptr->end) {
            queue_destroy(queue_ptr);
            return NULL;
        }

        queue_ptr->front->next = queue_ptr->end;
        queue_ptr->end->prev = queue_ptr->front;
    }
    return queue_ptr;
}

int queue_destroy(queue_t queue) {
    if (!queue) return -1;
    int size = queue->length;
    // Node *old_node = queue->front->next;
    
    // free all Node between sentinels
    void *empty_ptr = NULL;
    for (int i = 0; i < size; i++) {
        // dequeue all Node but don't free the data
        queue_dequeue(queue, &empty_ptr);
    }

    // free the sentinels
    free(queue->front);
    free(queue->end);

    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data) {
    if (!queue) return -1;
    Node *new_node = (Node *)calloc(1, sizeof(Node));
    Node *tail_sentinel = queue->end;

    if (!new_node) return -1;

    // make new node the last node before end
    new_node->next = tail_sentinel;
    new_node->prev = tail_sentinel->prev;
    new_node->data = data;

    tail_sentinel->prev->next = new_node;
    tail_sentinel->prev = new_node;

    queue->length++;

    return 0;
}

int queue_dequeue(queue_t queue, void **data) {
    if (!queue || queue_length(queue) <= 0) return -1;
    
    // use the node after front as curr because it was enqueued first
    Node *old_node = queue->front->next;
    Node *head_sentinel = queue->front;

    // store data pointer
    *data = old_node->data;

    // remove and deallocate curr
    head_sentinel->next = old_node->next;
    old_node->next->prev = head_sentinel;
    free(old_node);

    queue->length--;

    return 0;
}

int queue_delete(queue_t queue, void *data) {
    if (!queue || queue_length(queue) <= 0) return -1;
    
    Node *old_node = queue->front->next;

    while (old_node != queue->end) {
        if (old_node->data == data) {
            // remove this node
            old_node->prev->next = old_node->next;
            old_node->next->prev = old_node->prev;
            free(old_node);
            queue->length--;
            return 0;
        }
        old_node = old_node->next;
    }
    return -1;
}

int queue_iterate(queue_t queue, void (*func)(queue_t, void *)) {
    if (!queue || queue_length(queue) <= 0 || func == NULL) return -1;

    // save a copy of the current queue into an array of void pointers
    int size = queue_length(queue);
    void **qu_copy = (void **)calloc(size, sizeof(void *));
    if (qu_copy == NULL) return -1;

    Node *node = queue->front->next;
    for (int i = 0; i < size; i++) {
        qu_copy[i] = node->data;
        node = node->next;
    }

    // call the callback function on each element
    for (int i = 0; i < size; i++) {
        func(queue, qu_copy[i]);
    }

    free(qu_copy);

    return 0;
}

int queue_length(queue_t queue) {
    if (!queue) return -1;
    return queue->length;
}