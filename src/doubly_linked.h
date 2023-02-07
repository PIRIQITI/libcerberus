#ifndef A7E33D4E_E4E9_4637_8A99_8C8A1F279553
#define A7E33D4E_E4E9_4637_8A99_8C8A1F279553

#include <stdint.h>
#include "dbg.h"

#define CERB_OK 0
#define CERB_ERR -1

#define DOUBLY_LINKED_GREATER 1
#define DOUBLY_LINKED_LESS -1
#define DOUBLY_LINKED_EQUAL 0

// compare function pointer for doubly_linked (note consts and restrict. your function has to follow the rules imposed by those)
typedef int (*doubly_linked_cmp) (const void *const restrict data1, const void *const restrict data2);
// deallocate callback for list (if your d_array contains structs containing other allocated data for example, you need this)
typedef void (*doubly_linked_dealloc) (void *data);

typedef struct doubly_linked_node {
    struct doubly_linked_node *next; // next node
    struct doubly_linked_node *prev; // previous node
    void *data; // data you store
} doubly_linked_node;

typedef struct doubly_linked {
    doubly_linked_node *first; // first node
    doubly_linked_node *last; // last node
    doubly_linked_cmp compare; // compare callback
    uint32_t count;
} doubly_linked;

// create doubly_linked *list ( specify cmp if you need to sorted insert )
doubly_linked *doubly_linked_create(doubly_linked_cmp cmp); //
// push data in list
int doubly_linked_push(doubly_linked *restrict list, void *restrict data); //
// pop data and return
void *doubly_linked_pop(doubly_linked *restrict list); //
// insert in the front
int doubly_linked_unshift(doubly_linked *restrict list, void *restrict data); //
// remove first element from list and return
void *doubly_linked_shift(doubly_linked *restrict list); //
// remove doubly_linked_node *node from list
void *doubly_linked_remove(doubly_linked *restrict list, void *restrict data); //
// insert node after doubly_linked_node *node
int doubly_linked_insert_after(doubly_linked *restrict list, void *restrict entry, void *restrict data); //
// remove node after doubly_linked_node *node
void *doubly_linked_remove_after(doubly_linked *restrict list, void *restrict entry); //
// insert node before doubly_linked_node *node
int doubly_linked_insert_before(doubly_linked *restrict list, void *restrict entry, void *restrict data); //
// remove node before doubly_linked_node *node
void *doubly_linked_remove_before(doubly_linked *restrict list, void *restrict entry); //
// join 2 lists
// provide references to them
int doubly_linked_join(doubly_linked *restrict *restrict list1, doubly_linked *restrict *restrict list2); //
// split doubly_linked **list from node to node. please provide reference
// never pass nodes with wrong order as it leads to some strange. always pass from left to right
doubly_linked *doubly_linked_split(doubly_linked **list, doubly_linked_node *restrict from_node,
doubly_linked_node *restrict to_node); //

// to clear list from data it contains
// only use these on basic data structures ( int char float ) etc.
// this function was taken out on purpose                          int doubly_linked_clear(doubly_linked *list);
// to free just list and its nodes but not data it contains

// free nodes, data and struct itself. provide reference to list and singly_linked_dealloc callback for correct deallocation
int doubly_linked_destroy(doubly_linked *restrict *restrict list, doubly_linked_dealloc dealloc_cb);

// combined doubly_linked_clear and doubly_linked_destroy
// always pass a reference to make it NULL after freeing
// this function was taken out on purpose                          int doubly_linked_clear_destroy(doubly_linked **list);

// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
// free_func is a typedef. you have to write your own function which matches the type of free_func and pass that
// int doubly_linked_free_complex_data(doubly_linked **list, free_func handler_func);
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ this function was taken out on purpose

// int doubly_linked_free_list(doubly_linked **list); // to free just list and its nodes, but not data it contains
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^ this function was taken out on purpose

// sorted insert in list ( SHOULD BE INSERTED WITH THIS FUNCTION ONLY, IF YOU WANT TO HAVE SORTED ARRAY )
// int doubly_linked_sorted_insert(doubly_linked *list, void *data);
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ this function was taken out on purpose

#define doubly_linked_get_count(list) (list)->count

// L is doubly_linked *list
// S is starting node ( first or last ) D is direction ( next or prev )
// C is current node's name you want it to have
#define doubly_linked_iter(L, S, D, C) doubly_linked_node *_node = NULL; doubly_linked_node *C = NULL;\
        for (C = _node = (L)->S; _node != NULL; _node = C = _node->D)

// list is doubly_linked *list start is starting node ( first or last ) dir is direction ( next or prev )
// _data is a function pointer returning anything that matches format ( %s %d %u ) etc.
// format is "%s", "%d" ...
#define doubly_linked_print(list, start, dir, _data, format) if (list) {\
            printf("\n[ ");\
            doubly_linked_iter (list, start, dir, cur) { printf((format " -> "), _data(cur->data)); }\
            printf("NULL ]\n");\
        } else {\
            log_err("list is NULL.");\
        }

// list is doubly_linked *list
// start is starting node ( first or last ) dir is direction ( next or prev )
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define doubly_linked_search(list, start, dir, cmp_func, to_find, found_node) SLinkedNode *found_node = NULL;\
        if (list) {\
            doubly_linked_iter (list, start, dir, cur) {\
                if (cmp_func(to_find, cur->data) == 0) {\
                    found_node = cur;\
                    break;\
                }\
            }\
        } else {\
            log_err("list is NULL.");\
        }

#endif /* A7E33D4E_E4E9_4637_8A99_8C8A1F279553 */