#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h> // Added for malloc
#include "fibonacci_heap.h"

// Struct definitions are now in fibonacci_heap.h

// Function to create an empty Fibonacci heap
Fibonacci_Heap *create_fib_heap() {
    Fibonacci_Heap *heap = (Fibonacci_Heap *)malloc(sizeof(Fibonacci_Heap));
    if (heap == NULL) {
        return NULL; // Memory allocation failed
    }
    heap->min = NULL;
    heap->n = 0;
    heap->root_list = NULL;
    return heap;
}

// Function to insert a new node into the Fibonacci heap
bool insert_fib_heap(Fibonacci_Heap *fh, void *data) {
    if (fh == NULL) {
        return false; // Heap does not exist
    }

    // 1. Allocate memory for a new Fibonacci_Node
    Fibonacci_Node *new_node = (Fibonacci_Node *)malloc(sizeof(Fibonacci_Node));
    if (new_node == NULL) {
        return false; // Memory allocation failed
    }

    // 2. Initialize the new node
    new_node->key = data;
    new_node->degree = 0;
    new_node->marked = false;
    new_node->parent = NULL;
    new_node->child = NULL;
    // new_node->left and new_node->right are set below

    // 3. Add the new node to the root list
    if (fh->min == NULL) { // If the heap is empty
        fh->min = new_node;
        new_node->left = new_node;
        new_node->right = new_node;
        fh->root_list = new_node; // fh->min is the root_list
    } else {
        // Insert new_node to the right of fh->min
        // fh->min is part of the root list (which is circular)
        new_node->right = fh->min->right;
        new_node->left = fh->min;
        fh->min->right->left = new_node;
        fh->min->right = new_node;
        // fh->root_list remains a valid entry point, no need to change it
        // unless it was NULL, but that's covered by fh->min == NULL case.
    }

    // 4. Update fh->min if the new node's key is smaller
    if (fh->min == NULL || *(int *)(new_node->key) < *(int *)(fh->min->key)) {
        fh->min = new_node;
    }

    // 5. Increment fh->n
    fh->n++;

    return true;
}

// Function to get the minimum key from the Fibonacci heap
void *get_min(Fibonacci_Heap *fh) {
    if (fh == NULL || fh->min == NULL) {
        return NULL; // Heap is empty or invalid
    }
    return fh->min->key;
}
