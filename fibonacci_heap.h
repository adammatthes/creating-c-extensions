#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <stdbool.h>
#include <stddef.h>

// Node structure
typedef struct Fibonacci_Node {
    void *key;
    int degree;
    bool marked;
    struct Fibonacci_Node *parent;
    struct Fibonacci_Node *child;
    struct Fibonacci_Node *left;
    struct Fibonacci_Node *right;
} Fibonacci_Node;

// Heap structure
typedef struct Fibonacci_Heap {
    Fibonacci_Node *min;
    int n;
    Fibonacci_Node *root_list; 
} Fibonacci_Heap;

Fibonacci_Heap *create_fib_heap();

bool insert_fib_heap(Fibonacci_Heap *fh, void *data);

// For delete_fib_node, 'data' is expected to be an int* pointing to the value to be searched and deleted.
// The function will search for a node N where *(int*)(N->key) == *(int*)data.
// If found, the function will free N->key and then delete the node.
bool delete_fib_node(Fibonacci_Heap *fh, void *data);

// For change_fib_node_value, 'old_val' is an int* pointing to the value to be searched.
// 'new_val' is an int* (already allocated by caller) that will be adopted by the found node.
// The function will search for a node N where *(int*)(N->key) == *(int*)old_val.
// If found, it will free N->key, set N->key = new_val, and then perform heap adjustments.
bool change_fib_node_value(Fibonacci_Heap *fh, void *old_val, void *new_val);

void *get_min(Fibonacci_Heap *fh);

void *extract_min_fib_heap(Fibonacci_Heap *fh);

bool decrease_key_fib_heap(Fibonacci_Heap *fh, Fibonacci_Node *node, void *new_key);

bool delete_node_fib_heap(Fibonacci_Heap *fh, Fibonacci_Node *node);

void destroy_fib_heap(Fibonacci_Heap *fh);

#endif // FIBONACCI_HEAP_H
