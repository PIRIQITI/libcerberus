#include <stdlib.h>
#include "d_array.h"

// default compare type is integer
int default_compare(const void *const restrict data1, const void *const restrict data2)
{
    if ( *( (int *) data1 ) < *( (int *) data2 ) ) {
        return D_ARRAY_LESS;
    } else if ( *( (int *) data1 ) > *( (int *) data2 ) ) {
        return D_ARRAY_GREATER;
    } else {
        return D_ARRAY_EQUAL;
    }
}

d_array *d_array_create(d_array_cmp compare, uint32_t initial_capacity)
{
    d_array *array = malloc(sizeof(struct d_array));
    check_mem(array); // allocate memory for d_array struct and check it

    array->data = malloc(sizeof(void *) * initial_capacity);
    check_mem(array->data); // allocate memory for array->data and check it

    // if user doesn't define compare (otherwise it is NULL), then default one is set (see it above)
    array->compare = compare ? compare : default_compare;

    array->capacity = initial_capacity;
    array->length = 0;
    array->expand_rate = D_ARRAY_DEFAULT_EXPAND_RATE; // expand rate is default unless user changes it

    return array;

error:
    if (array) free(array);
    return NULL;
}

// internally used to manage growing and shrinking memory block
static int d_array_resize(d_array *restrict array, size_t new_size)
{
    // if space is not needed, deallocate and set data = NULL
    if (new_size == 0) {
        free(array->data);
        array->data = NULL;
        array->length = 0;
        array->capacity = 0;
        return CERB_OK;
    }

    void **new_data = (void **) realloc(array->data, sizeof(void *) * new_size);
    check_mem(new_data); // allocate new space and check it

    array->data = new_data; // set to newly allocated block only if execution goes past check_mem()

    return CERB_OK;

error:
    return CERB_ERR;
}

// internally used to manage inserting and removing
static inline int d_array_grow(d_array *array)
{
    int rc = d_array_resize(array, d_array_capacity(array) + d_array_expand_rate(array));
    check(rc != CERB_ERR, "Failed to grow array in size, new elements can't be added.");

    array->capacity += d_array_expand_rate(array); // we are growing by expand_rate

error: // fall through
    return rc;
}

// internally used to manage inserting and removing
static inline int d_array_shrink(d_array *array)
{
    // capacity - expand_rate doesn't cause underflow (caller functions handle that)
    int rc = d_array_resize(array, d_array_capacity(array) - d_array_expand_rate(array));
    check(rc != CERB_ERR, "Failed to contract.");

    // if length has decreased to 0 we don't subtract anything, else we subtract expand_rate
    array->capacity -= d_array_capacity(array) ? d_array_expand_rate(array) : 0;

error: // fall through
    return rc;
}

// expand array to new_capacity
int d_array_expand(d_array *array, uint32_t new_capacity)
{
    check(array != NULL, "array is NULL.");
    check(new_capacity > d_array_capacity(array), "new_capacity is less than or equal to the existing capacity.");

    int rc = d_array_resize(array, new_capacity);
    check(rc != CERB_ERR, "Failed to expand.");

    array->capacity = new_capacity; // update the capacity to new_capacity if resize is successful

    return CERB_OK;

error:
    return CERB_ERR;
}

// shrink array to exactly fit the current length
int d_array_shrink_to_fit(d_array *array)
{
    check(array != NULL, "array is NULL.");
    check(d_array_length(array) != d_array_capacity(array), "Matching size already.");
    
    int rc = d_array_resize(array, d_array_length(array));
    check(rc != CERB_ERR, "Failed to shrink.");

    array->capacity = array->length; // update capacity to exactly fit the quantity of elements if resize is successful

    return CERB_OK;

error:
    return CERB_ERR;
}

// push at the end ( at [array->length] )
int d_array_push_back(d_array *array, void *data)
{
    check(array != NULL, "array is NULL.");
    check(data != NULL, "data is NULL.");

    // if length and capacity are the same it is time to grow
    if (d_array_length(array) == d_array_capacity(array)) {
        // cast to unsigned 64-bit to prevent overflow
        if ( (uint64_t) d_array_capacity(array) + d_array_expand_rate(array) > UINT32_MAX ) {
            // check if max length has been reached
            log_err("Can't expand past max available size, AKA UINT32_MAX.");
            return CERB_ERR;
        } else {
            int rc = d_array_grow(array); // grow array
            if (rc == CERB_ERR) return CERB_ERR; // if growing fails, don't insert anything and return error
        }
    }

    array->data[d_array_length(array)] = data; // add data at the end
    array->length++; // increment length

    return CERB_OK;

error:
    return CERB_ERR;
}

