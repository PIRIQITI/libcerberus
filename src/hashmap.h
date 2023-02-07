#ifndef FA51084D_5EA5_4D17_AF8D_98B4A541B182
#define FA51084D_5EA5_4D17_AF8D_98B4A541B182

#include "d_array.h"

#define HASHMAP_GREATER 1
#define HASHMAP_LESS -1
#define HASHMAP_EQUAL 0

#define DEFAULT_NUMBER_OF_ENTRIES 500

// get map length
#define Hashmap_length(map_ptr) (map_ptr)->length
// get map capacity
#define Hashmap_capacity(map_ptr) (map_ptr)->capacity

// this macro supports printing Hashmap in pretty nice format (see d_array_print to find out about data_cb and format)
#define Hashmap_print(map_ptr, data_cb, format) if(!map){log_err("map is NULL.");}else{uint32_t i;for(i=0;i<Hashmap_capacity(map);\
        i++){if(map->entries[i]){printf("entry: %u -> [ ",i);int64_t j;for(j=0;j<(int64_t)d_array_length(map->entries[i])-1;j++){\
        printf(format" ",data_cb(((Hashmap_node*)map->entries[i]->data[j])->data));}if(d_array_length(map->entries[i]))\
        {printf(format,data_cb(((Hashmap_node*)map->entries[i]->data[j])->data));}printf(" ]\n");}}}

// Hashmap_nodes are stored in Hashmap->entry d_arrays
typedef struct Hashmap_node {
    void *data; // holds pointer to actual data
    uint32_t hash; // hash value is stored here
} Hashmap_node;

typedef uint32_t (*hash) (void *data); // hash function pointer
// compare function pointer for map (note consts and restrict. your function has to follow the rules imposed by those)
typedef int (*Hashmap_cmp) (const void *const restrict data1, const void *const restrict data2);

typedef struct Hashmap {
    d_array **entries; // entries contain other d_arrays as it's data
    Hashmap_cmp compare; // compare callback
    hash hash; // hash is hash function for Hashmap
    uint32_t capacity; // capacity is number of entries (wether they are allocated or not)
    uint32_t length; // length is number of elements each entry holds combined
} Hashmap;

// create map with number_of_entries size, hash, and cmp (compare) function
// if number_of_entries is 0, DEFAULT_NUMBER_OF_ENTRIES (500) is allocated
Hashmap *Hashmap_create(Hashmap_cmp cmp, hash hash, uint32_t number_of_entries);

// insert data in map
int Hashmap_insert(Hashmap *restrict map, void *data);
// find key in map (key and data are the same in most cases, but it still depends on your implementation of default_hash)
void *Hashmap_find(Hashmap *restrict map, void *restrict key);
// remove key from map (key and data are the same in most cases, but it still depends on your implementation of default_hash)
void *Hashmap_remove(Hashmap *restrict map, void *restrict key);

typedef void (*Hashmap_dealloc) (void *data); // function pointer for handling data freeing

// destroy map and set it to NULL on stack (you have to provide reference for map)
int Hashmap_destroy(Hashmap **restrict map, Hashmap_dealloc dealloc_cb);

#endif /* FA51084D_5EA5_4D17_AF8D_98B4A541B182 */
