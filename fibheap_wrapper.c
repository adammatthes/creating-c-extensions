#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h> // For malloc, free
#include "fibonacci_heap.h" // Assumes this is in the same directory

// --- Helper function to compare integers stored as void* ---
// This will be used by the Fibonacci heap internally if it needs to compare keys.
// We might need to pass this to create_fib_heap if it supports custom comparators.
// For now, this is a placeholder; the C heap might do direct int casting and comparison.
// int compare_int_keys(void* a, void* b) {
//     int int_a = *(int*)a;
//     int int_b = *(int*)b;
//     if (int_a < int_b) return -1;
//     if (int_a > int_b) return 1;
//     return 0;
// }

// --- Definition of the Python object ---
typedef struct {
    PyObject_HEAD
    Fibonacci_Heap *fh;
    // We might add a PyObject* to a comparison function here if needed later
} FibHeapObject;

// --- Forward declaration of the type object ---
static PyTypeObject FibHeapType;

// --- Methods for the FibHeapObject ---

// __new__ or __init__
static PyObject *
FibHeap_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    FibHeapObject *self;
    self = (FibHeapObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->fh = create_fib_heap();
        if (self->fh == NULL) {
            Py_DECREF(self);
            PyErr_SetString(PyExc_MemoryError, "Failed to create Fibonacci Heap.");
            return NULL;
        }
        // If create_fib_heap took a comparator:
        // self->fh->compare = compare_int_keys; // Or however it's set
    }
    return (PyObject *)self;
}

