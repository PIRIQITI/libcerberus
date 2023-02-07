#ifndef D9763356_738A_4CC3_81EE_E99D6E5B226A
#define D9763356_738A_4CC3_81EE_E99D6E5B226A

#include "doubly_linked.h"

#define Queue doubly_linked

// get how many elements queue has
#define Queue_get_count(queue) doubly_linked_get_count(queue)

// create a queue (queue_compare is compare callback. see default in doubly_linked.c)
// #define Queue_create(queue_compare) doubly_linked_create(queue_compare)

static inline Queue *Queue_create(doubly_linked_cmp cmp)
{
    return doubly_linked_create(cmp);
}

// unshift void *data in queue
// #define Queue_unshift(queue, data) doubly_linked_unshift(queue, data)

// unshift void *data in queue
static inline int Queue_push(Queue *queue, void *data)
{
    return doubly_linked_push(queue, data);
}

// pop and return from queue
// #define Queue_pop(queue) doubly_linked_pop(queue)

// pop and return from queue
static inline void *Queue_shift(Queue *queue)
{
    return doubly_linked_shift(queue);
}

// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
// #define Queue_destroy(queue, dealloc_cb) doubly_linked_destroy(queue, dealloc_cb)

// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
static inline int Queue_destroy(Queue **queue, doubly_linked_dealloc dealloc_cb)
{
    return doubly_linked_destroy(queue, dealloc_cb);
}

// iterate through queue
// queue is Queue *, cur is current node's name you specify
#define Queue_iter(queue, cur) doubly_linked_iter(queue, first, next, cur)
// print queue contents
// queue is Queue *, _data is a function returning printable data like ( int, char *, char, float ) ...
// format is "%s" "%d" ... according to what data returns
#define Queue_print(queue, _data, format) doubly_linked_print(queue, first, next, _data, format)
// queue is Queue *
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define Queue_search(queue, cmp_func, to_find, found_node) doubly_linked_search(queue, first, next, cmp_func, to_find, found_node)

#endif /* D9763356_738A_4CC3_81EE_E99D6E5B226A */