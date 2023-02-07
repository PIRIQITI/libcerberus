#ifndef CFB2ECC9_5B49_46D7_B2E6_3D43B3A19FFF
#define CFB2ECC9_5B49_46D7_B2E6_3D43B3A19FFF

#include "d_array.h"

#define HEAP_GREATER 1
#define HEAP_LESS -1
#define HEAP_EQUAL 0

// get the index of last element of array
#define heap_index_of_last(heap) d_array_index_of_last(heap)
// get a left child's parent node index ( 2 * i + 1 = self_index )
#define heap_index_of_parent_of_left(self_index) ( ((self_index) - 1) / 2 )
// get a right child's parent node index ( 2 * i + 1 = self_index )
#define heap_index_of_parent_of_right(self_index) ( ((self_index) - 2) / 2 )
// get an index of left child of some parent ( 2 * self_index + 1 = i )
#define heap_index_of_left_child_of_parent(self_index) ( 2 * (self_index) + 1 )
// get an index of right child of some parent ( 2 * self_index + 2 = i )
#define heap_index_of_right_child_of_parent(self_index) ( 2 * (self_index) + 2 )
// get a left or right child's parent index ( index % 2 == 0 ? |it is right child| : |it is left child| )
#define parent(index) ((index) % 2 == 0 ? heap_index_of_parent_of_right(index) : heap_index_of_parent_of_left(index))

// Heap is the same as d_array, only functions handle it differently
// be careful not to call d_array functions on heap, as some of them might alter heap invariant
// heap must have a valid compare function (see default compare for d_array)
#define Heap d_array

// create is the same as d_array_create (see d_array_create for details)
static inline Heap *heap_create(d_array_cmp compare, uint32_t initial_capacity)
{
    return d_array_create(compare, initial_capacity);
}

// deallocate the whole heap
static inline int heap_destroy(Heap **heap, d_array_dealloc dealloc_cb)
{
    return d_array_destroy(heap, dealloc_cb);
}

// returns length of heap. if heap is NULL, returns -1
static inline int heap_is_empty(Heap *restrict heap)
{
    check(heap != NULL, "heap is NULL.");

    return d_array_length(heap);

error:
    return CERB_ERR;
}

// returns 0-th (highest priority) element of heap
static inline void *heap_find_max_index(Heap *restrict heap)
{
    check(heap != NULL, "heap is NULL.");
    check(d_array_length(heap) != 0, "heap is empty.");

    return heap->data[0]; // max_index-item returned

error:
    return NULL;
}

// pop 0-th (max_index) element from heap and return (satisfy heap invariant automatically after pop)
void *heap_pop(Heap *heap);

// pop and extract_max_index does the same thing
static inline void *heap_extract_max_index(Heap *heap)
{
    return heap_pop(heap);
}

// pop and heap_delete_max_index does the same thing, only this function deallocates popped data too
// dealloc callback needed to free data (see d_array's dealloc_cb for details)
int heap_delete_max_index(Heap *heap, d_array_dealloc dealloc_cb);
// insert in heap as in array (satisfy heap invariant automatically after insertion)
int heap_insert(Heap *heap, void *data);
// replace root with some other value (satisfy heap invariant automatically after pop)
int heap_replace(Heap *restrict heap, void *restrict new_data, d_array_dealloc dealloc_cb);

// heapify array, return new heap and set old array reference to NULL (this needs an address of array)
Heap *heapify(d_array *restrict *restrict array);
// meld 2 heaps. preserves original heaps by memcopying (this doesn't create copies of actual data, just copies pointers)
// (if you deallocate data held by either of these 3 heaps the remaining heaps won't hold valid data)
Heap *heap_meld(Heap *restrict *restrict heap1, Heap *restrict *restrict heap2);

// merge 2 heaps. doesn't preserve original heaps
// (deallocation is only needed on returned heap not the original two)
static inline Heap *heap_merge(Heap **heap1, Heap *restrict *restrict heap2)
{
    // join them as regular arrays (all safety checks are in d_array_join. see function for details)
    d_array *joined = d_array_join(heap1, heap2);
    // if join was successful heapify the result of it. else return NULL
    if (joined) return heapify(&joined);
    else return NULL;
}

#endif /* CFB2ECC9_5B49_46D7_B2E6_3D43B3A19FFF */