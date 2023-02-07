#include "doubly_linked.h"
#include <string.h>
#include <stdlib.h>

static inline int default_compare(const void *restrict data1, const void *restrict data2)
{
    if ( *( (int *) data1 ) < *( (int *) data2 ) ) {
        return DOUBLY_LINKED_LESS;
    } else if ( *( (int *) data1 ) > *( (int *) data2 ) ) {
        return DOUBLY_LINKED_GREATER;
    } else {
        return DOUBLY_LINKED_EQUAL;
    }
}

doubly_linked *doubly_linked_create(doubly_linked_cmp cmp)
{
    doubly_linked *list = calloc(1, sizeof(doubly_linked));
    check_mem(list);

    list->compare = cmp == NULL ? default_compare : cmp;
    list->count = 0;

    return list;

error:
    return NULL;
}

static inline doubly_linked_node *doubly_linkedNode_create()
{
    doubly_linked_node *node = calloc(1, sizeof(doubly_linked_node));
    check_mem(node);

    return node;

error:
    return NULL;
}

/* insert operations */

// insert data at the end of a given list
int doubly_linked_push(doubly_linked *restrict list, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count < UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    doubly_linked_node *new_node = doubly_linkedNode_create();
    check(new_node != NULL, "Failed to push in list.");
    new_node->data = data;

    if (!list->count) { // if we only have one element first and last should be the same
        list->first = new_node;
        list->last = new_node;
    } else {
        doubly_linked_node *temp = list->last;
        list->last = new_node;
        new_node->prev = temp;
        temp->next = new_node;
    }
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

// insert data at the beginning of a given list
int doubly_linked_unshift(doubly_linked *restrict list, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count < UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    doubly_linked_node *new_node = doubly_linkedNode_create();
    check(new_node != NULL, "Failed to push in list.");
    new_node->data = data;

    if (!list->count) { // if we only have one element first and last should be the same
        list->first = new_node;
        list->last = new_node;
    } else {
        doubly_linked_node *temp = list->first;
        list->first = new_node;
        new_node->next = temp;
        temp->prev = new_node;
    }
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

// returns doubly_linked_node on success or NULL on faliure
static inline doubly_linked_node *in_list(doubly_linked *restrict list, void *restrict entry)
{
    doubly_linked_iter(list, first, next, cur) {
        if (list->compare(cur->data, entry) == DOUBLY_LINKED_EQUAL) {
            return cur;
        }
    }

    return NULL;
}

// insert data after entry
int doubly_linked_insert_after(doubly_linked *restrict list, void *restrict entry, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(entry != NULL, "entry is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count < UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    doubly_linked_node *node; // initialized in else if block

    if (list->compare(entry, list->last->data) == DOUBLY_LINKED_EQUAL) {
        int rc = doubly_linked_push(list, data);
        check(rc != CERB_ERR, "Failed insert after %p", entry);
        return rc;
    } else if ((node = in_list(list, entry)) != NULL) {
        doubly_linked_node *new_node = doubly_linkedNode_create();
        check(new_node != NULL, "Failed to insert after %p.", entry);
        new_node->data = data;

        doubly_linked_node *temp = node->next;
        node->next = new_node;
        new_node->next = temp;
        new_node->prev = node;
        temp->prev = new_node;
        list->count++;
        return CERB_OK;
    } else {
        log_err("entry %p doesn't belong to this list.", entry);
    }

error: // fall through
    return CERB_ERR;
}

// insert data before entry
int doubly_linked_insert_before(doubly_linked *restrict list, void *restrict entry, void *restrict data)
{
    check(list != NULL, "list is NULL.");
    check(entry != NULL, "entry is NULL.");
    check(data != NULL, "data is NULL.");
    check(list->count < UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");

    doubly_linked_node *node; // initialized in else if block

    if (list->compare(list->first->data, entry) == DOUBLY_LINKED_EQUAL) {
        int rc = doubly_linked_unshift(list, data);
        check(rc != CERB_ERR, "Failed to insert before %p", entry);
        return rc;
    } else if ((node = in_list(list, entry)) != NULL) {
        doubly_linked_node *new_node = doubly_linkedNode_create();
        check(new_node != NULL, "Failed to insert before %p", entry);
        new_node->data = data;

        doubly_linked_node *temp = node->prev;
        node->prev = new_node;
        new_node->prev = temp;
        new_node->next = node;
        temp->next = new_node;
        list->count++;
        return CERB_OK;
    } else {
        log_err("entry %p doesn't belong to this list.", entry);
    }

error: // fall through
    return CERB_ERR;
}

/* remove operations */

// removes last element from a given list (freeing returned memory is your responsibility)
void *doubly_linked_pop(doubly_linked *restrict list)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(list->count != 0, "Couldn't pop from an empty list.");

    data = list->last->data;

    if (list->count == 1) {
        free(list->last);
        list->first = NULL;
        list->last = NULL;
        list->count = 0; // or --; as count gets 0
    } else {
        doubly_linked_node *new_last = list->last->prev;
        free(list->last);
        list->last = new_last;
        new_last->next = NULL;
        list->count--;
    }

error: // fall through
    return data;
}

// removes first element from a given list (freeing returned memory is your responsibility)
void *doubly_linked_shift(doubly_linked *restrict list)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(list->count != 0, "Couldn't shift from an empty list.");

    data = list->first->data;

    if (list->count == 1) {
        free(list->first);
        list->first = NULL;
        list->last = NULL;
        list->count = 0;
    } else {
        doubly_linked_node *new_first = list->first->next;
        free(list->first);
        list->first = new_first;
        new_first->prev = NULL;
        list->count--;
    }

error: // fall through
    return data;
}

// removes specific entry from list (freeing returned data is your responsibility)
void *doubly_linked_remove(doubly_linked *restrict list, void *restrict entry)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(entry != NULL, "entry is NULL.");

    doubly_linked_node *node; // initialized in below else if block

    if (list->compare(list->first->data, entry) == DOUBLY_LINKED_EQUAL) {
        data = doubly_linked_shift(list);
    } else if (list->compare(list->last->data, entry) == DOUBLY_LINKED_EQUAL) {
        data = doubly_linked_pop(list);
    } else if ((node = in_list(list, entry)) != NULL) {
        data = node->data;
        node->prev->next = node->next;
        node->next->prev = node->prev;
        free(node);
        list->count--;
    } else {
        log_err("entry %p doesn't belong to this list.", entry);
    }

error: // fall through
    return data;
}

// remove data after entry (freeing returned data is your responsibility)
void *doubly_linked_remove_after(doubly_linked *restrict list, void *restrict entry)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(entry != NULL, "entry is NULL.");
    check(list->compare(list->last->data, entry) != DOUBLY_LINKED_EQUAL, "Couldn't remove after last element.");

    doubly_linked_node *node; // initialized in below else if block

    if (list->compare(list->last->prev, entry) == DOUBLY_LINKED_EQUAL) {
        data = doubly_linked_pop(list);
    } else if ((node = in_list(list, entry)) != NULL) {
        doubly_linked_node *delete_node = node->next;
        node->next = delete_node->next;
        node->next->prev = node;

        data = delete_node->data;
        free(delete_node);
        list->count--;
    } else {
        log_err("entry %p doesn't belong to this list.", entry);
    }

error: // fall through
    return data;
}

