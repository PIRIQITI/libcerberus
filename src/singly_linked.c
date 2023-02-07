#include "singly_linked.h"
#include <stdlib.h>

/* create operations */

static inline int default_compare(const void *const restrict data1, const void *const restrict data2)
{
    if ( *( (int *) data1 ) < *( (int *) data2 ) ) {
        return SINGLY_LINKED_LESS;
    } else if ( *( (int *) data1 ) > *( (int *) data2 ) ) {
        return SINGLY_LINKED_GREATER;
    } else {
        return SINGLY_LINKED_EQUAL;
    }
}

singly_linked *singly_linked_create(singly_linked_cmp cmp)
{
    singly_linked *list = calloc(1, sizeof(singly_linked));
    check_mem(list);

    list->compare = cmp == NULL ? default_compare : cmp;
    list->count = 0;

    return list;

error:
    return NULL;
}

static inline singly_linked_node *singly_linked_node_create()
{
    singly_linked_node *node = calloc(1, sizeof(singly_linked_node));
    check_mem(node);

    return node;

error:
    return NULL;
}

/* __insert operations */

// pushes at the end of a given list
int singly_linked_push(singly_linked *restrict list, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count <= UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    singly_linked_node *new_node = singly_linked_node_create();
    check(new_node != NULL, "Failed to create new node.");

    new_node->data = data;
    new_node->next = NULL;

    // if list->first is NULL we skip this loop and __insert the first element
    singly_linked_iter (list, cur) {
        if (!cur->next) {
            cur->next = new_node;
            list->count++;
            return CERB_OK;
        }
    }
    list->first = new_node;
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

// adds new data at the front of a given list
int singly_linked_unshift(singly_linked *restrict list, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count <= UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    singly_linked_node *new_node = singly_linked_node_create();
    check(new_node != NULL, "Failed to create new node.");

    new_node->data = data;
    new_node->next = list->first;
    list->first = new_node;
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

static inline singly_linked_node *in_list(singly_linked *restrict list, void *restrict entry)
{
    singly_linked_iter (list, cur) {
        if (list->compare(cur->data, entry) == SINGLY_LINKED_EQUAL) {
            return cur;
        }
    }

    return NULL;
}

// inserts after a given entry
int singly_linked_insert_after(singly_linked *restrict list, void *restrict entry, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(entry != NULL, "after is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count <= UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    singly_linked_node *node; // initialized in below if

    if ((node = in_list(list, entry)) != NULL) {
        singly_linked_node *new_node = singly_linked_node_create();
        check(new_node != NULL, "Failed to create new node.");

        new_node->data = data;
        new_node->next = node->next;
        node->next = new_node;
        list->count++;
        return CERB_OK;
    }

    log_err("entry %p doesn't belong to this list.", entry);

error: // fall through
    return CERB_ERR;
}

// inserts before a given entry
int singly_linked_insert_before(singly_linked *restrict list, void *restrict entry, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(entry != NULL, "entry is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count <= UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    // checking for first and last
    if (list->first && list->compare(list->first->data, entry) == SINGLY_LINKED_EQUAL) {
        int rc = singly_linked_unshift(list, data);
        check(rc != CERB_ERR, "Failed to insert before %p", entry);
        return CERB_OK;
    }

    // this can't be done with in_list() function so we are doing it this way
    // confirm that *after is indeed in this list
    singly_linked_iter (list, cur) { // these checks everything but the first and last nodes
        if (!cur->next) break; // to prevent NULL dereference at last node
        if (list->compare(cur->next->data, entry) == SINGLY_LINKED_EQUAL) {
            singly_linked_node *new_node = singly_linked_node_create();
            check(new_node != NULL, "Failed to create new node.");

            new_node->data = data;
            new_node->next = cur->next;
            cur->next = new_node;
            list->count++;
            return CERB_OK;
        }
    }

    log_err("entry %p doesn't belong to this list.", entry);

error: // fall through
    return CERB_ERR;
}

/* remove operations */

// pops last element from list (freeing returned memory is your responsibility)
void *singly_linked_pop(singly_linked *restrict list)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(list->count != 0, "Couldn't remove from an empty list.");

    if (list->count == 1) {
        // means we are removing the one and only element 
        data = list->first->data;
        free(list->first);
        list->first = NULL;
        list->count = 0; // or --; as list gets empty
    } else {
        singly_linked_iter (list, cur) {
            if (!cur->next->next) { // delete after cur
                data = cur->next->data;
                free(cur->next);
                cur->next = NULL;
                list->count--;
            }
        }
    }

error: // fall through
    return data;
}

// removes data at the start of a list (freeing returned memory is your responsibility)
void *singly_linked_shift(singly_linked *restrict list)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");

    if (list->first) {
        data = list->first->data;
        singly_linked_node *first_node = list->first; // not to loose or insta free the pointer
        list->first = list->first->next;
        free(first_node);
        list->count--;
        return data;
    }

    log_err("Couldn't shift from empty list.");

error: // fall through
    return data;
}

// remove specific entry from list (freeing returned data is your responsibility)
void *singly_linked_remove(singly_linked *restrict list, void *restrict entry)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(entry != NULL, "entry is NULL.");
    check(list->count != 0, "Couldn't remove from an empty list.");

    if (list->compare(list->first->data, entry) == SINGLY_LINKED_EQUAL) {
        return singly_linked_shift(list);
    }

    singly_linked_iter (list, cur) {
        if (!cur->next) break;
        if (list->compare(cur->next->data, entry) == SINGLY_LINKED_EQUAL) {
            data = cur->next->data;
            singly_linked_node *remove_node = cur->next;
            cur->next = cur->next->next;
            free(remove_node);
            list->count--;
            return data;
        }
    }

    log_err("Invalid node.");

error: // fall through
    return data;
}

// removes after given entry (freeing returned data is your responsibility)
void *singly_linked_remove_after(singly_linked *restrict list, void *restrict entry)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(entry != NULL, "entry is NULL.");
    check(list->count != 0, "Couldn't remove from an empty list.");

    singly_linked_node *node; // initialized in if block

    if ((node = in_list(list, entry)) != NULL && node->next) {
        data = node->next->data;
        singly_linked_node *after_next = node->next; // not to loose or insta free the pointer
        node->next = node->next->next;
        free(after_next);
        list->count--;
        return data;
    }

    log_err("Invalid entry.");

error: // fall through
    return data;
}

