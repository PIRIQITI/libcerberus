#include <stdlib.h>
#include "binary_search_tree.h"
#include "queue.h" // queue is needed for level order traversal

// default compare type is integer
static int default_compare(const void *const restrict data1, const void *const restrict data2)
{
    if ( *( (int *) data1 ) < *( (int *) data2 ) ) {
        return BSTREE_LESS;
    } else if ( *( (int *) data1 ) > *( (int *) data2 ) ) {
        return BSTREE_GREATER;
    } else {
        return BSTREE_EQUAL;
    }
}

// create a binary search tree
BS_tree *BS_tree_create(BS_tree_cmp compare)
{
    BS_tree *tree = malloc(sizeof(BS_tree));
    check_mem(tree); // allocate memory for tree

    tree->compare = compare == NULL ? default_compare : compare; // set default compare if one isn't provided
    tree->length = 0;
    tree->root = NULL;

    return tree;

error:
    return NULL;
}

// internally used to create binary search tree nodes
static inline BS_tree_node *BS_tree_node_create()
{
    BS_tree_node *node = calloc(1, sizeof(BS_tree_node));
    check_mem(node); // allocate data to binary search tree node and check

    return node;

error:
    return NULL;
}

// insert data in tree
int BS_tree_insert(BS_tree *tree, void *data)
{
    check(tree != NULL, "tree is NULL.");
    check(data != NULL, "data is NULL.");
    check(BS_tree_length(tree) != UINT32_MAX, "tree has reached it's max length AKA UINT32_MAX.");

    BS_tree_node *node = BS_tree_node_create();
    check(node != NULL, "Failed to insert in tree.");
    node->data = data; // assign data to node we just created

    if (!BS_tree_length(tree)) { // if tree is empty, insert root node
        tree->root = node; // set root node
        tree->length++; // and increment length
        return CERB_OK;
    } else {
        BS_tree_node *cur_node = tree->root; // start searching from root node
        while (1) { // this loop doesn't become infinite because return statements are present
            switch (tree->compare(data, cur_node->data)) { // compare tells us what to do
                case BSTREE_GREATER: // if right node exists change cur node to it, else insert and finish
                    if (cur_node->right) {
                        cur_node = cur_node->right; // change cur_node to it's right node
                    } else {
                        cur_node->right = node; // insert node
                        node->parent = cur_node; // make node's parent it's previous node AKA cur_node
                        tree->length++; // increment length
                        return CERB_OK;
                    }
                    break;
                case BSTREE_LESS: // if left node exists change cur node to it, else insert and finish
                    if (cur_node->left) {
                        cur_node = cur_node->left; // change cur_node to it's left node
                    } else {
                        cur_node->left = node; // insert node
                        node->parent = cur_node; // make node's parent it's previous node AKA cur_node
                        tree->length++; // increment length
                        return CERB_OK;
                    }
                    break;
                case BSTREE_EQUAL:
                    log_err("tree already contains data pointed by this ptr -> %p", data);
                    free(node); // free node which was allocated for data (since we aren't inserting it)
                    return CERB_ERR; // we don't allow duplicate entries
                default:
                    return CERB_ERR; // default won't run
            }
        }
    }

error:
    return CERB_ERR;
}

// internally used to search for node holding data in tree
static void *BS_tree_search_node(BS_tree *tree, void *data)
{
    BS_tree_node *cur_node = tree->root; // start searching from root node
    while (1) { // this loop doesn't become infinite because return statements are present
        switch (tree->compare(data, cur_node->data)) { // compare tells us what to do
            case BSTREE_GREATER: // if right node exists change cur node to it
                if (cur_node->right) cur_node = cur_node->right; // change cur_node to it's right node
                else return NULL; // if we were going right and nodes were no more, this means data isn't here
                break;
            case BSTREE_LESS: // if left node exists change cur node to it
                if (cur_node->left) cur_node = cur_node->left; // change cur_node to it's left node
                else return NULL; // if we were going left and nodes were no more, this means data isn't here
                break;
            case BSTREE_EQUAL:
                return cur_node; // return data if equal
            default:
                return NULL; // default won't run
        }
    }
}

void *BS_tree_search(BS_tree *tree, void *data)
{
    check(tree != NULL, "tree is NULL.");
    check(data != NULL, "data is NULL.");
    check(BS_tree_length(tree) != 0, "tree is empty.");

    BS_tree_node *found_node = BS_tree_search_node(tree, data);
    if (found_node) return found_node->data;

error: // fall through
    return NULL;
}

// // change left to one of the nodes which isn't NULL (remember when swapping one of the children is always NULL)
// if (new->parent->left == new) new->parent->left = new->left ? new->left : new->right;
// else new->parent->right = new->right ? new->right : new->left;
// // change right to one of the nodes which isn't NULL
// // above if else covers the case where there migh be removal from singly linkd list like tree