// remove data before entry (freeing returned data is your responsibility)
void *doubly_linked_remove_before(doubly_linked *restrict list, void *restrict entry)
{
    void *data = NULL;

    check(list != NULL, "list is NULL.");
    check(entry != NULL, "Couldn't remove after NULL.");
    check(list->compare(list->first->data, entry) != DOUBLY_LINKED_EQUAL, "Couldn't remove before first element.");

    doubly_linked_node *node; // initialized in below else if block

    if (list->compare(list->first->next->data, entry) == DOUBLY_LINKED_EQUAL) {
        data = doubly_linked_shift(list);
    } else if ((node = in_list(list, entry)) != NULL) {
        doubly_linked_node *delete_node = node->prev;
        node->prev = delete_node->prev;
        node->prev->next = node;

        data = delete_node->data;
        free(delete_node);
        list->count--;
    } else {
        log_err("entry %p doesn't belong to this list.", entry);
    }

    // if (before == list->first->next) { // this code got deprecated here and in other functions too
    //     data = doubly_linked_shift(list);
    // } else {
    //     doubly_linked_iter (list, first, next, cur) {
    //         if (cur == before) {
    //             doubly_linked_node *delete_node = cur->prev;
    //             cur->prev = delete_node->prev;
    //             cur->prev->next = cur;

    //             data = delete_node->data;
    //             free(delete_node);
    //             list->count--;
    //             break;
    //         }
    //     }
    // }

error: // fall through
    return data;
}

/* joining and splitting */

