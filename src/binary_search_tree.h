#ifndef F43451C3_8F17_49D8_9CE6_1B898625E8FB
#define F43451C3_8F17_49D8_9CE6_1B898625E8FB

#include <stdint.h>
#include <stdio.h>

#define CERB_OK  0
#define CERB_ERR -1

#define BSTREE_GREATER 1
#define BSTREE_LESS -1
#define BSTREE_EQUAL 0

#define BSTREE_PREORDER 0
#define BSTREE_INORDER 1
#define BSTREE_POSTORDER 2
#define BSTREE_LEVELORDER 4

#define BS_tree_length(tree_ptr) (tree_ptr)->length

typedef struct BS_tree_node {
    struct BS_tree_node *left; // left node
    struct BS_tree_node *right; // right node
    struct BS_tree_node *parent; // parent of current node
    void *data; // data you insert
} BS_tree_node;

// compare function pointer for BS_tree (note consts and restrict. your function has to follow the rules imposed by those)
typedef int (*BS_tree_cmp) (const void *const restrict data1, const void *const restrict data2);

typedef struct binary_search_tree {
    BS_tree_node *root; // root node of the tree
    BS_tree_cmp compare; // compare callback
    uint32_t length; // count of elements currently present in a tree
} BS_tree;


// create a binary search tree
BS_tree *BS_tree_create(BS_tree_cmp compare);

// insert data in tree
int BS_tree_insert(BS_tree *tree, void *data);
// search data in tree and return
void *BS_tree_search(BS_tree *tree, void *data);
// delete data from tree and return (deallocating returned data is your responsibility)
void *BS_tree_delete(BS_tree *tree, void *data);

typedef int (*BS_tree_traverse_cb) (void *data); // traverse callback for binary search tree

// traverse through tree (BSTREE_PREORDER, BSTREE_INORDER, BSTREE_POSTORDER, BSTREE_LEVELORDER) and apply traverse callback
int BS_tree_traverse(BS_tree *tree, int order, BS_tree_traverse_cb traverse_cb);

// default traverse callback has type "%d" to print
static int __default_print_traverse(void *data)
{
    printf("- %d -", *(int *) data);
    return CERB_OK;
}

// print tree args: pointer to BS_tree, order (see defines on line 13), traverse callback (if NULL, default is provided with "%d")
#define BS_tree_print(tree_ptr, order, traverse_cb)\
        BS_tree_traverse(tree, order, traverse_cb ? traverse_cb : __traverse);\
        printf("\n");

// deallocate callback for BS_tree (if your d_array contains structs containing other allocated data for example, you need this)
typedef void (*BS_tree_dealloc) (void *data);

// deallocate a whole binary search tree and set it's reference to NULL on stack (you need to provide address of tree)
int BS_tree_destroy(BS_tree **tree, BS_tree_dealloc dealloc_cb);

#endif /* F43451C3_8F17_49D8_9CE6_1B898625E8FB */