// push at [0], making index of each element after [0] 1 higher
int d_array_push_front(d_array *array, void *data)
{
    check(array != NULL, "array is NULL.");
    check(data != NULL, "data is NULL.");

    // if length and capacity are the same it is time to grow
    if (d_array_length(array) == d_array_capacity(array)) {
        // cast to unsigned 64-bit to prevent overflow
        if ( (uint64_t) d_array_capacity(array) + d_array_expand_rate(array) > UINT32_MAX ) {
            // check if max length has been reached
            log_err("Can't expand past max available size, AKA UINT32_MAX.");
            return CERB_ERR;
        } else {
            int rc = d_array_grow(array); // grow array
            if (rc == CERB_ERR) return CERB_ERR; // if growing fails, don't insert anything and return error
        }
    }

    uint32_t i; // take each element up one location making room at [0]
    for (i = d_array_length(array); i > 0; i--) {
        array->data[i] = array->data[i - 1];
    }
    array->data[0] = data; // insert new element at [0]
    array->length++; // and increment length

    return CERB_OK;

error:
    return CERB_ERR;
}
// static int data_cb(void *data) {return *(int *)data;}
// don't forget deallocating returned memory
void *d_array_pop_back(d_array *array)
{
    check(array != NULL, "array is NULL.");
    check(d_array_length(array), "array is empty.");

    /* not freeing or setting popped element to NULL, as it will be overwritten by another element */
    void *data = array->data[d_array_length(array) - 1];
    array->length--; // decrease length
    // d_array_print(array, data_cb, "%d");
    // debug("data = %d", *(int *)data);

    // cast to signed 64-bit to prevent underflow. can be shrinked down to zero with >=
    if ( (int64_t) d_array_capacity(array) - d_array_expand_rate(array) >= d_array_length(array) ) {
        d_array_shrink(array);
    }
// debug("data = %d", *(int *)data);
    return data; // return the data we took earlier

error:
    return NULL;
}

// don't forget deallocating returned memory
void *d_array_pop_front(d_array *array)
{
    check(array != NULL, "array is NULL.");
    check(d_array_length(array), "array is empty.");

    // not freeing or setting popped element to NULL, as it will be overwritten by another element
    void *data = array->data[0];
    array->length--; // decrease length

    uint32_t i; // take each element to it's front location, disappearing data at [0]
    for (i = 0; i < d_array_length(array); i++) {
        array->data[i] = array->data[i + 1];
    }

    // cast to signed 64-bit to prevent underflow. can be shrinked down to zero with >=
    if ( (int64_t) d_array_capacity(array) - d_array_expand_rate(array) >= d_array_length(array) ) {
        d_array_shrink(array);
    }

    return data; // return the data we took earlier

error:
    return NULL;
}

// insert at an already existing array position, making index of each element after [posittion] 1 higher
int d_array_insert_at(d_array *array, uint32_t position, void *data)
{
    check(array != NULL, "array is NULL.");
    check(position < d_array_length(array), "Invalid position.");
    check(data != NULL, "data is NULL.");

    // if length and capacity are the same it is time to grow
    if (d_array_length(array) == d_array_capacity(array)) {
        // cast to unsigned 64-bit to prevent overflow
        if ( (uint64_t) d_array_capacity(array) + d_array_expand_rate(array) > UINT32_MAX ) {
            // check if max length has been reached
            log_err("Can't expand past max available size, AKA UINT32_MAX.");
            return CERB_ERR;
        } else {
            int rc = d_array_grow(array); // grow d_array
            if (rc == CERB_ERR) return CERB_ERR; // if growing fails, don't insert anything and return error
        }
    }

    uint32_t i; // take each element up one position making room at [position]
    for (i = d_array_length(array); i > position; i--) {
        array->data[i] = array->data[i - 1];
    }
    array->data[i] = data; // insert new element at [position]
    array->length++; // increment length

    return CERB_OK;

error:
    return CERB_ERR;
}

