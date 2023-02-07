#include <stdlib.h>
#include "heap.h"

// swap data on two different positions in heap (internally used by emerge and sink functions)
#define HEAP_SWAP(heap, position1, position2)\
            void *_data1 = (heap)->data[(position1)];\
            (heap)->data[(position1)] = (heap)->data[(position2)];\
            (heap)->data[(position2)] = _data1;

// emerge node from a bottom towards a root until needed
static void emerge(Heap *restrict heap, uint32_t node_position)
{
    // node_position is current node's position we are adjusting
    uint32_t parent_index = parent(node_position); // determine parent of node

    // loop until we have reached root or until invariant is corrupted
    while (node_position != 0 && heap->compare(heap->data[node_position], heap->data[parent_index]) == HEAP_GREATER) {
        HEAP_SWAP(heap, node_position, parent_index); // swap child and parent
        node_position = parent_index; // update swapped child's position
        parent_index = parent(node_position); // calculate new index for swapped child
    }
}

// sink node from up towards bottom until needed
static void sink(Heap *restrict heap, uint32_t node_position)
{ // node_position is current node's position we are adjusting and we assume it holds max value
    uint64_t max_index = node_position;

    while (1) {
        // make this uint64_t to prevent overflow (position miscalculation)
        uint64_t left_child_index = heap_index_of_left_child_of_parent(node_position);
        
        // check to see if we are going out of bounds with left node and break if so
        if (left_child_index > heap_index_of_last(heap)) break;

        // compare current node with left node and pick maximum out of those
        if (heap->compare(heap->data[node_position], heap->data[left_child_index]) == HEAP_LESS) {
            max_index = left_child_index;
        }
        // make this uint64_t to prevent overflow (position miscalculation)
        uint64_t right_child_index = heap_index_of_right_child_of_parent(node_position);

        // check to see if we are going out of bounds with right node and if not so
        if (right_child_index <= heap_index_of_last(heap) && // compare current maximum with right node and pick maximum again
        heap->compare(heap->data[max_index], heap->data[right_child_index]) == HEAP_LESS) {
            max_index = right_child_index;
        }

        if (max_index == node_position) break; // if max index hasn't changed at this point, heap invariant is satisfied

        // swap current position of node and max_index (determined above)
        HEAP_SWAP(heap, node_position, max_index);
        node_position = max_index; // update node_position to swapped index
    }
}

// insert in heap as in array (satisfy heap invariant automatically after insertion)
int heap_insert(Heap *heap, void *data)
{
    check(heap != NULL, "heap is NULL.");
    check(data != NULL, "data is NULL.");
    
    // push new element at the back
    int rc = d_array_push_back(heap, data);
    check(rc != CERB_ERR, "Failed to add new value to heap.");

    // if we don't have more than a (root) node, no need to adjust
    if (d_array_length(heap) != 1) {
        emerge(heap, d_array_length(heap) - 1);
    }
// make it not to allow duplicat entries (this would require to maintain a map of heap entries for efficiency)
    return CERB_OK;

error:
    return CERB_ERR;
}
// static int data_cb(void *data) {return *(int *)data;}
// pop 0-th (max_index) element from heap and return (satisfy heap invariant automatically after pop)
void *heap_pop(Heap *heap)
{
    check(heap != NULL, "heap is NULL.");
    check(d_array_length(heap) != 0, "heap is empty.");

    if (d_array_length(heap) < 3) { // if length is 0 we return 0. if length is 2 we return [0] and replacement is automatic
        return d_array_pop_front(heap);
    } else { // greater than 3 needs adjustments for heap invariant
        void *max_index = heap->data[0]; // save max_index as its being overwritten
        // heap->data[0] = d_array_pop_back(heap); // pop last element from array and assign it to root
        // debug("max_index = %d", *(int *)max_index);
        void *new_root = d_array_pop_back(heap);
        heap->data[0] = new_root;
        // debug("temp = %d", *(int *)temp);
        // d_array_print(heap, data_cb, "%d");
        // debug("(in heap) data = %d", *(int *)heap->data[0]);
        sink(heap, 0); // adjust to satisfy heap invariant
        return max_index; // return value held by root
    }

error:
    return NULL;
}

