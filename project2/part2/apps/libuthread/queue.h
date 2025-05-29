/*queue.h*/

#ifndef QUEUE_H
#define QUEUE_H

// queue_t is a pointer to a queue
typedef struct Queue * queue_t;

/*
 * queue_create - Allocate an empty queue_t
 * Return: queue_t pointed to an empty queue, or NULL if allocation fails
 */
queue_t queue_create(void);

/*
 * queue_destroy - Deallocate a queue_t
 * @queue: queue_t to deallocate
 * Return: 0 if success, -1 if queue is NULL
 */
int queue_destroy(queue_t queue);

/*
 * queue_enqueue - Enqueue a data
 * @queue: queue_t to enqueue item to
 * @data: The data to enqueue
 * Return: 0 if success, -1 if queue is NULL
 */
int queue_enqueue(queue_t queue, void *data);

/*
 * queue_dequeue - Dequeue the earliest enqueued data
 * @queue: queue_t to dequeue item from
 * @data: Address of data pointer where data is received
 * Return: 0 if success, -1 if queue is NULL or empty
 */
int queue_dequeue(queue_t queue, void **data);

/*
 * queue_delete - Delete a data
 * @queue: queue_t in which to delete the data
 * @data: The data to delete from the queue
 * Return: 0 if success, -1 if queue is NULL, empty, or doesn't contain the data
 */
int queue_delete(queue_t queue, void *data);

/*
 * queue_iterate - Iterate through all data in queue_t
 * @queue: queue_t to iterate
 * @func: Function to call on each queue data
 * Return: 0 if success, -1 if queue is NULL or empty
 */
int queue_iterate(queue_t queue, void (*func)(queue_t, void*));

/*
 * queue_length - queue_t's length
 * @queue: queue_t whose length is requested
 * Return: Length of queue, or -1 if queue is NULL
 */
int queue_length(queue_t queue);

#endif //QUEUE_H