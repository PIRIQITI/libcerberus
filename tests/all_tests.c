#include "minunit.h"
#include "../src/dbg.h"
#include "../src/singly_linked.h"
#include "../src/doubly_linked.h"
#include "../src/d_array.h"
#include "../src/hashmap.h"
#include "../src/heap.h"
#include "../src/priority_queue.h"
#include "../src/binary_search_tree.h"
#include "../src/stack.h"
#include "../src/queue.h"
#include <string.h>


void handler_func_ALL(void *data)
{
    free(data);
}

void nofree_cb(void *data)
{
    return;
}

// default compare type is integer
int cmp_func_ALL(const void *const restrict data1, const void *const restrict data2)
{
    if ( *( (int *) data1 ) < *( (int *) data2 ) ) {
        return -1;
    } else if ( *( (int *) data1 ) > *( (int *) data2 ) ) {
        return 1;
    } else {
        return 0;
    }
}



// data for test cases

int *get_test_data()
{
    int *data = malloc(sizeof(int));

    check_mem(data);

    return data;

error:
    return NULL;
}


singly_linked *s_linked = NULL;
doubly_linked *d_linked = NULL;
d_array *array = NULL;
Hashmap *map = NULL;
Heap *heap = NULL;
priority_queue *p_queue = NULL;
BS_tree *b_tree = NULL;
Stack *stack = NULL;
Queue *queue = NULL;


int *test1 = NULL;
int *test2 = NULL;
int *test3 = NULL;
int *test4 = NULL;
int *test5 = NULL;

int rc = 0;
void *data = NULL;

// test singly linked list

int create_test_data()
{
    test1 = get_test_data();
    test2 = get_test_data();
    test3 = get_test_data();
    test4 = get_test_data();
    test5 = get_test_data();

    if (test1 == NULL || test2 == NULL || test3 == NULL || test4 == NULL || test5 == NULL) return -1;

    *test1 = 2467;
    *test2 = 67;
    *test3 = 323666;
    *test4 = 6496;
    *test5 = 139;

    return 0;
}

// test singly linked list

char *test_create_SL()
{
    s_linked = singly_linked_create(NULL);
    mu_assert(s_linked != NULL, "Failed to create list.");

    return NULL;
}

char *test_push_SL()
{
    rc = singly_linked_push(s_linked, test1);
    mu_assert(rc != CERB_ERR, "push failed.");

    rc = singly_linked_push(s_linked, test2);
    mu_assert(rc != CERB_ERR, "push failed.");

    mu_assert(s_linked->count == 2, "wrong count on push.");

    return NULL;
}

char *test_pop_SL()
{
    data = singly_linked_pop(s_linked);
    mu_assert(data != NULL, "pop failed.");

    mu_assert(s_linked->count == 1, "wrong count on pop.");

    return NULL;
}

char *test_remove_SL()
{
    data = singly_linked_remove(s_linked, s_linked->first->data);
    mu_assert(data != NULL, "remove failed.");

    mu_assert(s_linked->count == 0, "wrong count on remove.");

    return NULL;
}

char *test_free_list_SL()
{
    // rc = SLinked_free_list(&s_linked);
    rc = singly_linked_destroy(&s_linked, nofree_cb);
    mu_assert(rc != CERB_ERR, "error while freeing.");

    return NULL;
}

// test doubly linked list

char *test_create_DL()
{
    d_linked = doubly_linked_create(NULL);
    mu_assert(d_linked != NULL, "Failed to create list.");

    return NULL;
}

char *test_push_DL()
{
    rc = doubly_linked_push(d_linked, test1);
    mu_assert(rc != CERB_ERR, "push failed.");

    rc = doubly_linked_push(d_linked, test2);
    mu_assert(rc != CERB_ERR, "push failed.");

    mu_assert(d_linked->count == 2, "wrong count on push.");

    return NULL;
}

char *test_pop_DL()
{
    data = doubly_linked_pop(d_linked);
    mu_assert(data != NULL, "pop failed.");

    mu_assert(d_linked->count == 1, "wrong count on pop.");

    return NULL;
}

char *test_remove_DL()
{
    data = doubly_linked_remove(d_linked, d_linked->first->data);
    mu_assert(data != NULL, "remove failed.");

    mu_assert(d_linked->count == 0, "wrong count on remove.");

    return NULL;
}

char *test_free_list_DL()
{
    // rc = DLinked_free_list(&d_linked);
    rc = doubly_linked_destroy(&d_linked, nofree_cb);
    mu_assert(rc != CERB_ERR, "error while freeing.");

    return NULL;
}

// test dynamic array

char *test_create_DA()
{
    array = d_array_create(NULL, 500);
    mu_assert(array != NULL, "failed to create array.");

    return NULL;
}

char *test_push_DA()
{
    rc = d_array_push_back(array, test1);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(array->length == 1, "wrong count on push.");

    d_array_push_back(array, test2);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(array->length == 2, "wrong count on push.");
    
    d_array_push_back(array, test3);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(array->length == 3, "wrong count on push.");
    
    d_array_push_back(array, test4);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(array->length == 4, "wrong count on push.");

    d_array_push_back(array, test5);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(array->length == 5, "wrong count on push.");

    return NULL;
}

