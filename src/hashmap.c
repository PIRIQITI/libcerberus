#include <stdlib.h>
#include "hashmap.h"

const uint32_t FNV_PRIME = 16777619;
const uint32_t FNV_OFFSET_BASIS = 2166136261;

// fnv1a hash (default)
static uint32_t default_hash(void *data)
{
    uint32_t hash = FNV_OFFSET_BASIS;

    int i = 0, len = strlen((char *) data);
    for (i = 0; i < len; i++) {
        hash ^= i < len ? ((char *) data)[i] : 0;
        hash *= FNV_PRIME;
    }

    return hash;
}

// default compare type is integer
int Hashmap_default_compare(const void *restrict data1, const void *restrict data2)
{
    if ( *( (int *) data1 ) < *( (int *) data2 ) ) {
        return HASHMAP_LESS;
    } else if ( *( (int *) data1 ) > *( (int *) data2 ) ) {
        return HASHMAP_GREATER;
    } else {
        return HASHMAP_EQUAL;
    }
}

Hashmap *Hashmap_create(Hashmap_cmp cmp, hash hash, uint32_t number_of_entries)
{
    // allocate memory for struct
    Hashmap *map = malloc(sizeof(Hashmap));
    check_mem(map);

    uint32_t amount = number_of_entries == 0 ? DEFAULT_NUMBER_OF_ENTRIES : number_of_entries;

    // allocate some number of blocks to hold pointers to d_array
    map->entries = (d_array **) calloc(amount, sizeof(d_array *));
    check_mem(map->entries);

    // if hash is NULL, then default hash is set
    map->hash = hash == NULL ? default_hash : hash;

    // if cmp is present set it, else set default
    map->compare = cmp == NULL ? Hashmap_default_compare : cmp;
    
    // set capacity to number of entries (default or specified)
    map->capacity = amount;

    return map;

error:
    if (map) free(map);
    return NULL;
}

// internally used to find entry or create it
static inline d_array *find_or_create(Hashmap *map, uint32_t hash, int create)
{
    if (create && !map->entries[hash]) {
        // create entry (number of elements each entry will hold is gonna be the same as capacity of map)
        map->entries[hash] = d_array_create(map->compare, DEFAULT_NUMBER_OF_ENTRIES);
        check_mem(map->entries[hash]);
    }

    return map->entries[hash];

error:
    return NULL;
}

// internally used to create hashmap nodes
static inline Hashmap_node *Hashmap_node_create(void *data, uint32_t hash)
{
    Hashmap_node *node = malloc(sizeof(Hashmap_node)); // allocate memory for hashmapnode
    check_mem(node);

    node->data = data;
    node->hash = hash;

    return node;

error:
    return NULL;
}

// internally used for rehashing items if needed
static void Hashmap_rehash(Hashmap *map)
{
    // reallocate map->entries so that it has |capacity + capacity / 2| capacity

    // loop through each bucket, rehash and add items to their new entries
    // (min Heap could be used to start rehashing smallest lengthed entries first)
    // (which could decrease total time took by rehash)

    /*
        typedef struct Heap_property_for_Hashmap_handling {
            uint32_t length;
            uint32_t active_position;
        } Property;
    */

   // above structure is an example of how heap could be used for maintaining active entries list (active_position)
   // and their lengths for efficient rehashing (starting from min lengths and going forward) (length)
   // this heap should be sorted by length (length) for efficient rehashing
   // active_positions don't need sort because we need their list just for deallocating memory
   // loop through heap as d_array and deallocate active positions

   // it would be better to wrap Heap in priority_queue and work with that
}

// insetr data in hashmap
int Hashmap_insert(Hashmap *restrict map, void *data)
{
    check(map != NULL, "map is NULL.");
    check(data != NULL, "data is NULL.");

    // obtain hash value
    uint32_t hash = map->hash(data) % Hashmap_capacity(map);

    // find or create entry
    d_array *entry = find_or_create(map, hash, 1);
    check(entry != NULL, "Failed to insert in map.");

    // create hashmap node
    Hashmap_node *node = Hashmap_node_create(data, hash);
    check(node != NULL, "Failed to insert in map."); // in case of error here, entry above stays allocated anyways

    // add value to it
    d_array_push_back(entry, node);
    map->length++; // increase length after inserting

    return CERB_OK;

error:
    return CERB_ERR;
}

// find data with key associated with it (in most cases key is data itself)
void *Hashmap_find(Hashmap *restrict map, void *restrict key)
{
    check(map != NULL, "map is NULL.");
    check(key != NULL, "key is NULL.");

    // obtain hash value
    uint32_t hash = map->hash(key) % Hashmap_capacity(map);

    d_array *entry = find_or_create(map, hash, 0);
    if (!entry) return NULL; // if entry doesn't exist at all then data doesn't exist at all too

    uint32_t i; // if entry exists find data within it
    for (i = 0; i < d_array_length(entry); i++) {
        if (entry->compare(((Hashmap_node *)entry->data[i])->data, key) == HASHMAP_EQUAL) {
            return ((Hashmap_node *)entry->data[i])->data;
        }
    }

error: // fall through
    return NULL;
}

// remove data with key from map (deallocating returned data is your responsibility)
void *Hashmap_remove(Hashmap *restrict map, void *restrict key)
{
    check(map != NULL, "map is NULL.");
    check(key != NULL, "key is NULL.");

    // obtain hash value
    uint32_t hash = map->hash(key) % Hashmap_capacity(map);

    d_array *entry = find_or_create(map, hash, 0);
    if (!entry) return NULL; // if entry doesn't exist at all then data doesn't exist at all too

    Hashmap_node *found_node = NULL;

    uint32_t i; // if entry exists find data within it
    for (i = 0; i < d_array_length(entry); i++) {
        if (entry->compare(((Hashmap_node *)entry->data[i])->data, key) == HASHMAP_EQUAL) {
            found_node = (Hashmap_node *) entry->data[i];
            break;
        }
    }
    if (!found_node) return NULL;

    Hashmap_node *last_node = d_array_pop_back(entry);
    void *data = found_node->data; // keep the data

    if (found_node == last_node) {
        free(last_node); // if last one was the one we were looking for just free it's node and return data
    } else {
        free(found_node); // deallocate found node
        entry->data[i] = last_node; // and set last node at it's place
    }
    map->length--; // decrease length after removing

    return data;

error:
    return NULL;
}

// deallocate entire hashmap
int Hashmap_destroy(Hashmap **restrict map, Hashmap_dealloc dealloc_cb)
{
    check(map != NULL, "Address of map is NULL.");
    check(*map != NULL, "map is NULL.");

    uint32_t i;
    for (i = 0; i < Hashmap_capacity(*map); i++) {
        d_array *cur_array = (*map)->entries[i];
        if (cur_array) {
            uint32_t j;
            for (j = 0; j < d_array_length(cur_array); j++) {
                dealloc_cb(((Hashmap_node *)cur_array->data[j])->data); // free actual data
                free(cur_array->data[j]); // free Hashmap_node
            }
            free(cur_array->data); // free cur_array->data
            free(cur_array); // and cur_array itself and move to the next one
        }
    }
    free((*map)->entries);
    free(*map);
    *map = NULL;

    return CERB_OK;

error:
    return CERB_ERR; // maintain a list of active (allocated) entries for efficiency
}