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

bool delete_fib_node(Fibonacci_Heap *fh, void *data);

bool change_fib_node_value(Fibonacci_Heap *fh, void *old_val, void *new_val);

void *get_min(Fibonacci_Heap *fh);

#endif // FIBONACCI_HEAP_H