// don't forget deallocating returned memory
void *d_array_remove_at(d_array *array, uint32_t position)
{
    check(array != NULL, "array is NULL.");
    check(position < d_array_length(array), "Invalid position.");

    // not freeing or setting popped element to NULL, as it will be overwritten by another element
    void *data = array->data[position];
    array->length--; // decrease length

    uint32_t i; // take each element after [position] to it's front location, disappearing data at [position]
    for (i = position; i < d_array_length(array); i++) {
        array->data[i] = array->data[i + 1];
    }

    // cast to signed 64-bit to prevent underflow. can be shrinked down to zero with >=
    if ( (int64_t) d_array_capacity(array) - d_array_expand_rate(array) >= d_array_length(array) ) {
        d_array_shrink(array);
    }

    return data; // return the data we took earlier

error:
    return NULL;
}

// equivalent to d_array_remove_at, except that it deallocates data at [position]
int d_array_delete_at(d_array *array, uint32_t position, d_array_dealloc dealloc_cb)
{
    check(array != NULL, "array is NULL.");
    check(position < d_array_length(array), "Invalid position.");
    check(dealloc_cb != NULL, "dealloc_cb is NULL.");

    dealloc_cb(array->data[position]); // deallocate data on [position]
    array->length--; // decrease length

    uint32_t i; // take each element after [position] to it's front location, disappearing data at [position]
    for (i = position; i < d_array_length(array); i++) {
        array->data[i] = array->data[i + 1];
    }

    // cast to signed 64-bit to prevent underflow. can be shrinked down to zero with >=
    if ( (int64_t) d_array_capacity(array) - d_array_expand_rate(array) >= d_array_length(array) ) {
        d_array_shrink(array);
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

// deletes first match of data in array and deallocates it too
int d_array_delete(d_array *array, void *data, d_array_dealloc dealloc_cb)
{
    check(array != NULL, "array is NULL.");
    check(data != NULL, "data is NULL.");
    check(dealloc_cb != NULL, "dealloc_cb is NULL.");

    uint32_t i; // loop through an array and find the element
    for (i = 0; i < d_array_length(array); i++) {
        if (array->compare(array->data[i], data) == D_ARRAY_EQUAL) {
            dealloc_cb(data); // deallocate data found
            array->length--; // decrease length of array
            for (; i < d_array_length(array); i++) {
                // take each element after [position] to it's front location, disappearing data at [position]
                array->data[i] = array->data[i + 1];
            }

            // cast to signed 64-bit to prevent underflow. can be shrinked down to zero with >=
            if ( (int64_t) d_array_capacity(array) - d_array_expand_rate(array) >= d_array_length(array) ) {
                d_array_shrink(array);
            }
            return CERB_OK;
        }
    }

error: // fall through
    return CERB_ERR;
}

/* splitting and joining */

// join 2 arrays, deallocating array2 and it's data block AKA array->data (you need to provide reference to those arrays)
d_array *d_array_join(d_array **array1, d_array *restrict *restrict array2)
{
    check(array1 != NULL, "Address of array1 is NULL.");
    check(*array1 != NULL, "array1 is NULL.");
    check(array2 != NULL, "Address of array2 is NULL.");
    check(*array2 != NULL, "array2 is NULL.");
    check(d_array_length(*array1) != 0, "array1 is empty.");
    check(d_array_length(*array2) != 0, "array2 is empty.");
    check((uint64_t) d_array_length(*array1) + d_array_length(*array2) <= UINT32_MAX,
    "Can't expand past max available size, AKA UINT32_MAX.");
    check((*array1)->compare == (*array2)->compare, "array1 compare and array2 compare don't match.");

    // make this signed 64-bit to prevent overflow or underflow
    int64_t diff = (int64_t) d_array_capacity(*array1) - d_array_length(*array1) - d_array_length(*array2);

    // if array1 capacity is insufficient to hold both array elements
    // then we expand and capacity is the exact amount needed to hold both array elements
    if (diff < 0) {
        // new size will be old capacity + the difference between lengths and capacity
        d_array_expand(*array1, d_array_capacity(*array1) + (diff * -1));
    }

    // copy memory of array2 at the end of array1
    memcpy((*array1)->data + d_array_length(*array1), (*array2)->data, d_array_length(*array2) * sizeof(void *));
    (*array1)->length += d_array_length(*array2);

    // deallocate array2's data and struct itself
    free((*array2)->data);
    free(*array2);

    d_array *new_array = *array1; // put array1 into this variable

    // and set it's reference to NULL
    *array1 = NULL;
    *array2 = NULL;

    return new_array;

error:
    return NULL;
}

// split one array into 2, returning splitted portion
d_array *d_array_split(d_array **array, uint32_t from_position, uint32_t to_position)
{
    check(array != NULL, "Address of array1 is NULL.");
    check(*array != NULL, "array1 is NULL.");
    check(d_array_length(*array) > 1, "Couldn't split array of length <= 1.");
    // check(from_position >= 0 && from_position < d_array_length(*array) - 1, "Invalid from_position.");
    check(from_position < d_array_length(*array) - 1, "Invalid from_position.");
    // check(to_position <= d_array_length(*array) && to_position >= 0, "Invalid to_position.");
    check(to_position <= d_array_length(*array), "Invalid to_position.");
    check(from_position < to_position, "Invalid positions.");

    uint32_t new_array_length = to_position - from_position; // calculate length of splitted part

    // create new array and assign length to it
    d_array *new_array = d_array_create((*array)->compare, new_array_length);
    check(new_array != NULL, "Couldn't split the array.");
    new_array->length = d_array_capacity(new_array);

    // copy splitted part into new array
    memcpy(new_array->data, (*array)->data + from_position, d_array_length(new_array) * sizeof(void *));

    uint32_t i = 0, j = 0; // move parts of d_array that were beyond the split position
    for (i = from_position; j < d_array_length(*array) - to_position; i++, j++) {
        (*array)->data[i] = (*array)->data[j + to_position];
    }

    // resize to fit the new length exactly
    int rc = d_array_resize(*array, d_array_length(*array) - new_array_length);
    if (rc == CERB_ERR) { // if resize fails, don't split at all and deallocate all newly created blocks
        // give back old elements their indexes
        for (i = from_position, j = 0; j < d_array_length(*array) - to_position; i++, j++) {
            (*array)->data[j + to_position] = (*array)->data[i];
        }
        // memcopy back slpitted elements to their places
        memcpy((*array)->data + from_position, new_array->data, d_array_length(new_array) * sizeof(void *));

        log_err("Failed to resize the array, thus it hasn't been splitted.");
        free(new_array->data);
        free(new_array);
        return NULL;
    }
    (*array)->length = d_array_length(*array) - new_array_length; // set array length to correct amount
    (*array)->capacity = d_array_length(*array);
    
    return new_array;
    
error:
    return NULL;
}

// deallocate all currently used data in slots, but leave the slots ( memory block AKA array->data ) allocated
int d_array_clear(d_array *restrict array, d_array_dealloc dealloc_cb)
{
    check(array != NULL, "array is NULL.");
    check(dealloc_cb != NULL, "dealloc_cb is NULL.");

    uint32_t i; // deallocate every data entry at slots, set length = 0 ( note that capacity stays the same )
    for (i = 0; i < d_array_length(array); i++) {
        dealloc_cb(array->data[i]);
    }
    array->length = 0;

    return CERB_OK;

error:
    return CERB_ERR;
}

/*
deallocate array data fields with dealloc_cb callback, free data block of array and array itself,
set array to NULL, provide reference to array to set it to NULL after deallocating it.
*/
int d_array_destroy(d_array *restrict *restrict array, d_array_dealloc dealloc_cb)
{
    check(array != NULL, "Address of array is NULL.");
    check(*array != NULL, "array is NULL.");
    check(dealloc_cb != NULL, "dealloc_cb is NULL.");

    uint32_t i; // deallocate all data fields from callback
    for (i = 0; i < d_array_length(*array); i++) {
        dealloc_cb((*array)->data[i]);
    }
    // free data block and array struct itself
    free((*array)->data);
    free(*array);

    // set array on stack to NULL
    *array = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

// go through each element in array and apply callback
int d_array_traverse(d_array *array, d_array_traverse_cb traverse_cb)
{
    check(array != NULL, "array is NULL.");
    check(traverse_cb != NULL, "traverse_cb is NULL.");

    uint32_t i; // go through each element and apply traverse callback to data fields
    for (i = 0; i < d_array_length(array); i++) {
        if (traverse_cb(array->data[i]) != CERB_OK) {
            return CERB_ERR;
        }
    }

    return CERB_OK;

error:
    return CERB_ERR;
}