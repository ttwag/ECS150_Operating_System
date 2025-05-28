#ifndef QUEUE_H
#define QUEUE_H

typedef struct node {
    node *next;
    void *data;
} node_t;

/*
 * queue_t - Queue type
 *
 * A queue is a FIFO that stores data.
 */
typedef struct queue_t {
    node_t *front;
    node_t *end;
    int length;
} queue_t;

/*
 * queue_create - Allocate an empty queue_t
 * Return: Pointer to an empty queue_t, or NULL if allocation fails
 */
queue_t *queue_create();

/*
 * queue_destroy - Deallocate a queue_t
 * @queue: queue_t to deallocate
 * Return: 0 if success, -1 otherwise
 */
int queue_destroy(queue_t queue);

/*
 * queue_enqueue - Enqueue a data
 * @queue: queue_t to enqueue item to
 * @data: The data to enqueue
 * Return: 0 if success, -1 otherwise
 */
int queue_enqueue(queue_t queue, void *data);

/*
 * queue_dequeue - Dequeue the earliest enqueued data
 * @queue: queue_t to dequeue item from
 * @data: Address of data pointer where data is received
 * Return: 0 if success, -1 otherwise
 */
int queue_dequeue(queue_t queue, void **data);

/*
 * queue_delete - Delete a data
 * @queue: queue_t in which to delete the data
 * @data: The data to delete
 * Return: 0 if success, -1 otherwise
 */
int queue_delete(queue_t queue, void *data);

/*
 * queue_iterate - Iterate through all data in queue_t
 * @queue: queue_t to iterate
 * @func: Function to call on each queue data
 * Return: 0 if success, -1 otherwise
 */
int queue_iterate(queue_t queue, void (*func)(queue_t, void*));

/*
 * queue_length - queue_t's length
 * @queue: queue_t whose length is requested
 * Return: Length of queue, or -1 if failure
 */
int queue_length(queue_t queue);

#endif