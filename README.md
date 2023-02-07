# libcerberus, C library of data structures
---
## Overview:
This library was inspired by Zed A. Shaw's (zedshaw) book, "Learn C The Hard Way".  
It contains data structures not only presented in that book, but also some other ones too. These data structures are not copied and pasted, they are independently written by me, they have decent functionality, are tested well and I've been using them for some of my personal projects. In near future, I plan to add more data structures to this project like suffix array, ternary search tree and several others too. Moreover I will be improving the existing functionality (will make binary search tree self balancing for example) and I will take a close look on every pull request, I'll accept bugs, if they are hiding somewhere in my code, take them seriously and fix them.
## Technical info:
Code is compiled with: `gcc 11.3.0`  
Code is built on: `5.15.0-58-generic #64-Ubuntu x86_64 GNU/Linux`  
OS Version: `Ubuntu 22.04.1 LTS`
### build options:
Project uses `Makefile` and has some options you can choose from.  
1. `make`: This is default build, it makes **.a** and **.so** files, makes **bin** and **build** directories and stores static and shared library objects in **build/lib**. Header files are copied from **src** to **build/include**. This build also runs tests located in **tests** directory, *Note that tests are in bash and uses valgrind-3.18.1 to test memory*. test results are displayed on screen, valgrind output is stored in **tests/tests.log** file and tail of that file is also written on screen.
2. `make dev`: This is for developer builds. It compiles with **-g -Wextra** flags and does everything else option **1** does.
3. `make install`: This is for installing library in some location in linux filesystem after it is built with option **1**. *(DESTDIR)/(PREFIX)/lib/* is where it gets installed, where *DESTDIR* and *PREFIX* could be modified. Default values for *DESTDIR* is nothing, whereas *PREFIX=/usr/local*
4. `make check`: This option just runs the simple bash script, which goes through all files in this project and looks for some dangerous C functions such as *strcpy()* or others mainly in that family.
5. `make clean`: Lastly clean option which removes everything that got created in these build options (*except the files that got installed with option **3***) and leaves only the skeleton as it was originally.
### P.S.
*Ignore few compiler warnings that get generated, everything is under conrol.*  

---
## Description and Examples:
---
### Important notes before you start:
  
**#define CERB_OK 0**  
**#define CERB_ERR -1**  
  
*All functions return 0 for success or -1 for error. If pointer is expected from function, it returns `NULL` on error and valid pointer otherwise.*  
*I recommend checking return values with macros above (which are defined in every header file) to stay consistent. For `NULL`, test as you wish.*  
  
**#define (*NAME*)_GREATER 1**  
**#define (*NAME*)_LESS -1**  
**#define (*NAME*)_EQUAL 0**  
  
*(NAME) being the name of the data structure*  
  
*Every data structure that has comparable interface has these macros, which work as, for example, values returned from **srtcmp()** work.*
  
*See these macros in header files by yourselves to remove any uncertainty that you might have.*  
  

---
### singly linked list:
**Let's take time and analyze this piece of code**
```
#include "singly_linked.h"
#include <stdlib.h>

void free_int(void *data)
{
    free(data);
}

int printable(void *data)
{
    return * ( (int *) data );
}

int main()
{   
    int *test1 = malloc(sizeof(int));
    int *test2 = malloc(sizeof(int));
    int *test3 = malloc(sizeof(int));
    int *test4 = malloc(sizeof(int));
    int *test5 = malloc(sizeof(int));
    
    *test1 = 1;
    *test2 = 3;
    *test3 = 25;
    *test4 = 2;
    *test5 = 7;

    singly_linked *list = singly_linked_create(NULL);

    singly_linked_push(list, test1);
    singly_linked_push(list, test2);
    singly_linked_unshift(list, test4);
    singly_linked_insert_before(list, test2, test5);
    singly_linked_insert_after(list, test4, test3);

    singly_linked_print(list, printable, "%d");

    free_int(singly_linked_remove_before(list, test2));
    free_int(singly_linked_remove_after(list, test3));
    free_int(singly_linked_remove(list, test3));
    free_int(singly_linked_pop(list));

    singly_linked_print(list, printable, "%d");

    singly_linked_destroy(&list, free_int);

    return 0;
}
```

When in main, we initialize some variables on heap to work with. After that, we create a singly linked list with first argument being `NULL`, which means we are going to use **default_compare**(see how it is declared in **singly_linked.c**) function which supports integers. we need compare because when, for example, inserting or removing before or after something or searching some value in list, there has to be something that will understand what data types are we comparing to each other. You can implement a list of strings for example and make **strcmp()** your compare function.  
Next, we have five function calls which all insert data into the list in different ways (names say clearly about that). In **singly_linked_insert_before()** *test5* is inserted before *test2* and so on with **insert_after**. You get the idea.  
Next, we print the list with **singly_linked_print()**, which is a macro and takes list as it's first argument. Then comes printable, which is just a function defined above **main()** and returns integer (remember we have to tell what we want to print) from **void*** pointer. And finally format of **"%d"** which is for integers. Output of this looks like this:  
  
*[ 2 -> 25 -> 1 -> 7 -> 3 -> NULL ]* 
   
Nice and easily understandable format.  
After these we start to clean up the list, notice how function names match, just the word insert is changed with remove and it removes data from list, then returns it. After returning it is your responsibility to clean up the data pointed to by that returned pointer, which in this case we handle with **free_int()** function defined above.  
We then print the list once again to see what is left:  
  
*[ 2 -> NULL ]*  
  
As you can see, entries were removed exactly as we specified.  
Finally, we destroy the list, for which we need to provide reference to it in order to set it to `NULL` on stack. Notice the callback **free_int** passed too, which is needed to properly deallocate data pointed to by **void*** pointers. If you had list that contained struct which in itself contained pointers to some allocated data, your free callback would be much different, because you need to free not only pointer to struct, but the data pointed to struct elements (you would cast **void*** to your desired type and work with that).  
  
See **singly_linked.h** to find what functions are available to use, there are quite a lot of them and they are useful. Brief description of what each function does is written there too.  

---
### doubly linked list:
Doubly linked list is almost the same as singly linked list. Basically what I've described above, fits for the doubly linked list too.

---
### stack and queue:
Stack and queue are pretty straight forward too. They rely on doubly linked list, so you basically know how they work already. Look through their header files, it is descriptive and simple.

---
### dynamic array:
**Let's now analyze how dynamic array functions**
```
#include "d_array.h"
#include <stdlib.h>

void free_int(void *data)
{
    free(data);
}

int printable(void *data)
{
    return *((int *) data);
}

int main()
{
    int *test1 = malloc(sizeof(int));
    int *test2 = malloc(sizeof(int));
    int *test3 = malloc(sizeof(int));
    int *test4 = malloc(sizeof(int));
    int *test5 = malloc(sizeof(int));
    
    *test1 = 1;
    *test2 = 3;
    *test3 = 25;
    *test4 = 2;
    *test5 = 7;

    d_array *array = d_array_create(NULL, 1);

    d_array_push_back(array, test4);
    d_array_push_back(array, test5);
    d_array_push_front(array, test3);
    d_array_push_front(array, test2);
    d_array_insert_at(array, 2, test1);

    d_array_shrink_to_fit(array);

    d_array_print(array, printable, "%d");

    free_int(d_array_pop_back(array));
    free_int(d_array_pop_front(array));
    free_int(d_array_remove_at(array, 1));

    d_array_destroy(&array, free_int);

    return 0;
}
```
Initially, **d_array_create()** creates array to work with. This array has default capacity (AKA how many elements it is capable to hold) of 1 and compare callback of `NULL`, which, from the singly linked list, we know is default compare and is type of integer.  
Next we have inserting operations, which insert data in different ways (names are pretty descriptive, so I won't discribe them). Notice that we told array that it had an initial capacity of 1, but it expands by default if it has no more space available for new entries and shrinks if elements are getting fewer than capacity is allocated. Default number of shrinkage and growth is **100**, but it is customizable. Then we print the array which has output like this:  

*[ 3, 25, 1, 2, 7 ]*  
  
See singly linked list section for info about printing.  
Then, we remove in different ways from array and free the returned data. Finally, we destroy the array (again see singly linked section for details about how these are done).  
  
This data structure has really rich functionality, see header file **d_array.h** to find out what else is available. Every function has description above it about what it does etc...

---
### hash map:
**Now let's look into hash map**
```
#include "hashmap.h"
#include <stdlib.h>

void free_int(void *data)
{
    free(data);
}

int printable(const void *data)
{
    return *((int *) data);
}

int main()
{
    int *test1 = malloc(sizeof(int));
    int *test2 = malloc(sizeof(int));
    int *test3 = malloc(sizeof(int));
    int *test4 = malloc(sizeof(int));
    int *test5 = malloc(sizeof(int));
    
    *test1 = 1;
    *test2 = 3;
    *test3 = 25;
    *test4 = 2;
    *test5 = 7;

    Hashmap *map = Hashmap_create(NULL, NULL, 10);

    Hashmap_insert(map, test1);
    Hashmap_insert(map, test2);
    Hashmap_insert(map, test3);
    Hashmap_insert(map, test4);
    Hashmap_insert(map, test5);

    Hashmap_print(map, printable, "%d");

    int look_for = 7;
    void *data = Hashmap_find(map, &look_for);
    debug("data = %d", *(int *)data);
    
    free_int(Hashmap_remove(map, &look_for));

    Hashmap_print(map, printable, "%d");

    Hashmap_destroy(&map, free_int);

    return 0;
}
```
Hash map is built on dynamic array, each entry of it is dynamic array, map stores multiple entries of this, where it puts data.  
As always we do some setup and start working. **Hashmap_create()** creates map. First argument is hash function, if it is `NULL`, then default hash is used, which is *fnv1a* hash. Second argument is compare callback (see singly linked list for that). Lastly third is number of entries map will have. It will have **10** dynamic array entries with each being `100 * sizeof(void *)` length. Keep in mind that, for now, this number (**10**) is not expandable (I plan to implement that in future).  
Next, we have regular insertions in map. After that comes printing and it has this output:  
  
*entry: 0 -> [ 3 ]  
entry: 2 -> [ 1 ]  
entry: 4 -> [ 7 ]  
entry: 8 -> [ 25 ]  
entry: 9 -> [ 2 ]*  
  
  Notice how we have 10 entries, but only entries above are used to store our data. This entries are now allocated as default size dynamic arrays, whereas entries which don't hold data are kept empty until they are needed.  
  
After that we find the if the value **7** is present in map and we print the return value (this is for demonstrational purposes only, never use return values of any of these functions until you check, because if we can't find **7**, `NULL` will be returned), output is this:  
  
*DEBUG hashmap_tests.c:40:main() data = 7*  
  
Next, we delete that same value by passing it to **Hashmap_remove()**, which looks for the data, removes and returns it, if it exists, then we free it as usual.  
  
Before destroyeing, we print the map once again and I want you to take a close look on this:  
  
*entry: 0 -> [ 3 ]  
entry: 2 -> [ 1 ]  
entry: 4 -> [  ]  
entry: 8 -> [ 25 ]  
entry: 9 -> [ 2 ]*  
  
**7** is no longer in here as we expected, but entry **4** still prints itself. This is because, if one of the entries of map was allocated and now it got empty, it doesn't get deallocated, it sits, waiting for data to come (this might not be the best implementation for large scale data management, because it wastes memory, but for small scale I have decided so), but next line when we destroy map entirely, every region allocated, gets deallocated (again, see singly linked list if you have questions about how **destroy()** functions work).

---
### heap:
**Take a look at this code for heap**  
```
#include <stdlib.h>
#include "heap.h"

void free_int(void *data)
{
    free(data);
}

int printable(const void *data)
{
    return *((int *) data);
}

int main()
{
    int *test1 = malloc(sizeof(int));
    int *test2 = malloc(sizeof(int));
    int *test3 = malloc(sizeof(int));
    int *test4 = malloc(sizeof(int));
    int *test5 = malloc(sizeof(int));
    
    *test1 = 1;
    *test2 = 3;
    *test3 = 25;
    *test4 = 2;
    *test5 = 7;

    Heap *heap = heap_create(NULL, 200);

    heap_insert(heap, test1);
    heap_insert(heap, test2);
    heap_insert(heap, test3);
    heap_insert(heap, test4);
    heap_insert(heap, test5);

    d_array_print(heap, printable, "%d");

    free_int(heap_pop(heap));
    free_int(heap_pop(heap));

    d_array_print(heap, printable, "%d");

    heap_destroy(&heap, free_int);

    return 0;
}
```
As usual we set up some things to work with. Then we create heap with **heap_create()**. First argument is for compare callback which if `NULL`, gets set to default which is type integer. Second argument specifies initial capacity (just like in dynamic array). In fact, if you look into code you'll see this: `#define Heap d_array`. In this library heap is just dynamic array and it is only the functions of heap that give it heap behaviour.  
After Creation, we insert data in heap and print it. The output looks like this:  
  
*[ 25, 7, 3, 1, 2 ]*  
  
As you can see, heap invariant is satisfied.  
  
Now let's delete 2 value from heap and print it again (**heap_pop()** deletes and returns highest priority value of a given heap):  
  
*[ 3, 2, 1 ]*  
  
Values deleted are indeed correct and heap invariant is preserved also.  
  
Lastly we destroy heap and end with that.  
  
Heap has other functionalities too, if you look at header file for it (**heap.h**) you'll find **heapify()** function too, which you can apply to dynamic array to transform it into a heap (there are number of other useful function also).  
  
---
### priority queue:
As stack and queue in this library, priority queue is built on other data structure entirely, which is heap. Look into header file for priority queue (**priority_queue.h**) to see more. It is pretty straight forward if you follow guidlines above for heap.  
  
---
### binary search tree:
**Finally, let's look at how binary search tree is implemented**
```
#include <stdlib.h>
#include "binary_search_tree.h"
#include "dbg.h"

void free_int(void *data)
{
    free(data);
}

int printable(const void *data)
{
    return *((int *) data);
}

int main()
{
    int *test1 = malloc(sizeof(int));
    int *test2 = malloc(sizeof(int));
    int *test3 = malloc(sizeof(int));
    int *test4 = malloc(sizeof(int));
    int *test5 = malloc(sizeof(int));
    
    *test1 = 1;
    *test2 = 3;
    *test3 = 25;
    *test4 = 2;
    *test5 = 7;

    BS_tree *tree = BS_tree_create(NULL);

    BS_tree_insert(tree, test2);
    BS_tree_insert(tree, test3);
    BS_tree_insert(tree, test4);
    BS_tree_insert(tree, test1);
    BS_tree_insert(tree, test5);

    BS_tree_print(tree, BSTREE_LEVELORDER, __traverse);

    int find = 25;
    void *found = BS_tree_search(tree, &find);
    debug("found = %d", *(int *)found);
    int delete = 3;
    free_int(BS_tree_delete(tree, &delete));

    BS_tree_print(tree, BSTREE_LEVELORDER, __traverse);

    BS_tree_destroy(&tree, free_int);

    return 0;
}
```  
  
We start by creating a binary search tree, the argument `NULL` indicates, that we are using default compare callback, which is of type integer. Then we insert some data randomly in the tree.  
  
This version of print is a little bit different from what we have seen so far. **BSTREE_LEVELORDER, BSTREE_INORDER, BSTREE_PREORDER and BSTREE_POSTORDER** are macros defined in **binary_search_tree.h**, which are used to traverse tree in desired order, as we are doing it here and traversing with **BSTREE_LEVELORDER** option. Third argument is a function that will be applied when traversing through the tree. **__traverse** is defined in header file. To be more clear, when traversing, one might want to end traversal at some point based on, for example, what is found in tree. That is why **BS_tree_traverse()** will stop traversing when traverse callback it calls returns **CERB_ERR AKA -1** and that is why **__traverse()** returns **CERB_OK AKA 0**, since we don't want to stop displaying data to a screen until we are finished. The data on screen looks like this (remember leftmost is root and level order traversal is done):  
  
*-3 -- 2 -- 25 -- 1 -- 7 -*  
  
Tree is indeed valid.  
  
Next we perform find operation, which is straight forward and after that, output is shown on screen (this is for demonstrational purposes only, never use return values of any of these functions until you check, because if we can't find **25**, `NULL` will be returned) and it looks like this:  
  
*DEBUG treetests.c:41:main() found = 25*  
  
**25** was in the tree for sure and we have found it.  
  
After that, we delete value **3** which is returned by **BS_tree_delete()**'s return value (don't forget to free data).  
  
Before destruction, we print the tree to show the result of deleting **3**:  
  
*- 2 -- 1 -- 25 -- 7 -*  
  
Which is successful, tree got rearranged.  
  
Finally, we destroy the tree as in the cases of every other data structures.