char *test_pop_DA()
{
    data = d_array_pop_back(array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(array->length == 4, "wrong count on pop.");

    data = d_array_pop_back(array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(array->length == 3, "wrong count on pop.");

    data = d_array_pop_back(array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(array->length == 2, "wrong count on pop.");

    data = d_array_pop_back(array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(array->length == 1, "wrong count on pop.");

    return NULL;
}

char *test_free_array_DA()
{
    rc = d_array_destroy(&array, nofree_cb);
    mu_assert(rc != CERB_ERR, "error while freeing.");

    return NULL;
}

// test hashmap

char *test_create_HM()
{
    map = Hashmap_create(NULL, NULL, 0);
    mu_assert(map != NULL, "failed to create map.");

    return NULL;
}

char *test_set_HM()
{
    // rc = Hashmap_set(map, test1, test1, MAKE_SORTED | ALLOW_DUPLICATES);
    rc = Hashmap_insert(map, test1);
    mu_assert(rc != CERB_ERR, "failed to set.");

    // rc = Hashmap_set(map, test2, test2, MAKE_SORTED | ALLOW_DUPLICATES);
    rc = Hashmap_insert(map, test2);
    mu_assert(rc != CERB_ERR, "failed to set.");

    // rc = Hashmap_set(map, test3, test3, MAKE_SORTED | ALLOW_DUPLICATES);
    rc = Hashmap_insert(map, test3);
    mu_assert(rc != CERB_ERR, "failed to set.");

    // rc = Hashmap_set(map, test4, test4, MAKE_SORTED | ALLOW_DUPLICATES);
    rc = Hashmap_insert(map, test4);
    mu_assert(rc != CERB_ERR, "failed to set.");

    // rc = Hashmap_set(map, test5, test5, MAKE_SORTED | ALLOW_DUPLICATES);
    rc = Hashmap_insert(map, test5);
    mu_assert(rc != CERB_ERR, "failed to set.");

    return NULL;
}

char *test_delete_HM()
{
    data = Hashmap_remove(map, test3);
    mu_assert(data != NULL, "failed to delete.");
    // free(data);

    data = Hashmap_remove(map, test5);
    mu_assert(data != NULL, "failed to delete.");
    // free(data);

    data = Hashmap_remove(map, test1);
    mu_assert(data != NULL, "failed to delete.");
    // free(data);

    return NULL;
}

char *test_free_complex_data_HM()
{
    rc = Hashmap_destroy(&map, nofree_cb);
    mu_assert(rc != CERB_ERR, "failed to free.");

    return NULL;
}

// test heap

char *test_create_H()
{
    heap = heap_create(NULL, 500);
    mu_assert(heap != NULL, "failed to create heap.");

    return NULL;
}

char *test_insert_H()
{
    rc = heap_insert(heap, test1);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = heap_insert(heap, test2);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = heap_insert(heap, test3);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = heap_insert(heap, test4);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = heap_insert(heap, test5);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    return NULL;
}

char *test_delete_H()
{
    data = heap_pop(heap);
    mu_assert(data != NULL, "failed to pop.");

    data = heap_pop(heap);
    mu_assert(data != NULL, "failed to pop.");

    data = heap_pop(heap);
    mu_assert(data != NULL, "failed to pop.");

    return NULL;
}

char *test_free_complex_data_H()
{
    rc = heap_destroy(&heap, nofree_cb);
    mu_assert(rc != CERB_ERR, "failed to free.");

    return NULL;
}

// test binary search tree

char *test_create_BS()
{
    b_tree = BS_tree_create(NULL);
    mu_assert(b_tree != NULL, "failed to create map.");

    return NULL;
}

char *test_insert_BS()
{
    rc = BS_tree_insert(b_tree, test1);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = BS_tree_insert(b_tree, test2);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = BS_tree_insert(b_tree, test3);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = BS_tree_insert(b_tree, test4);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    rc = BS_tree_insert(b_tree, test5);
    mu_assert(rc != CERB_ERR, "failed to insert.");

    return NULL;
}

// free data (test4, test1, test2) in this function not to lose pointers to them
char *test_delete_BS()
{
    data = BS_tree_delete(b_tree, test4);
    mu_assert(data != NULL, "failed to delete.");
    handler_func_ALL(data);

    data = BS_tree_delete(b_tree, test1);
    mu_assert(data != NULL, "failed to delete.");
    handler_func_ALL(data);

    data = BS_tree_delete(b_tree, test2);
    mu_assert(data != NULL, "failed to delete.");
    handler_func_ALL(data);
    
    return NULL;
}

// free leftover data too in this function
char *test_free_complex_data_BS()
{
    rc = BS_tree_destroy(&b_tree, handler_func_ALL);
    mu_assert(rc != CERB_ERR, "failed to free.");

    return NULL;
}

// =========================================================================================================

// run all tests

char *all_tests()
{
    rc = create_test_data();
    if (rc == -1) return "Failed to create test data.";

    mu_suite_start();

    mu_run_test(test_create_SL);
    mu_run_test(test_push_SL);
    mu_run_test(test_pop_SL);
    mu_run_test(test_remove_SL);
    mu_run_test(test_free_list_SL);

    mu_run_test(test_create_DL);
    mu_run_test(test_push_DL);
    mu_run_test(test_pop_DL);
    mu_run_test(test_remove_DL);
    mu_run_test(test_free_list_DL);

    mu_run_test(test_create_DA);
    mu_run_test(test_push_DA);
    mu_run_test(test_pop_DA);
    mu_run_test(test_free_array_DA);

    mu_run_test(test_create_HM);
    mu_run_test(test_set_HM);
    mu_run_test(test_delete_HM);
    mu_run_test(test_free_complex_data_HM);

    mu_run_test(test_create_H);
    mu_run_test(test_insert_H);
    mu_run_test(test_delete_H);
    mu_run_test(test_free_complex_data_H);

    mu_run_test(test_create_BS);
    mu_run_test(test_insert_BS);
    mu_run_test(test_delete_BS);
    mu_run_test(test_free_complex_data_BS);

    return NULL;
}

RUN_TESTS(all_tests);