// join 2 lists
int doubly_linked_join(doubly_linked *restrict *restrict list1, doubly_linked *restrict *restrict list2)
{
    check(list1 != NULL, "Address of list1 is NULL.");
    check(*list1 != NULL, "list1 is NULL.");
    check(list2 != NULL, "Address of list2 is NULL.");
    check(*list2 != NULL, "list2 is NULL.");
    check((*list1)->count != 0, "Doesn't make sense to join to an empty list.");
    check((*list2)->count != 0, "Doesn't make sense to join an empty list.");
    check((*list1)->count + (*list2)->count <= UINT32_MAX, "Can't expand past max available size, AKA UINT32_MAX.");
    check((*list1)->compare == (*list2)->compare, "Couldn't join lists of different cmp_templates");

    (*list1)->last->next = (*list2)->first;
    (*list2)->first->prev = (*list1)->last;
    (*list1)->last = (*list2)->last;
    (*list1)->count += (*list2)->count;

    free(*list2);
    *list2 = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

// this function is used in doubly_linked_split to locate doubly_linked_node * pointers
static inline int node_in_list(doubly_linked *restrict list, doubly_linked_node *restrict node)
{
    doubly_linked_iter (list, first, next, cur) {
        if (cur == node) {
            return CERB_OK;
        }
    }

    return CERB_ERR;
}

// split list from node to node (this function requires you to pass doubly_linked_node *)
doubly_linked *doubly_linked_split(doubly_linked **list, doubly_linked_node *restrict from_node,
doubly_linked_node *restrict to_node)
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

    doubly_linked *new_list = NULL;
    uint32_t count = 0;

    if (from_node == NULL) {
        check(to_node->next, "Doesn't make sense to split from first including last.");

        new_list = doubly_linked_create((*list)->compare);
        check(new_list != NULL, "Couldn't create new list.");

        new_list->first = (*list)->first;
        new_list->last = to_node;

        (*list)->first = to_node->next;
        (*list)->first->prev = NULL;
        to_node->next = NULL;
    } else if (to_node == NULL) {
        check(from_node != (*list)->first, "Doesn't make sense to split from first including last.");

        new_list = doubly_linked_create((*list)->compare);
        check(new_list != NULL, "Couldn't create new list.");

        new_list->first = from_node;
        new_list->last = (*list)->last;

        (*list)->last = from_node->prev;
        (*list)->last->next = NULL;
        from_node->prev = NULL;
    } else {

        if (from_node == (*list)->first) {
            new_list = doubly_linked_split(list, NULL, to_node);
            if (!new_list)
                goto error;
            return new_list;
        }

        new_list = doubly_linked_create((*list)->compare);
        check(new_list != NULL, "Couldn't create new list.");

        from_node->prev->next = to_node->next;
        to_node->next->prev = from_node->prev;
        from_node->prev = NULL;
        to_node->next = NULL;

        new_list->first = from_node;
        new_list->last = to_node;
    }

    doubly_linked_node *count_node = new_list->first;
    while (count_node) {
        count_node = count_node->next;
        count++;
    }

    new_list->count = count;
    (*list)->count = (*list)->count - count;

    return new_list;

error:
    return NULL;
}

/* clear and destroy operations */

int doubly_linked_clear(doubly_linked *list)
{
    check(list != NULL, "Somehow got list that is NULL.");

    doubly_linked_iter(list, first, next, cur) {
        free(cur->data);
        cur->data = NULL;
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

// free nodes, data and struct itself. pass by reference to make it NULL, so no access after destroyed
int doubly_linked_destroy(doubly_linked *restrict *restrict list, doubly_linked_dealloc dealloc_cb)
{
    check(list != NULL, "Address of list is NULL.");
    check(*list != NULL, "list is NULL.");

    doubly_linked_node *first = (*list)->first;
    doubly_linked_node *freeable = NULL;
    for (; first != NULL;) {
        freeable = first;
        first = first->next;
        dealloc_cb(freeable->data);
        free(freeable);
    }
    free(*list);
    *(list) = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int doubly_linked_clear_destroy(doubly_linked **list)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    if (!(*list)->count)
        goto empty_list; // if list is empty skip the loop

    doubly_linked_node *cur_node = (*list)->first;
    doubly_linked_node *next_node = NULL;

    for (next_node = cur_node->next; next_node != NULL; next_node = cur_node->next)
    {
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

int doubly_linked_free_complex_data(doubly_linked **list, doubly_linked_dealloc handler_func)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    doubly_linked_node *first = (*list)->first;
    doubly_linked_node *freeable = NULL;
    for (; first != NULL;)
    {
        freeable = first;
        first = first->next;
        handler_func(freeable->data);
        free(freeable);
    }
    free(*(list));
    *(list) = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int doubly_linked_free_list(doubly_linked **list)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    doubly_linked_iter(*list, first, next, cur)
    {
        free(cur);
    }

    free(*list);

    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

/* sorted insertion */

int doubly_linked_sorted_insert(doubly_linked *list, void *data)
{
    int rc = -1;

    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");

    doubly_linked_iter(list, first, next, cur)
    {
        if (list->compare(cur->data, data) > 0)
        {
            rc = doubly_linked_insert_before(list, cur, data);
            check(rc != CERB_ERR, "Failed to sorted_insert.");
            return rc;
        }
    }
    // if all insert_before attampts fail it means that we have to insert at the end
    rc = doubly_linked_push(list, data);
    check(rc != CERB_ERR, "Failed to sorted_insert.");

error:
    return rc;
}