// replace root with some other value (satisfy heap invariant automatically after pop)
int heap_replace(Heap *restrict heap, void *restrict new_data, d_array_dealloc dealloc_cb)
{
    check(heap != NULL, "heap is NULL.");
    check(new_data != NULL, "new_data is NULL.");
    check(dealloc_cb != NULL, "dealloc_cb is NULL.");

    // free [0]
    dealloc_cb(heap->data[0]);

    // replace with new_data
    heap->data[0] = new_data;

    // adjust to satisfy heap invariant
    sink(heap, 0);

    return CERB_OK;

error:
    return CERB_ERR;
}

// pop and heap_delete_max_index does the same thing, only this function deallocates popped data too
// dealloc callback needed to free data (see d_array's dealloc_cb for details)
int heap_delete_max_index(Heap *heap, d_array_dealloc dealloc_cb)
{
    check(dealloc_cb != NULL, "dealloc_cb is NULL.");

    // first, pop the first element
    void *data = heap_pop(heap);
    if (data == NULL) return CERB_ERR;
    
    // then free it's storage
    dealloc_cb(heap_pop(heap));

    return CERB_OK;

error:
    return CERB_ERR;
}

// heapify array, return new heap and set old array reference to NULL (this needs an address of array)
Heap *heapify(d_array *restrict *restrict array)
{
    check(array != NULL, "Address of array is NULL.");
    check(*array != NULL, "array is NULL.");
    check(d_array_length(*array) > 1, "Doesn't make sense to heapify array of length <= 1.");

    // last non-leaf node
    uint32_t node_pos = parent(heap_index_of_last(*array));

    // emerge every node after last non-leaf node starting from lowest indexed leaf node
    uint32_t i;
    for (i = node_pos + 1; i < d_array_length(*array); i++) {
        emerge(*array, i);
    }

    // emerge and sink (depending which one is required) the rest of the nodes starting from root
    for (i = 0; i < node_pos + 1; i++) {
        emerge(*array, i);
        sink(*array, i);
    }

    Heap *new_heap = *array; // save heapified array
    *array = NULL; // set old array on stack to NULL

    return new_heap; // and return heapified array

error:
    return NULL;
}

// meld 2 heaps. preserves original heaps by memcopying (this doesn't create copies of actual data, just copies pointers)
// (if you deallocate data held by either of these 3 heaps the remaining heaps won't hold valid data)
Heap *heap_meld(Heap *restrict *restrict heap1, Heap *restrict *restrict heap2)
{
    check(heap1 != NULL, "Address of heap1 is NULL.");
    check(*heap1 != NULL, "heap1 is NULL.");
    check(heap2 != NULL, "Address of heap2 is NULL.");
    check(*heap2 != NULL, "heap2 is NULL.");
    check(d_array_length(*heap1) != 0, "heap1 is empty.");
    check(d_array_length(*heap2) != 0, "heap2 is empty.");
    check((uint64_t) d_array_length(*heap1) + d_array_length(*heap2) <= UINT32_MAX,
    "Can't expand past max available size, AKA UINT32_MAX.");
    check((*heap1)->compare == (*heap2)->compare, "heap1 compare and heap2 compare don't match.");

    d_array *new_array = d_array_create((*heap1)->compare, d_array_length(*heap1) + d_array_length(*heap2));

    // copy memory from heap1 to new_array
    memcpy(new_array->data, (*heap1)->data, d_array_length(*heap1) * sizeof(void *));
    new_array->length = d_array_length(*heap1); // set length

    // copy memory from heap2 to new_array
    memcpy(new_array->data + d_array_length(new_array), (*heap2)->data, d_array_length(*heap2) * sizeof(void *));
    new_array->length += d_array_length(*heap2); // set length

    Heap *new_heap = heapify(&new_array); // heapify is needed

    return new_heap;

error:
    return NULL;
}