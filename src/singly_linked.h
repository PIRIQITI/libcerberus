#ifndef D84C1D58_017D_4D7A_947A_C40713539E7F
#define D84C1D58_017D_4D7A_947A_C40713539E7F

#include <stdint.h>
#include "dbg.h"

#define CERB_OK 0
#define CERB_ERR -1

#define SINGLY_LINKED_GREATER 1
#define SINGLY_LINKED_LESS -1
#define SINGLY_LINKED_EQUAL 0

// compare function pointer for singly_linked (note consts and restrict. your function has to follow the rules imposed by those)
typedef int (*singly_linked_cmp) (const void *const restrict data1, const void *const restrict data2);
// deallocate callback for list (if your d_array contains structs containing other allocated data for example, you need this)
typedef void (*singly_linked_dealloc) (void *data);

typedef struct singly_linked_node {
    void *data; // data you store in list
    struct singly_linked_node *next; // next node
} singly_linked_node;

typedef struct singly_linked {
    singly_linked_node *first; // first node
    singly_linked_cmp compare; // compare callback
    uint32_t count; // count of list (length)
} singly_linked;

// create singly_linked *list ( specify cmp if you need to sorted insert )
singly_linked *singly_linked_create(singly_linked_cmp compare); //
// push data in list
int singly_linked_push(singly_linked *restrict list, void *restrict data); //
// pop data from list and return
void *singly_linked_pop(singly_linked *restrict list); //
// insert data at first position in list
int singly_linked_unshift(singly_linked *restrict list, void *restrict data); //
// remove first entry from list and return
void *singly_linked_shift(singly_linked *restrict list); //
// remove singly_linkedNode *node from list and return
void *singly_linked_remove(singly_linked *restrict list, void *restrict node); //
// insert data after singly_linkedNode *after
int singly_linked_insert_after(singly_linked *restrict list, void *restrict after, void *restrict data); //
// remove data after singly_linkedNode *node and return
void *singly_linked_remove_after(singly_linked *restrict list, void *restrict node); //
// insert data before singly_linkedNode *before
int singly_linked_insert_before(singly_linked *restrict list, void *restrict before, void *restrict data); //
// remove data before singly_linkedNode *node and return
void *singly_linked_remove_before(singly_linked *restrict list, void *restrict node); //
// join 2 lists
// provide references to them
int singly_linked_join(singly_linked *restrict *restrict list1, singly_linked *restrict *restrict list2); //
// split singly_linked **list from node to node, provide reference to list
// never pass nodes with wrong order as it leads to some strange behaviour. always pass from left to right
singly_linked *singly_linked_split(singly_linked *restrict *restrict list, singly_linked_node *restrict from_node,
singly_linked_node *restrict to_node);

// to clear list from data it contains
// only use these on basic data structures ( int char float ) etc.
// this function is taken out for purpose                         int singly_linked_clear(singly_linked *list); //

// free nodes, data and struct itself. provide reference to list and singly_linked_dealloc callback for correct deallocation
int singly_linked_destroy(singly_linked *restrict *restrict list, singly_linked_dealloc dealloc_cb);

// always pass a reference to make it NULL after freeing
// this function is taken out for purpose                         int singly_linked_clear_destroy(singly_linked **list);

// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
// free_func is a typedef. you have to write your own function which matches the type of free_func and pass that
// int singly_linked_free_complex_data(singly_linked **list, singly_linked_dealloc handler_func);
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ above function was taken out on purpose

// int singly_linked_free_list(singly_linked **list); // to free just list and its nodes, but not data it contains
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^ above function was taken out on purpose

// sorted insert in list ( SHOULD BE INSERTED WITH THIS FUNCTION ONLY, IF YOU WANT TO HAVE SORTED ARRAY )
// int singly_linked_sorted_insert(singly_linked *list, void *data);
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ above function was taken out on purpose

// L is singly_linked *list
// C is current node's name you want it to have
#define singly_linked_iter(L, C) singly_linked_node *_node = NULL; singly_linked_node *C = NULL;\
        for (C = _node = (L)->first; _node != NULL; _node = C = _node->next)

// list is singly_linked *list
// _data is a function pointer returning anything that matches format ( %s %d %u ) etc.
// format is "%s", "%d" ...
#define singly_linked_print(list, _data, _format)\
        if (list) {\
            printf("\n[ ");\
            singly_linked_iter (list, cur) { printf((_format " -> "), _data(cur->data)); }\
            printf("NULL ]\n");\
        } else {\
            log_err("list is NULL.");\
        }

// list is singly_linked *list
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define singly_linked_search(list, cmp_func, to_find, found_node) singly_linked_node *found_node = NULL;\
        if (list) {\
            singly_linked_iter (list, cur) { if (cmp_func(to_find, cur->data) == 0) {\
                    found_node = cur;\
                    break;\
                }\
            }\
        } else {\
            log_err("list is NULL.");\
        }

// reverse singly_linked *list
#define singly_linked_reverse(list)\
    if (list) {\
        if (list->count > 1) {\
            singly_linked_node *first = list->first;\
            singly_linked_node *second = first->next;\
            first->next = NULL; /* NULL out first->next as it will become last->next */\
            singly_linked_node *third = second->next;\
            for (; third != NULL; second->next = first, first = second, second = third, third = second->next){}\
            second->next = first;\
            list->first = second;\
        } else {\
            log_err("Couldn't reverse list of count < 2.");\
        }\
    } else {\
        log_err("list is NULL.");\
    }

#endif /* D84C1D58_017D_4D7A_947A_C40713539E7F */