// internally used by delete function to replace a deleting node
static void BS_tree_replace(BS_tree_node *new, BS_tree_node *old)
{
    // change left to one of the nodes which isn't NULL (remember when swapping one of the children is always NULL)
    if (new->parent->left == new) {
        if (new->left) { // if it isn't leaf node, set parent's new node and child's new node appropriately
            new->parent->left = new->left;
            new->left->parent = new->parent;
        } else { // if it is leaf node, just set parent->left to NULL to break the chain
            new->parent->left = NULL; // COMMENTED OUT THIS LINE TO TEST BOTTOM 2 LINES
            // new->parent->left = new->right;
            // if (new->right) new->right->parent = new->parent;
        }
    } else { // change right to one of the nodes which isn't NULL
        if (new->right) { // if it isn't leaf node, set parent's new node and child's new node appropriately
            new->parent->right = new->right;
            new->right->parent = new->parent;
        } else { // if it is leaf node, just set parent->right to NULL to break the chain
            new->parent->right = NULL; // COMMENTED OUT THIS LINE TO TEST BOTTOM 2 LINES
            // new->parent->right = new->left;
            // if (new->left) new->left->parent = new->parent;
        }
    }
    // above if else covers the case where there migh be removal from singly linkd list like tree

    // now that we have finished setting up nodes for new to be succesfully removed
    // we can start taking care of the old node which must be replaced
    
    // if old->parent is NULL (AKA old is root node), new's parent will automatically become NULL
    new->parent = old->parent;
    // if parent is present, that means we are removing somewhere in the middle or end
    if (old->parent != NULL) {
        // if old node was it's parent's left node, change parent->left to new
        if (old->parent->left == old) old->parent->left = new;
        else old->parent->right = new;
        // else if it was right change parent->right to new
    }
    new->left = old->left; // set up links to left side
    if (new->left) new->left->parent = new; // if left wasn't NULL, set it's parent to new (since it was old)
    new->right = old->right; // set up links to right node
    if (new->right) new->right->parent = new; // if right wasn't NULL, set it's parent to new (since it was old)
}

// internally used by delete function to find replacement of deleting node
static inline BS_tree_node *BS_tree_find_replacement(BS_tree_node *node)
{
    BS_tree_node *replacement = NULL;
    if (node->left) { // if node->left exists search replacement node on left side of the tree
        replacement = node->left; // go to the left node, take right turn and find max
        while (replacement->right) replacement = replacement->right; // if right is NULL left is max
    } else if (node->right) { // if node->right exists search replacement node on right side of the tree
        replacement = node->right; // go to the right node, take left turn and find min
        while (replacement->left) replacement = replacement->left; // if left is NULL right is min
    } else { // if neither left nor right are present then it is leaf node and it doesn't need replacement
        // so do nothing in this else block, go down and return
    }
    return replacement;
}

// internally used to change one node (if it exists) with another when deleting
void *BS_tree_change(BS_tree *tree, BS_tree_node *cur_node)
{
    // make case for root (be very careful of NULLs) (its done)
    BS_tree_node *replacement = BS_tree_find_replacement(cur_node); // find replacement node
    // debug("replacement = %d", *(int *) replacement->data);
    if (replacement) { // if node wasn't leaf we are replacing it with valid one
        BS_tree_replace(replacement, cur_node);
        // if it was root, set tree->root to replacement too
        if (cur_node == tree->root) tree->root = replacement;
    } else { // else deleting it and setting parent's link to it to NULL
        if (cur_node->parent) { // this means we aren't deleting root
            // set parent's correct child's link to NULL as it will be removed
            if (cur_node->parent->left == cur_node) cur_node->parent->left = NULL;
            else cur_node->parent->right = NULL;
            // if (cur_node->parent->right == NULL) debug("NULL");
            // debug("cur_node->parent->data = %d", *(int *) cur_node->parent->data);
        } else { // this means we are deleting root node
            tree->root = NULL; // set tree->root to NULL as we will free it down there
        }
    }
    void *data = cur_node->data; // save data before access becomes UB
    free(cur_node); // deallocate node
    tree->length--; // decrease length
    return data; // return data
}

// delete data from tree and return (deallocating returned data is your responsibility)
void *BS_tree_delete(BS_tree *tree, void *data)
{
    check(tree != NULL, "tree is NULL.");
    check(data != NULL, "data is NULL.");
    check(BS_tree_length(tree) != 0, "tree is empty.");

    BS_tree_node *cur_node = tree->root; // start searching from root node
    while (1) { // this loop doesn't become infinite because return statements are present
        switch (tree->compare(data, cur_node->data)) { // compare tells us what to do
            case BSTREE_GREATER:
                if (cur_node->right) cur_node = cur_node->right; // if right node exists change cur node to it
                else return NULL; // if we were going right and nodes were no more, this means data isn't here
                break;
            case BSTREE_LESS:
                if (cur_node->left) cur_node = cur_node->left; // if left node exists change cur node to it
                else return NULL; // if we were going left and nodes were no more, this means data isn't here
                break;
            case BSTREE_EQUAL: // return the data which has just got deleted
                return BS_tree_change(tree, cur_node);
            default:
                return NULL; // default won't run
        }
    }

error:
    return NULL;
}

