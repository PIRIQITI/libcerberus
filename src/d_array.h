#ifndef E9987CBD_81B9_420E_931B_13867F04AC33
#define E9987CBD_81B9_420E_931B_13867F04AC33

#include <stdint.h>
// for d_array_print
#include <stdio.h>
#include "dbg.h"

#define CERB_OK  0
#define CERB_ERR -1

#define D_ARRAY_GREATER 1
#define D_ARRAY_LESS -1
#define D_ARRAY_EQUAL 0

#define D_ARRAY_DEFAULT_EXPAND_RATE 100

// get the index of last element of array
#define d_array_index_of_last(array_ptr) ((array_ptr)->length - 1)
// get the data at [0] (NULL if length is 0)
#define d_array_at_zero(array_ptr) ((array_ptr)->length ? (array_ptr)->data[0] : NULL)
// get the data at last position of array
#define d_array_at_last(array_ptr) ((array_ptr)->length ? (array_ptr)->data[d_array_index_of_last(array_ptr)] : NULL)
// get array capacity
#define d_array_capacity(array_ptr) ((array_ptr)->capacity)
// get array length
#define d_array_length(array_ptr) ((array_ptr)->length)
// get array expand rate
#define d_array_expand_rate(array_ptr) ((array_ptr)->expand_rate)

// new_expand_rate should be in range of 1 to UINT16_MAX.
#define d_array_set_expand_rate(array_ptr, new_expand_rate) ((array_ptr)->expand_rate = (new_expand_rate) ? (new_expand_rate) : 1)

// format is "%s", "%c", "%d", "%f" etc...  data_cb (which is a function) returns data which matches format
#define d_array_print(array_ptr, data_cb, format)\
        if(array_ptr){\
        printf("[ ");uint32_t i=0;for(;i<(int64_t)(array_ptr)->length-1;i++){printf(format", ",data_cb((array_ptr)->data[i]));}\
        if((array_ptr)->length){printf(format,data_cb((array_ptr)->data[i]));}printf(" ]\n");}\
        else{log_err("array is NULL.");}

// ==============================================================================================================================

// compare function pointer for d_array (note consts and restrict. your function has to follow the rules imposed by those)
typedef int (*d_array_cmp) (const void *const restrict data1, const void *const restrict data2);

typedef struct d_array {
    d_array_cmp compare; // compare callback
    void **data; // array of pointers to data
    uint32_t capacity; // capacity of d_array
    uint32_t length; // length of d_array
    uint16_t expand_rate; // expand rate of d_array
} d_array;

// set specific [position] to data in array
static inline int d_array_set(d_array *restrict array, uint32_t position, void *restrict data)
{
    check(array != NULL, "array is NULL.");
    check(position < d_array_length(array), "Invalid position.");
    check(data != NULL, "data is NULL.");

    array->data[position] = data; // set data

    return CERB_OK;

error:
    return CERB_ERR;
}

// get data at specific [position] from array
static inline void *d_array_get(d_array *restrict array, uint32_t position)
{
    check(array != NULL, "array is NULL.");
    check(position < d_array_length(array), "Invalid position.");
    
    return array->data[position]; // return data at [position]

error:
    return NULL;
}

// get index of data in array (-1 if it doesn't exist)
static inline int64_t d_array_index_of(d_array *restrict array, void *restrict data)
{
    check(array != NULL, "array is NULL.");
    check(data != NULL, "data is NULL.");
    
    uint32_t i;
    for (i = 0; i < d_array_length(array); i++) {
        if (array->compare(array->data[i], data) == 0) return i;
    }

error: // fall through
    return CERB_ERR;
}

// create d_array
d_array *d_array_create(d_array_cmp compare, uint32_t initial_capacity);

// push data at the back of d_array
int d_array_push_back(d_array *array, void *data);
// push data at the front of d_array
int d_array_push_front(d_array *array, void *data);

// pop data from the back of an array and return it (deallocating returned data is your resposibility)
void *d_array_pop_back(d_array *array);
// pop data from the front of an array and return it (deallocating returned data is your resposibility)
void *d_array_pop_front(d_array *array);

// insert data at a specific [position] of d_array
int d_array_insert_at(d_array *array, uint32_t position, void *data);
// remove data from a specific [position] of d_array
void *d_array_remove_at(d_array *array, uint32_t position);

// expand array to new_capacity
int d_array_expand(d_array *array, uint32_t new_capacity);
// shrink array to exactly fit the current length
int d_array_shrink_to_fit(d_array *array);

// join 2 arrays, deallocating array2 and it's data block AKA array->data (you need to provide reference to those arrays)
d_array *d_array_join(d_array **array1, d_array *restrict *restrict array2);
// split one array into 2, returning splitted portion
d_array *d_array_split(d_array **array1, uint32_t from_position, uint32_t to_position);

typedef int (*d_array_traverse_cb) (void *data); // traverse callback for d_array (see d_array_traverse for more info)

// traverses through d_array and applies your traverse_cb to every element it has
int d_array_traverse(d_array *array, d_array_traverse_cb traverse_cb);

// deallocate callback for d_array (if your d_array contains structs containing other allocated data for example, you need this)
typedef void (*d_array_dealloc) (void *data);

// equivalent to d_array_remove_at, except that it deallocates data at [position]
int d_array_delete_at(d_array *array, uint32_t position, d_array_dealloc dealloc_cb);
// deletes first match of data in array and deallocates it too
int d_array_delete(d_array *array, void *data, d_array_dealloc dealloc_cb);
// destroys an entire d_array and sets it to NULL on stack (you need to provide reference to d_array)
int d_array_destroy(d_array *restrict *restrict array, d_array_dealloc dealloc_cb);
// deallocate all currently used data in slots, but leave the slots ( memory block AKA array->data ) allocated
int d_array_clear(d_array *restrict array, d_array_dealloc dealloc_cb);

#endif /* E9987CBD_81B9_420E_931B_13867F04AC33 */