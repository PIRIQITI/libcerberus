#ifndef DA30B4E4_4771_452B_ABA5_8E814ED70200
#define DA30B4E4_4771_452B_ABA5_8E814ED70200

#include "doubly_linked.h"

#define Stack doubly_linked

// get how many elements stack has
#define Stack_get_count(stack) doubly_linked_get_count(stack)

// create a stack (stack_compare is compare callback. see default in doubly_linked.c)
// #define Stack_create(stack_compare) doubly_linked_create(stack_compare)

// create a stack (stack_compare is compare callback. see default in doubly_linked.c)
static inline Stack *Stack_create(doubly_linked_cmp cmp)
{
    return doubly_linked_create(cmp);
}

// push void *data in stack
// #define Stack_push(stack, data) doubly_linked_push(stack, data)

// push void *data in stack
static inline int Stack_push(Stack *stack, void *data)
{
    return doubly_linked_push(stack, data);
}

// pop data from stack and return (freeing returned data is your responsibility)
// #define Stack_pop(stack) doubly_linked_pop(stack)

// pop data from stack and return (freeing returned data is your responsibility)
static inline void *Stack_pop(Stack *stack)
{
    return doubly_linked_pop(stack);
}

// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
static inline int Stack_destroy(Stack **stack, doubly_linked_dealloc dealloc_cb)
{
    return doubly_linked_destroy(stack, dealloc_cb);
}

// iterate through stack
// stack is Stack *, cur is current node's name you specify
#define Stack_iter(stack, cur) doubly_linked_iter(stack, last, prev, cur)
// print stack contents
// stack is Stack *, _data is a function returning printable data like ( int, char *, char, float ) ...
// format is "%s" "%d" ... according to what data returns
#define Stack_print(stack, _data, format) doubly_linked_print(stack, last, prev, _data, format)
// stack is Stack *
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define Stack_search(stack, cmp_func, to_find, found_node) doubly_linked_search(stack, last, prev, cmp_func, to_find, found_node)

#endif /* DA30B4E4_4771_452B_ABA5_8E814ED70200 */