// internally used to traverse through tree
static void traverse(BS_tree_node *node, int order, int *rc, BS_tree_traverse_cb traverse_cb)
{ // node is a BS_tree_node which changes every recursion accordingly
    if (*rc == CERB_ERR || node == NULL) return; // if, at any point *rc becomes 0 or node won't exist we return
    // find out in which order to traverse through tree
    switch (order) {
        case BSTREE_PREORDER: // preorder traversal
            *rc = traverse_cb(node->data);
            traverse(node->left, order, rc, traverse_cb);
            traverse(node->right, order, rc, traverse_cb);
            break;
        case BSTREE_INORDER: // inorder traversal
            traverse(node->left, order, rc, traverse_cb);
            *rc = traverse_cb(node->data);
            traverse(node->right, order, rc, traverse_cb);
            break;
        case BSTREE_POSTORDER: // postorder traversal
            traverse(node->left, order, rc, traverse_cb);
            traverse(node->right, order, rc, traverse_cb);
            *rc = traverse_cb(node->data);
            break;
        default:
            log_err("Invalid order.");
            *rc = CERB_ERR;
            break;
    }   
}

// internally used to make push code size compact
#define PUSH_NODE(n) if(n){rc=Queue_push(queue,n);check(rc!=CERB_ERR,"Out of memory on level order traversal.");}

// internally used to do level order traversal
static inline int traverse_LOT(Queue *queue, BS_tree_node *start, BS_tree_traverse_cb traverse_cb)
{
    int rc; // this is for checking Queue_push return values
    PUSH_NODE(start); // push root and check for memory also
    while (queue->first) { // as long as queue has an element, it won't stop
        start = Queue_shift(queue); // remove from the front of a queue
        PUSH_NODE(start->left); // push left of current (if exists)
        PUSH_NODE(start->right); // push right of current (if exists)
        rc = traverse_cb(start->data);
        if (rc == CERB_ERR) return CERB_ERR; // if traverse stops we'll stop too
    }
    return CERB_OK;

error:
    return CERB_ERR;
}

// internally used to just pass to the Queue_destroy function (the name "nofree": it won't free anything that tree has)
static inline void nofree_cb(void *data)
{
    // gcc will generate unused param warning when wextra is enabled, disregard that in this case
    return; // do nothing
}

// for level order traversal for printing, levels could be stored in an array
// and then they could be printed out together to show the user which level has what

// traverse through tree (BSTREE_PREORDER, BSTREE_INORDER, BSTREE_POSTORDER, BSTREE_LEVELORDER) and apply traverse callback
int BS_tree_traverse(BS_tree *tree, int order, BS_tree_traverse_cb traverse_cb)
{
    check(tree != NULL, "tree is NULL.");
    check(BS_tree_length(tree) != 0, "tree is empty.");
    check(traverse_cb != NULL, "traverce_cb is NULL.");

    BS_tree_node *start = tree->root; // start node is always root
    int rc; // for memory checking

    // if level order traversal is requested we do it in traverse_LOT function
    if (order == BSTREE_LEVELORDER) {
        Queue *queue = Queue_create(tree->compare); // create queue to order the level of tree
        check(queue != NULL, "Out of memory on level order traversal."); // check to see if have queue
        rc = traverse_LOT(queue, start, traverse_cb); // do the traversal
        Queue_destroy(&queue, nofree_cb); // destroy the queue
    } else {
        rc = CERB_OK; // rc will get us traverse_cb return value back
        // traverse function does pre in and post order traversals
        traverse(start, order, &rc, traverse_cb);
    }

    return rc;

error:
    return CERB_ERR;
}

// internally used to recurse through tree and deallocate the data it holds
static inline void postorder_free(BS_tree_node *node, BS_tree_dealloc dealloc_cb)
{
    // if left and right exist free in that case, otherwise do not
    if (node->left) postorder_free(node->left, dealloc_cb);
    if (node->right) postorder_free(node->right, dealloc_cb);
    dealloc_cb(node->data); // call callback to free data
    free(node); // free node allocated internally
}

// deallocate a whole binary search tree and set it's reference to NULL on stack (you need to provide address of tree)
int BS_tree_destroy(BS_tree **tree, BS_tree_dealloc dealloc_cb)
{
    check(tree != NULL, "Address of tree is NULL.");
    check(*tree != NULL, "tree is NULL.");
    check(dealloc_cb != NULL, "dealloc_cb is NULL.");

    // if tree isn't empty deallocate it's elements
    if (BS_tree_length(*tree)) postorder_free((*tree)->root, dealloc_cb);
    free(*tree); // free tree structure itself

    *tree = NULL; // set stack reference to NULL

    return CERB_OK;
    
error:
    return CERB_ERR;
}