// __dealloc__
static void
FibHeap_dealloc(FibHeapObject *self) {
    if (self->fh != NULL) {
        // IMPORTANT: This is a placeholder for proper cleanup.
        // We need a function in fibonacci_heap.c, e.g., destroy_fib_heap(fh),
        // that iterates through all nodes, frees their keys (the int* we allocated),
        // and then frees the heap structure.
        // For now, we'll just free the heap structure itself, which is a memory leak
        // for the keys. This will be addressed in step 2 of the main plan.
        
        // Conceptual cleanup (actual implementation depends on fibonacci_heap.c):
        // Fibonacci_Node *current_node = self->fh->min; // Or some way to iterate root list
        // if (current_node != NULL) {
        //     // ... iterate and free all node->key contents ...
        // }
        // free(self->fh); // This only frees the heap struct, not its contents.
        
        // We will need a proper `destroy_fib_heap` function.
        // destroy_fib_heap(self->fh); // This is the target
        
        // TEMPORARY: just free the main struct.
        // This will be updated once destroy_fib_heap is available.
        // To avoid compilation errors for now if destroy_fib_heap doesn't exist.
        // In fibonacci_heap.c, we'd expect something like:
        // void destroy_fib_heap(Fibonacci_Heap* heap) {
        //    if (!heap) return;
        //    while (heap->n > 0) {
        //        void* key = extract_min_fib_heap(heap); // Assuming extract_min returns key
        //        if (key) free(key); // Free the int*
        //    }
        //    free(heap);
        // }
        // For now, if extract_min is not destructive of keys, or if we don't have it,
        // we can't properly clean. This is a major point for the next step.
        // Let's assume for now that `fibonacci_heap.c` will be updated with a
        // function that handles freeing internal keys.
        // If `extract_min_fib_heap` frees the node but returns the key, we can do:
        if (self->fh->n > 0) {
             // This is a temporary, inefficient way to clear if extract_min returns key
             // AND if the heap doesn't manage key memory internally on delete/extract.
             // A dedicated destroy function in the C library is better.
             // This loop is problematic if extract_min_fib_heap itself doesn't free the key's container node
             // or if it modifies the heap structure in a way that makes repeated calls unsafe without re-linking.
             // For now, we'll assume such a `destroy_fib_heap` will be made available.
             // So, this dealloc will be simplified assuming that function will be called.
        }
        // Placeholder for calling the actual destroy function from the C library
        // destroy_fib_heap(self->fh); 
        // If no such function, then at least free the heap struct itself.
        free(self->fh); // This is insufficient for keys, but prevents FibHeap struct leak.
        self->fh = NULL;
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}

// __repr__
static PyObject *
FibHeap_repr(FibHeapObject *self) {
    if (self->fh == NULL) {
        return PyUnicode_FromString("<FibHeap object (uninitialized)>");
    }
    // In a real scenario, you might want to show more info, like size or min element
    return PyUnicode_FromFormat("<FibHeap object at %p, size %d>", (void *)self, self->fh->n);
}

// --- Python Methods for FibHeap ---

// insert(self, value)
static PyObject *
FibHeap_insert(FibHeapObject *self, PyObject *args) {
    long val;
    if (!PyArg_ParseTuple(args, "l", &val)) {
        return NULL; // Error already set by PyArg_ParseTuple
    }

    if (self->fh == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Heap not initialized.");
        return NULL;
    }

    // Allocate memory for the integer key, as the C heap stores void*
    int *key_ptr = (int *)malloc(sizeof(int));
    if (key_ptr == NULL) {
        return PyErr_NoMemory();
    }
    *key_ptr = (int)val; // Cast long to int; consider error checking for overflow if Python long > C int max

    if (!insert_fib_heap(self->fh, key_ptr)) {
        free(key_ptr); // Free if insertion failed
        PyErr_SetString(PyExc_RuntimeError, "Failed to insert into Fibonacci Heap.");
        return NULL;
    }

    Py_RETURN_NONE;
}

// get_min(self)
static PyObject *
FibHeap_get_min(FibHeapObject *self, PyObject *Py_UNUSED(ignored)) {
    if (self->fh == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Heap not initialized.");
        return NULL;
    }

    if (self->fh->n == 0) { // Or check self->fh->min == NULL
        Py_RETURN_NONE; // Standard Python way to indicate "empty" or "not found" for get operations
    }

    void *min_key_ptr = get_min(self->fh);
    if (min_key_ptr == NULL) { // Should be redundant if n > 0 check is done
        Py_RETURN_NONE;
    }

    return PyLong_FromLong(*(int *)min_key_ptr);
}

// extract_min(self)
static PyObject *
FibHeap_extract_min(FibHeapObject *self, PyObject *Py_UNUSED(ignored)) {
    if (self->fh == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Heap not initialized.");
        return NULL;
    }

    if (self->fh->n == 0) { // Or check self->fh->min == NULL
        Py_RETURN_NONE;
    }

    void *extracted_key_ptr = extract_min_fib_heap(self->fh);
    if (extracted_key_ptr == NULL) {
        // This case should ideally not happen if n > 0,
        // but good to handle if extract_min can return NULL for other reasons.
        PyErr_SetString(PyExc_RuntimeError, "extract_min_fib_heap returned NULL unexpectedly.");
        return NULL;
    }

    long val = *(int *)extracted_key_ptr;
    free(extracted_key_ptr); // CRITICAL: Free the int* that was allocated in insert

    return PyLong_FromLong(val);
}

// delete(self, value)
static PyObject *
FibHeap_delete(FibHeapObject *self, PyObject *args) {
    long val;
    if (!PyArg_ParseTuple(args, "l", &val)) {
        return NULL;
    }

    if (self->fh == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Heap not initialized.");
        return NULL;
    }

    // Note: delete_fib_node expects a void* that matches the stored key's value.
    // It does NOT take a pointer to a value to search for, but rather, the value itself
    // cast to void*. However, our heap stores int*s.
    // This means delete_fib_node must be able to find the int* whose *value* is `val`,
    // and then free that specific `int*`.
    // The current `delete_fib_node(fh, data)` signature in fibonacci_heap.h suggests it expects `data`
    // to be the actual pointer stored. This is a mismatch if we pass `(void*)&val`.
    // We need `delete_fib_node` to search for a node where `*(int*)node->key == val`.
    // For now, we will assume `delete_fib_node` in `fibonacci_heap.c` is adapted or
    // a new function is created that can search by value and free the internal key pointer.
    // This is a point to address in Step 2 (Modify fibonacci_heap.c).
    // Let's call it with a temporary int pointer for now, assuming the C func will handle it.
    // This is a placeholder for what `delete_fib_node` needs to achieve.
    // A proper implementation would require `delete_fib_node` to:
    // 1. Find the node N such that *(int*)(N->key) == val.
    // 2. If found, free N->key.
    // 3. Proceed with deleting N from the heap.
    
    // Create a temporary int with the value to facilitate search by value (conceptual)
    // This specific pointer `temp_key_for_search_val` is NOT what's stored.
    // The function `delete_fib_node` must understand this.
    // A better API for `delete_fib_node` might be `delete_fib_node_by_value(fh, int value_to_delete)`.
    // Assuming `delete_fib_node` can handle finding the actual `int*` key based on the value.
    // This is a major assumption for this step.
    
    // For the wrapper, we pass the value. The C function `delete_fib_node`
    // will need to iterate, find the node whose `*(int*)key == value`, then free that key and node.
    // The `void* data` parameter in `delete_fib_node(fh, data)` must be interpreted carefully.
    // If it's meant to be the *value* to delete, the C function needs to do the search.
    // If it's meant to be the *exact pointer* that was stored, then Python side cannot easily get this
    // without an internal mapping.
    // The plan mentioned "delete_fib_node (value-based delete)". So we assume the C function will do the search.
    // We don't pass `&val` because `delete_fib_node` would try to free `&val` which is on stack.
    // The `delete_fib_node` must handle the freeing of the *actual stored int pointer*.
    // We are essentially passing the value `(void*)(intptr_t)val` to avoid pointer issues here,
    // and expecting C side to search for node `n` where `*(int*)(n->key) == (int)val`.
    // This is still not quite right, as `delete_fib_node` expects a `void*` that is a key.
    // The most robust way is if `delete_fib_node` accepts an `int` directly if it's specialized,
    // or if it accepts the `void* key_to_find_and_delete` and it iterates.
    // Let's assume `delete_fib_node` is like: `bool delete_fib_node(Fibonacci_Heap *fh, void *key_value_to_delete)`
    // where `key_value_to_delete` is actually a pointer to the value, not the value itself.
    // This is getting complicated due to `void*`.
    // The `test_delete_node` in C uses `delete_node_fib_heap(heap, node_to_delete)` which takes a node.
    // The header also has `bool delete_fib_node(Fibonacci_Heap *fh, void *data);`
    // Let's assume `data` for `delete_fib_node` is a pointer to the key value to be deleted,
    // and the function will find the *actual stored int pointer* and free it.
    
    // This is the most problematic function to wrap without knowing `delete_fib_node`'s exact behavior
    // with respect to `void*` and memory management of `int*`.
    // For now, let's assume `delete_fib_node` will search for a node containing an `int*` that points to `val`.
    // This is a placeholder call, the actual C implementation of `delete_fib_node` is critical.
    // We are passing a pointer to 'val' which is on the stack. This is not what we want to delete or free.
    // The C function needs to search for a node N where *(int*)N->key == val.
    // A temporary key pointer for the value.
    int temp_val = (int)val;
    if (!delete_fib_node(self->fh, &temp_val)) { // This assumes delete_fib_node searches by value comparison
                                              // AND correctly frees the *stored* int* key.
        PyErr_SetString(PyExc_RuntimeError, "Failed to delete from Fibonacci Heap (or value not found).");
        return NULL;
    }

    Py_RETURN_NONE;
}


// update_key(self, old_value, new_value)
static PyObject *
FibHeap_update_key(FibHeapObject *self, PyObject *args) {
    long old_val_long, new_val_long;
    if (!PyArg_ParseTuple(args, "ll", &old_val_long, &new_val_long)) {
        return NULL;
    }

    if (self->fh == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Heap not initialized.");
        return NULL;
    }

    // Similar to delete, `change_fib_node_value` needs to handle finding the node
    // with `old_val`, free its existing `int*` key, and then store a new `int*` for `new_val`.
    // The parameters `void* old_val, void* new_val` for `change_fib_node_value` in `fibonacci_heap.h`
    // are also ambiguous. Are they values or pointers to values?
    // Assuming they are pointers to values for searching and the new value to adopt.
    
    int old_key_val = (int)old_val_long;
    
    // The new key needs to be dynamically allocated as it will be stored in the heap.
    int *new_key_ptr = (int *)malloc(sizeof(int));
    if (new_key_ptr == NULL) {
        return PyErr_NoMemory();
    }
    *new_key_ptr = (int)new_val_long;

    // This call assumes `change_fib_node_value` will:
    // 1. Find a node N such that `*(int*)(N->key) == old_key_val`.
    // 2. If found, `free(N->key)`.
    // 3. Set `N->key = new_key_ptr`.
    // 4. Perform heap adjustments (decrease_key logic).
    // If `change_fib_node_value` fails, it should not have adopted `new_key_ptr`, so we free it.
    if (!change_fib_node_value(self->fh, &old_key_val, new_key_ptr)) {
        free(new_key_ptr);
        PyErr_SetString(PyExc_RuntimeError, "Failed to update key in Fibonacci Heap (or old value not found).");
        return NULL;
    }

    // If successful, new_key_ptr is now owned by the heap.
    Py_RETURN_NONE;
}

// __len__
static Py_ssize_t
FibHeap_len(FibHeapObject *self) {
    if (self->fh == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Heap not initialized.");
        return -1; // Error indicator for sequence protocol
    }
    return (Py_ssize_t)self->fh->n;
}


// --- Method Definitions Table ---
static PyMethodDef FibHeap_methods[] = {
    {"insert", (PyCFunction)FibHeap_insert, METH_VARARGS, "Insert a value into the heap."},
    {"get_min", (PyCFunction)FibHeap_get_min, METH_NOARGS, "Get the minimum value from the heap."},
    {"extract_min", (PyCFunction)FibHeap_extract_min, METH_NOARGS, "Extract the minimum value from the heap."},
    {"delete", (PyCFunction)FibHeap_delete, METH_VARARGS, "Delete a value from the heap."},
    {"update_key", (PyCFunction)FibHeap_update_key, METH_VARARGS, "Update a key from old_value to new_value."},
    // {"decrease_key", ...} // Could be added if a node reference mechanism is implemented
    {NULL}  /* Sentinel */
};

// --- Sequence Methods (for __len__) ---
static PySequenceMethods FibHeap_as_sequence = {
    (lenfunc)FibHeap_len, // sq_length
    0,                    // sq_concat
    0,                    // sq_repeat
    0,                    // sq_item
    0,                    // sq_slice
    0,                    // sq_ass_item
    0,                    // sq_ass_slice
    0,                    // sq_contains
    0,                    // sq_inplace_concat
    0,                    // sq_inplace_repeat
};

// --- Type Definition ---
static PyTypeObject FibHeapType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fibheap.FibHeap",
    .tp_doc = "Fibonacci Heap object",
    .tp_basicsize = sizeof(FibHeapObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = FibHeap_new,
    .tp_dealloc = (destructor)FibHeap_dealloc,
    .tp_repr = (reprfunc)FibHeap_repr,
    .tp_methods = FibHeap_methods,
    .tp_as_sequence = &FibHeap_as_sequence,
    // tp_init, etc. can be added if FibHeap_new is split into tp_new and tp_init
};

// --- Module Definition ---
static PyModuleDef fibheapmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "fibheap",
    .m_doc = "A Python C extension for a Fibonacci Heap.",
    .m_size = -1,
    /* .m_methods = NULL, // No module-level functions, only the type */
};

// --- Module Initialization Function ---
PyMODINIT_FUNC
PyInit_fibheap(void) {
    PyObject *m;
    if (PyType_Ready(&FibHeapType) < 0)
        return NULL;

    m = PyModule_Create(&fibheapmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&FibHeapType);
    if (PyModule_AddObject(m, "FibHeap", (PyObject *)&FibHeapType) < 0) {
        Py_DECREF(&FibHeapType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