// removes before given entry (freeing returned data is your responsibility)
void *singly_linked_remove_before(singly_linked *restrict list, void *restrict entry)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(entry != NULL, "before is NULL.");
    check(list->count > 1, "Couldn't remove before from a list with count of 1 or 0.");
    // check(list->compare(list->first->data, entry) == SINGLY_LINKED_EQUAL, "Couldn't remove before the first element.");

    if (list->compare(list->first->next->data, entry) == SINGLY_LINKED_EQUAL) {
        // removing before the second element, so we are shifting
        return singly_linked_shift(list);
    } else {
        // check to see if the *before is indeed in this list (can't be done with in_list() function)
        singly_linked_iter (list, cur) {
            if (!cur->next->next) break;
            if (list->compare(cur->next->next->data, entry) == SINGLY_LINKED_EQUAL) {
                data = cur->next->data;
                singly_linked_node *cur_next = cur->next;  // not to loose or insta free the pointer
                cur->next = cur->next->next;
                free(cur_next);
                list->count--;
                return data;
            }
        }
    }

    log_err("Invalid entry.");

error: // fall through
    return data;
}

/* joining and splitting */

// this will extend list1 with elements of list2, free list2 and set it's reference to NULL
int singly_linked_join(singly_linked *restrict *restrict list1, singly_linked *restrict *restrict list2)
{
    check(list1 != NULL, "Address of list1 is NULL.");
    check(*list1 != NULL, "list1 is NULL.");
    check(list2 != NULL, "Address of list2 is NULL.");
    check(*list2 != NULL, "list2 is NULL.");
    check((*list1)->count != 0, "Doesn't make sense to join to an empty list.");
    check((*list2)->count != 0, "Doesn't make sense to join an empty list.");    
    check((*list1)->count + (*list2)->count <= UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");
    check((*list1)->compare == (*list2)->compare, "Couldn't join lists of different compare callbacks");

    singly_linked_iter (*list1, cur) {
        if (!cur->next) {
            cur->next = (*list2)->first;
            (*list1)->count += (*list2)->count;
            break;
        }
    }

    free(*list2);
    *list2 = NULL;
    
    return CERB_OK;

error:
    return CERB_ERR;
}

// this function is used in doubly_linked_split to locate doubly_linked_node * pointers
static inline int node_in_list(singly_linked *restrict list, singly_linked_node *restrict node)
{
    singly_linked_iter (list, cur) {
        if (cur == node) {
            return CERB_OK;
        }
    }

    return CERB_ERR;
}

// this will return new list splitted from **list, original list is changed via pointer (please provide an address)
singly_linked *singly_linked_split(singly_linked *restrict *restrict list, singly_linked_node *restrict from_node,
singly_linked_node *restrict to_node)
{
    check(list != NULL, "Address of list is NULL.");
    check(*list != NULL, "list is NULL.");
    check(!(from_node == NULL && to_node == NULL), "Invalid nodes.");

    if (node_in_list(*list, from_node) == CERB_ERR && from_node != NULL) {
        log_err("Invalid from_node.");
        return NULL;
    } else if (node_in_list(*list, to_node) == CERB_ERR && to_node != NULL) {
        log_err("Invalid to_node.");
        return NULL;
    }

    singly_linked *new_list = NULL;
    uint32_t count = 0;

    if (from_node == NULL) {
        singly_linked_iter (*list, cur) {

            count++;

            if (cur == to_node) {
                check(cur->next, "Doesn't make sense to split from first including last.");

                new_list = singly_linked_create((*list)->compare);
                check(new_list != NULL, "Couldn't create new list.");

                new_list->first = (*list)->first;
                (*list)->first = cur->next;
                cur->next = NULL;
                new_list->count = count;
                (*list)->count = (*list)->count - count;
                return new_list;
            }
        }
    } else if (to_node == NULL) {
        check(from_node != (*list)->first, "Doesn't make sense to split from first including last.");

        singly_linked_iter (*list, cur) {

            count++;

            if (cur->next == from_node) {
                new_list = singly_linked_create((*list)->compare);
                check(new_list != NULL, "Couldn't create new list.");

                new_list->first = from_node;
                cur->next = NULL;
                new_list->count = (*list)->count - count;
                (*list)->count = count;
                return new_list;
            }
        }
    } else {

        if (from_node == (*list)->first) {
            new_list = singly_linked_split(list, NULL, to_node);
            if (!new_list) goto error;
            return new_list;
        }
        
        // if we are splitting not from the first to not to the last
        singly_linked_iter (*list, cur) {
            if (cur->next == from_node) {
                new_list = singly_linked_create((*list)->compare);
                check(new_list != NULL, "Couldn't create new list.");
                new_list->first = from_node;
                cur->next = to_node->next; // move link to to node's next element
                to_node->next = NULL; // set this to NULL as it becomes the last element of new list

                // make new loop for counting
                singly_linked_node *new_list_node = new_list->first;
                count++;
                while (new_list_node->next) {
                    new_list_node = new_list_node->next;
                    count++;
                }

                // set new_list->count to count coming from above while loop
                new_list->count = count;
                (*list)->count = (*list)->count - count;

                return new_list;
            }
        }
    }

error:
    return NULL;
}

/* clear and destroy operations */

// frees all nodes, but not data or list itself
int singly_linked_clear(singly_linked *restrict list)
{
    check(list != NULL, "list is NULL.");

    singly_linked_iter (list, cur) {
        free(cur->data);
        cur->data = NULL;
    }
    list->count = 0; // set count to 0 as nodes have been cleared

    return CERB_OK;

error:
    return CERB_ERR;
}

// free nodes, data and struct itself. pass by reference to make it NULL, so no access after destroyed
int singly_linked_destroy(singly_linked *restrict *restrict list, singly_linked_dealloc dealloc_cb)
{
    check(list != NULL, "Address of list is NULL.");
    check(*list != NULL, "list is NULL.");

    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    singly_linked_node *cur_node = (*list)->first;
    singly_linked_node *next_node = NULL;
    
    for (next_node = cur_node->next; next_node != NULL; next_node = cur_node->next) {
        dealloc_cb(cur_node->data);
        free(cur_node);
        cur_node = next_node;
    }
    dealloc_cb(cur_node->data);
    free(cur_node); // free the last one and we are done

empty_list: // fall through
    free(*list);
    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int singly_linked_clear_destroy(singly_linked **list) // pass by reference to make it NULL so no access after destroyed
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    singly_linked_node *cur_node = (*list)->first;
    singly_linked_node *next_node = NULL; // pottential bug here
    
    for (next_node = cur_node->next; next_node != NULL; next_node = cur_node->next) {
        free(cur_node->data);
        free(cur_node);
        cur_node = next_node;
    }
    free(cur_node->data);
    free(cur_node); // free the last one and we are done

empty_list: // fall through
    free(*list);
    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int singly_linked_free_complex_data(singly_linked **list, singly_linked_dealloc handler_func)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    singly_linked_node *cur_node = (*list)->first;
    singly_linked_node *next_node = NULL; // pottential bug here
    
    for (next_node = cur_node->next; next_node != NULL; next_node = cur_node->next) {
        handler_func(cur_node->data);
        free(cur_node);
        cur_node = next_node;
    }
    handler_func(cur_node->data);
    free(cur_node); // free the last one and we are done

empty_list: // fall through
    free(*list);
    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int singly_linked_free_list(singly_linked **list)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    singly_linked_iter (*list, cur) {
        free(cur);
    }

    free(*list);

    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

/* sorted insertion */

int singly_linked_sorted_insert(singly_linked *list, void *data) // see if you can optimise insert before
{
    int rc = -1;

    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");

    // this function is optimisable by a lot but if i do
    // code becomes very ugly and hard to understand so i'll leave it as is
    
    singly_linked_iter (list, cur) {
        if (list->compare(cur->data, data) > 0) {
            rc = singly_linked_insert_before(list, cur, data);
            check(rc != CERB_ERR, "Failed to sorted_insert.");
            return rc;
        }
    }
    // if all insert_before attampts fail it means that we have to insert at the end
    rc = singly_linked_push(list, data);
    check(rc, "Failed to sorted_insert.");

error:
    return rc;
}