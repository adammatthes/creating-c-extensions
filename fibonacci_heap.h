#IFNDEF FIBONACCI_HEAP_H
#DEFINE FIBONACCI_HEAP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Fibonacci_Heap Fibonacci_Heap;

Fibonacci_Heap *create_fib_heap();

bool insert_fib_heap(Fibonacci_Heap *fh, void *data);

bool delete_fib_node(Fibonacci_Heap *fh, void *data);

bool change_fib_node_value(Fibonacci_Heap *fh, void *old_val, void *new_val);

void *get_min(Fibonacci_Heap *fh);

#ENDIF
