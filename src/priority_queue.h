#ifndef A779246D_0893_4BC6_8336_B5E90F472076
#define A779246D_0893_4BC6_8336_B5E90F472076

#include "heap.h"

#define PRIORITY_QUEUE_GREATER 1
#define PRIORITY_QUEUE_LESS -1
#define PRIORITY_QUEUE_EQUAL 0

// priority queue is wrapped in Heap
#define priority_queue Heap


// create priority queue
static inline priority_queue *priority_queue_create(d_array_cmp compare, uint32_t initial_capacity)
{
    return heap_create(compare, initial_capacity);
}

// insert in priority queue
static inline int priority_queue_insert(priority_queue *queue, void *data)
{
    return heap_insert(queue, data);
}

// delete highest priority element from queue
static inline void *priority_queue_shift(priority_queue *queue)
{
    return heap_pop(queue);
}

// deallocate the whole heap
static inline int priority_queue_destroy(priority_queue **queue, d_array_dealloc dealloc_cb)
{
    return heap_destroy(queue, dealloc_cb);
}

#endif /* A779246D_0893_4BC6_8336_B5E90F472076 */
