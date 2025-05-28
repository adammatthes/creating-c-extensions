#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h> // Added for malloc
#include <string.h> // Added for memset
#include <math.h>   // Added for log2 (though using fixed size array for now)
#include <limits.h> // Added for INT_MIN
#include <stdio.h>  // Added for fprintf in delete_fib_node
#include "fibonacci_heap.h"

// Struct definitions are now in fibonacci_heap.h

// Forward declarations for helper functions
static void link_fib_nodes(Fibonacci_Heap *fh, Fibonacci_Node *y, Fibonacci_Node *x);
static void consolidate_fib_heap(Fibonacci_Heap *fh);
static void cut_fib_node(Fibonacci_Heap *fh, Fibonacci_Node *x, Fibonacci_Node *y);
static void cascading_cut_fib_node(Fibonacci_Heap *fh, Fibonacci_Node *y);
static Fibonacci_Node* find_node_by_value_recursive(Fibonacci_Node *start_node, int value_to_find, Fibonacci_Node *head_of_list_to_avoid_revisit_in_circular_search);

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
    // Assumes data is a dynamically allocated int* from the wrapper
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

// Function to delete a specific node from the Fibonacci heap
bool delete_node_fib_heap(Fibonacci_Heap *fh, Fibonacci_Node *node) {
    // a. Handle NULL inputs
    if (fh == NULL || node == NULL) {
        return false;
    }

    // b. Create a "negative infinity" key
    int *neg_inf_key = (int *)malloc(sizeof(int));
    if (!neg_inf_key) {
        return false; // Memory allocation failed
    }
    *neg_inf_key = INT_MIN;

    // c. Call decrease_key_fib_heap(fh, node, neg_inf_key)
    // Note: The current decrease_key_fib_heap does not free the old key.
    // This is a potential memory leak if the original key of 'node' was dynamically allocated
    // and is not referenced elsewhere. For the purpose of this function, we assume
    // that the key being replaced by neg_inf_key will be managed or is not a leak concern here,
    // or that decrease_key should ideally handle freeing the old key if it takes ownership of the new one.
    // Given the current API, `node->key` is simply overwritten.
    // Let's store the old key to free it *if* decrease_key succeeds and the node's key was changed.
    // However, the problem asks to free neg_inf_key, implying it's the one passed around.
    // The current decrease_key will make node->key = neg_inf_key.
    
    void* old_key_of_node = node->key; // Store to potentially free if it's orphaned.
                                     // This step is complex because we don't know if old_key_of_node
                                     // should be freed by this function. Typically, data stored in a generic
                                     // data structure is managed by the user.
                                     // For the specific case of delete, the node (and its key) is being removed.
                                     // If extract_min frees the node structure but returns the key,
                                     // and if this key was `neg_inf_key`, then `neg_inf_key` is freed.
                                     // What about the original `old_key_of_node`? It's now orphaned.
                                     // This suggests that keys should probably always be freed by extract_min
                                     // if they are considered "owned" by the heap.
                                     // For now, let's follow the exact instructions which focus on neg_inf_key.
    // MODIFIED logic for delete_node_fib_heap:
    
    void* original_key = node->key; // 1. Store original key

    // 2. Create a new int* for INT_MIN
    int *temp_min_key = (int *)malloc(sizeof(int));
    if (temp_min_key == NULL) {
        return false; // Allocation failed
    }
    *temp_min_key = INT_MIN;

    // 3. Call decrease_key_fib_heap. decrease_key_fib_heap should not free the old key (original_key).
    // It just replaces node->key with temp_min_key.
    if (!decrease_key_fib_heap(fh, node, (void *)temp_min_key)) {
        free(temp_min_key); // Decrease key failed, free the allocated temp_min_key
        // original_key is still in the node.
        return false;
    }
    // Now node->key is temp_min_key. The original_key is orphaned by the node.

    // 4. Free the original key
    if (original_key != NULL) { // Should always be true if node was valid
        free(original_key);
    }

    // 5. Call extract_min_fib_heap. This will extract the node with temp_min_key.
    void *extracted_key_ptr = extract_min_fib_heap(fh);

    // 6. Free the INT_MIN key that was extracted.
    if (extracted_key_ptr == temp_min_key) {
        free(extracted_key_ptr);
    } else {
        // This case indicates a problem: extract_min didn't return the temp_min_key.
        // This could happen if another INT_MIN key was already in the heap or if decrease_key failed silently
        // or if extract_min logic is flawed.
        // We must free temp_min_key if it wasn't the one returned to avoid a leak.
        if (temp_min_key != NULL) free(temp_min_key);
        // And if extracted_key_ptr is something else, the caller of extract_min (this function)
        // is responsible for it. If it's a user key, this would be a bug in logic.
        // For now, we assume extracted_key_ptr IS temp_min_key.
        // If it's not, we've already freed temp_min_key, and we let extracted_key_ptr leak if it's
        // not a key that should be freed here (e.g. if it was another node's key, but that's very unlikely).
        // This path suggests an error, so returning false might be appropriate or logging.
        // For now, stick to freeing the temp_min_key if it wasn't returned.
    }
    
    return true;
}

// Function to delete a node by its data content (value)
// 'data' is assumed to be an int* pointing to the value to delete.
bool delete_fib_node(Fibonacci_Heap *fh, void *data) {
    if (fh == NULL || data == NULL || fh->root_list == NULL) {
        return false;
    }
    int value_to_delete = *(int*)data;

    Fibonacci_Node *node_to_delete = find_node_by_value_recursive(fh->root_list, value_to_delete, fh->root_list);

    if (node_to_delete == NULL) {
        return false; // Node not found
    }

    return delete_node_fib_heap(fh, node_to_delete);
}

// Function to change the value of a node in the Fibonacci heap
// old_val_ptr is an int* pointing to the value to find.
// new_key_ptr_to_adopt is a new int* (allocated by wrapper) to be adopted by the node.
bool change_fib_node_value(Fibonacci_Heap *fh, void *old_val_ptr, void *new_key_ptr_to_adopt) {
    if (fh == NULL || old_val_ptr == NULL || new_key_ptr_to_adopt == NULL || fh->root_list == NULL) {
        if (new_key_ptr_to_adopt != NULL && old_val_ptr == NULL) { 
            // If old_val_ptr is NULL, node cannot be found, new_key_ptr_to_adopt would leak if not freed by caller (wrapper).
            // This case implies an error in arguments. The Python wrapper should handle freeing new_key_ptr_to_adopt if this function fails early.
        }
        return false;
    }

    int value_to_find = *(int*)old_val_ptr;
    int new_value = *(int*)new_key_ptr_to_adopt;

    Fibonacci_Node *node_to_change = find_node_by_value_recursive(fh->root_list, value_to_find, fh->root_list);

    if (node_to_change == NULL) {
        // Node not found. Wrapper is responsible for freeing new_key_ptr_to_adopt.
        return false;
    }

    // Store the original key of the node to free it after replacement.
    void* original_node_key = node_to_change->key;
    int original_node_key_value = *(int*)original_node_key; // Value before change

    // Adopt the new key.
    // node_to_change->key = new_key_ptr_to_adopt; // This is done by decrease_key or manually if increase

    if (new_value < original_node_key_value) {
        // New value is smaller, use decrease_key logic.
        // decrease_key_fib_heap will set node_to_change->key = new_key_ptr_to_adopt.
        if (decrease_key_fib_heap(fh, node_to_change, new_key_ptr_to_adopt)) {
            free(original_node_key); // Free the old key after successful decrease_key
            return true;
        } else {
            // decrease_key failed. The node's key is unchanged.
            // Wrapper is responsible for freeing new_key_ptr_to_adopt.
            return false;
        }
    } else if (new_value > original_node_key_value) {
        // New value is larger (increase key). This is complex.
        // Standard way: delete the node and re-insert the new value.
        // delete_node_fib_heap will free original_node_key.
        // We need to pass a pointer to new_value for re-insertion, which is new_key_ptr_to_adopt.
        // This means delete_node_fib_heap must NOT free new_key_ptr_to_adopt if it's passed in somehow.
        // This is tricky. Let's simplify:
        // 1. Free the old key.
        // 2. Set the node's key to the new key.
        // 3. If parent exists and heap property violated (child > parent), it's an error or needs complex fixing.
        //    Fibonacci heaps are not optimized for increase_key.
        //    For now, we will just update the key. If this breaks heap structure for parent,
        //    that's a limitation. A full "increase key" would involve removing from parent,
        //    potentially adding to root list, and then potentially a cascading cut upwards on parent.
        //    This is beyond typical decrease_key logic.
        //    A simpler model for increase key: delete and re-insert.
        //    Let's try delete and re-insert.
        
        // The value `value_to_find` (which is `original_node_key_value`) is what we need to delete.
        // We already have the `node_to_change`.
        if (delete_node_fib_heap(fh, node_to_change)) { // This will free original_node_key
            // Now re-insert the new_key_ptr_to_adopt
            if (insert_fib_heap(fh, new_key_ptr_to_adopt)) {
                return true; // Successfully deleted and re-inserted
            } else {
                // Insertion failed. This is problematic. new_key_ptr_to_adopt was not inserted.
                // Wrapper should free new_key_ptr_to_adopt.
                // The heap is now in a state where a node was deleted.
                return false; // Indicate error.
            }
        } else {
            // Deletion failed. new_key_ptr_to_adopt has not been used. Wrapper should free it.
            return false;
        }
    } else { // new_value == original_node_key_value
        // Values are the same. No change needed.
        // However, new_key_ptr_to_adopt was allocated by wrapper and needs to be freed as it won't be used.
        // This function should not free it; the wrapper should if this returns true (or false) without adopting.
        // For consistency, if no operation happens, the wrapper should free new_key_ptr_to_adopt.
        // Let's return true, but the wrapper needs to know new_key_ptr_to_adopt wasn't consumed.
        // This path is tricky for memory management of new_key_ptr_to_adopt.
        // The Python wrapper will only free new_key_ptr_to_adopt if change_key fails.
        // If it succeeds, it assumes new_key_ptr_to_adopt is consumed.
        // So, if values are same, we must "consume" new_key_ptr_to_adopt and free original_node_key.
        node_to_change->key = new_key_ptr_to_adopt;
        free(original_node_key);
        return true;
    }
}

// Function to get the minimum key from the Fibonacci heap
void *get_min(Fibonacci_Heap *fh) {
    if (fh == NULL || fh->min == NULL) {
        return NULL; // Heap is empty or invalid
    }
    return fh->min->key;
}

// Helper function to link node y to node x as a child of x
static void link_fib_nodes(Fibonacci_Heap *fh, Fibonacci_Node *y, Fibonacci_Node *x) {
    // a. Remove y from the root list
    y->left->right = y->right;
    y->right->left = y->left;

    // If y was fh->root_list, and y is not the only node, update fh->root_list
    // This specific check might be better handled in consolidate or extract_min
    // For now, we assume fh->root_list will be correctly repointed by the caller (consolidate)
    // if y happened to be the fh->root_list.

    // b. Make y a child of x
    y->parent = x;
    if (x->child == NULL) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        // Add y to x's child list (insert y to the right of x->child)
        y->right = x->child->right;
        y->left = x->child;
        x->child->right->left = y;
        x->child->right = y;
    }

    // c. Increment x->degree
    x->degree++;

    // d. Set y->marked = false
    y->marked = false;
}

// Helper function to consolidate the root list
static void consolidate_fib_heap(Fibonacci_Heap *fh) {
    if (fh == NULL || fh->root_list == NULL) {
        return;
    }

    // Max degree is roughly log_phi(n). For simplicity, use a fixed large enough array.
    // D(n) <= log_phi(n). For n = 2^64 (max for unsigned long long), log_phi(n) approx 92.
    // A more common practical limit for competitive programming might be n ~ 10^6, log_phi(10^6) ~ 28.
    // Let's use 64 as a fixed size, which should be safe for typical integer counts.
    const int MAX_DEGREE_ESTIMATE = 64; // Max degree for n up to approx 2^44
    Fibonacci_Node *A[MAX_DEGREE_ESTIMATE];
    memset(A, 0, sizeof(A)); // Initialize A with NULLs (0 for pointers)

    Fibonacci_Node *current_root = fh->root_list;
    if (current_root == NULL) return;

    // Need to iterate through all root nodes.
    // Since the list is circular and modified during iteration (nodes removed by link_fib_nodes),
    // it's safer to collect them first or carefully manage iteration.
    // Let's create a temporary array of root nodes to iterate over.
    // Count root nodes first
    int num_roots = 0;
    Fibonacci_Node *temp = current_root;
    do {
        num_roots++;
        temp = temp->right;
    } while (temp != current_root);

    Fibonacci_Node **roots_to_process = (Fibonacci_Node **)malloc(num_roots * sizeof(Fibonacci_Node *));
    if (roots_to_process == NULL) {
        // Memory allocation failed, cannot proceed with consolidation robustly.
        // This is a critical error. For now, just return, but a real system might need error handling.
        return;
    }
    temp = current_root;
    for (int i = 0; i < num_roots; i++) {
        roots_to_process[i] = temp;
        temp = temp->right;
    }

    // b. For each node w in the original root list
    for (int i = 0; i < num_roots; i++) {
        Fibonacci_Node *x = roots_to_process[i];

        // Ensure x is still in the root list (it might have been linked as a child)
        // A simple check: if x has a parent, it's no longer a root in the main list.
        if (x->parent != NULL) {
            continue;
        }

        int d = x->degree;
        while (A[d] != NULL) {
            Fibonacci_Node *y = A[d]; // y is the other node with the same degree

            // Ensure x and y are different nodes. This should always be true if logic is correct.
            if (x == y) {
                 // This case should ideally not happen if nodes are processed correctly.
                 // If it does, break to avoid infinite loop.
                break;
            }

            if (*(int *)(x->key) > *(int *)(y->key)) {
                Fibonacci_Node *temp_node = x;
                x = y;
                y = temp_node;
            }
            // Now x->key <= y->key, so y will become child of x
            link_fib_nodes(fh, y, x); // y is removed from root list here
                                      // x's degree is incremented inside link_fib_nodes
            A[d] = NULL;
            d++; // Degree of x has increased
            if (d >= MAX_DEGREE_ESTIMATE) {
                // Degree exceeds array bounds, something is wrong or MAX_DEGREE_ESTIMATE is too small.
                // Handle error or resize A if it were dynamic. For fixed size, this is an issue.
                // This could happen if MAX_DEGREE_ESTIMATE is too small for fh->n.
                // For now, we'll assume it's large enough.
                break; // Avoid array out of bounds
            }
        }
        A[d] = x;
    }
    free(roots_to_process);

    // c. Reconstruct the root list from array A
    fh->min = NULL;
    fh->root_list = NULL; // Reset root list

    for (int d = 0; d < MAX_DEGREE_ESTIMATE; d++) {
        if (A[d] != NULL) {
            Fibonacci_Node *node_to_add = A[d];
            // Ensure node_to_add is properly a root
            node_to_add->parent = NULL;
            // Add A[d] to the root list
            if (fh->root_list == NULL) {
                fh->root_list = node_to_add;
                node_to_add->left = node_to_add;
                node_to_add->right = node_to_add;
            } else {
                // Insert node_to_add to the right of fh->root_list
                node_to_add->right = fh->root_list->right;
                node_to_add->left = fh->root_list;
                fh->root_list->right->left = node_to_add;
                fh->root_list->right = node_to_add;
            }

            // Update fh->min
            if (fh->min == NULL || *(int *)(node_to_add->key) < *(int *)(fh->min->key)) {
                fh->min = node_to_add;
            }
        }
    }
    // If fh->root_list was reconstructed, fh->min is now set.
    // If the heap became empty (e.g. extracting last element, though consolidate is not directly for that)
    // then fh->min and fh->root_list will be NULL.
    // The problem statement said "fh->root_list points to a valid node ... (e.g. fh->min)"
    // If fh->min is set, fh->root_list can be fh->min.
    // Our current reconstruction makes fh->root_list point to the *last* node added from A.
    // It's fine as long as it's a valid entry point.
    // For consistency, let's make fh->root_list = fh->min if fh->min is not NULL.
    if (fh->min != NULL) {
        fh->root_list = fh->min;
    } else {
        fh->root_list = NULL; // Should already be NULL if fh->min is NULL
    }
}

// Function to decrease the key of a node in the Fibonacci heap
// new_key is an int* that will be adopted by the node.
// This function does NOT free the old node->key. The caller must do so.
bool decrease_key_fib_heap(Fibonacci_Heap *fh, Fibonacci_Node *node, void *new_key) {
    // a. Basic checks
    if (fh == NULL || node == NULL || new_key == NULL) {
        return false; // Invalid input
    }
    // It's possible that new_key is not strictly less, e.g. when setting to INT_MIN for delete
    // For a generic decrease_key, it must be less.
    // However, change_fib_node_value might call this even if new_key == old_key (which is fine).
    // The > check is important.
    if (node->key != NULL && *(int *)new_key > *(int *)(node->key)) { 
        // Only check if node->key is not NULL. If it was NULL, any new key is fine.
        // This path typically shouldn't be hit if called by change_fib_node_value correctly.
        return false; // New key is greater than current key
    }

    // b. Update the key (adopt the new_key pointer)
    // The old node->key is now orphaned by the node. Caller's responsibility.
    node->key = new_key;

    // c. Let y = node->parent
    Fibonacci_Node *y = node->parent;

    // d. If node is not a root and its key is now less than its parent's key
    if (y != NULL && (*(int *)(node->key) < *(int *)(y->key))) {
        // i. Call cut_fib_node(fh, node, y)
        cut_fib_node(fh, node, y);
        // ii. Call cascading_cut_fib_node(fh, y)
        cascading_cut_fib_node(fh, y);
    }

    // e. Update fh->min
    // This check is important even if the node was already a root, or if it became a root.
    if (fh->min == NULL || (*(int *)(node->key) < *(int *)(fh->min->key))) {
        fh->min = node;
    }

    // f. Return true
    return true;
}

// Function to extract the minimum key from the Fibonacci heap
void *extract_min_fib_heap(Fibonacci_Heap *fh) {
    if (fh == NULL) return NULL;

    // a. Let z be fh->min
    Fibonacci_Node *z = fh->min;

    // b. If z is NULL (heap is empty), return NULL
    if (z == NULL) {
        return NULL;
    }

    // c. Store z->key to be returned later
    void *min_key = z->key;
    bool z_had_children = (z->child != NULL); // Track if z initially had children

    // d. If z has children (i.e., z->child is not NULL):
    if (z_had_children) {
        Fibonacci_Node *child = z->child;
        Fibonacci_Node *first_child = child; // To detect cycle completion
        Fibonacci_Node *next_child;

        // Need to save children before modifying lists extensively
        // Count children first
        int num_children = 0;
        do {
            num_children++;
            child = child->right;
        } while (child != first_child);
        
        Fibonacci_Node **children_array = (Fibonacci_Node **)malloc(num_children * sizeof(Fibonacci_Node *));
        if (children_array == NULL && num_children > 0) {
            // Memory allocation failed, cannot proceed safely.
            // This is a critical error state. For now, return NULL as we can't correctly modify the heap.
            // This indicates an out-of-memory condition.
            // The key `min_key` is still valid from z, but the heap is now in a bad state.
            // To prevent use of a corrupted heap, maybe it's better to signal a fatal error.
            // For now, returning NULL for the key, though z and min_key are technically available.
            // This makes the function's behavior consistent on failure (return NULL).
            return NULL; 
        }

        child = first_child;
        for(int i=0; i < num_children; ++i) {
            children_array[i] = child;
            child = child->right;
        }

        for (int i = 0; i < num_children; ++i) {
            Fibonacci_Node *current_child_to_add = children_array[i];
            // i. Add current_child_to_add to the root list of fh
            if (fh->root_list == NULL) { // If root list is currently empty (e.g. z was the only root)
                fh->root_list = current_child_to_add;
                current_child_to_add->left = current_child_to_add;
                current_child_to_add->right = current_child_to_add;
            } else {
                // Insert current_child_to_add to the right of fh->root_list
                current_child_to_add->left = fh->root_list;
                current_child_to_add->right = fh->root_list->right;
                fh->root_list->right->left = current_child_to_add;
                fh->root_list->right = current_child_to_add;
            }
            // Set current_child_to_add->parent = NULL
            current_child_to_add->parent = NULL;
        }
        
        if(num_children > 0) free(children_array);

        // ii. Clear z->child (it no longer has children in the heap structure)
        z->child = NULL; // z->degree is not explicitly reset here as z is removed.
    }

    // e. Remove z from the root list
    if (z == z->right) { // z is the only node in the root list
        // This means if z had children, they are now the root list.
        // If z had no children, the root list is now empty.
        if (z_had_children) {
            // fh->root_list was set to one of the children (or is NULL if z had no children)
            // fh->min will be found by consolidate.
            // The loop above already made fh->root_list point to one of the children if any.
        } else {
            // z was the only node and had no children.
            fh->min = NULL;
            fh->root_list = NULL;
        }
    } else { // z has siblings in the root list
        z->left->right = z->right;
        z->right->left = z->left;
        
        // If fh->root_list was z, point it to another node
        if (fh->root_list == z) {
            fh->root_list = z->right;
        }
        // fh->min will be updated by consolidate. As a temporary measure,
        // we could set fh->min = fh->root_list, but consolidate handles this.
        // For robustness, let's ensure fh->min is not z if it's still pointing to it.
        if (fh->min == z) { // This will always be true here
            fh->min = fh->root_list; // Temporary new min, consolidate will find the actual one.
                                     // If fh->root_list became NULL (e.g. z was only node, children became root list, but no children), this is fine.
        }
    }

    // f. If z == z->right AND z->child == NULL (originally, not after children are moved)
    // This condition means the heap had only one node (z) and it had no children.
    // In this case, fh->root_list will be NULL at this point.
    if (fh->root_list == NULL) { // This is a better check for emptiness now
        fh->n = 0;
        // fh->min is already NULL
        free(z);
        return min_key;
    }

    // g. Call consolidate_fib_heap(fh)
    // consolidate_fib_heap will update fh->min and ensure fh->root_list points to the new min.
    consolidate_fib_heap(fh);

    // h. Decrement fh->n
    fh->n--;

    // i. Free the extracted node z
    free(z);

    // j. Return the stored key
    return min_key;
}

// Helper function to cut node x from its parent y
static void cut_fib_node(Fibonacci_Heap *fh, Fibonacci_Node *x, Fibonacci_Node *y) {
    // a. Remove x from y's child list
    if (x == x->right) { // x is the only child of y
        y->child = NULL;
    } else { // x has siblings
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right; // Update y's child pointer to another child
        }
    }

    // b. Decrement y->degree
    y->degree--;

    // c. Add x to the root list of fh
    // Initially, x becomes its own circular list
    x->left = x;
    x->right = x;

    if (fh->root_list == NULL) {
        fh->root_list = x;
        // fh->min would also be x here, but insert_fib_heap logic handles fh->min update
        // or it will be updated when next needed e.g. by consolidate or extract_min
    } else {
        // Insert x to the right of fh->root_list (which is often fh->min)
        x->right = fh->root_list->right;
        x->left = fh->root_list;
        fh->root_list->right->left = x;
        fh->root_list->right = x;
    }
    // If fh->min needs update, it's typically done by the caller or by consolidate.
    // However, if x's key is smaller than current fh->min, it should become fh->min.
    // This is not explicitly stated for cut, but good for consistency.
    // For now, let's assume fh->min update is handled by operations like decrease_key or delete,
    // which are the typical callers of cut/cascading_cut.
    // The primary role here is to move x to the root list.

    // d. Set x->parent = NULL
    x->parent = NULL;

    // e. Set x->marked = false
    x->marked = false;
}

// Helper function for cascading cuts
static void cascading_cut_fib_node(Fibonacci_Heap *fh, Fibonacci_Node *y) {
    // a. Let z = y->parent
    Fibonacci_Node *z = y->parent;

    // b. If z is not NULL (y is not a root)
    if (z != NULL) {
        // i. If y->marked is false
        if (y->marked == false) {
            y->marked = true;
        } else { // ii. Else (y->marked is true)
            cut_fib_node(fh, y, z);
            cascading_cut_fib_node(fh, z);
        }
    }
}

// Helper function to find a node by its integer value (recursive)
// 'start_node' is the node to begin search from in a list (e.g. fh->root_list or parent->child).
// 'value_to_find' is the integer value being searched for.
// 'list_head_marker' is used to detect when a circular list traversal is complete. It should be the same as 'start_node' for the initial call on a list.
static Fibonacci_Node* find_node_by_value_recursive(Fibonacci_Node *current_search_candidate, int value_to_find, Fibonacci_Node *list_head_marker) {
    if (current_search_candidate == NULL) {
        return NULL;
    }

    Fibonacci_Node *iter_node = current_search_candidate;
    Fibonacci_Node *found_node = NULL;

    do {
        // Check current node
        if (iter_node->key != NULL && *(int*)(iter_node->key) == value_to_find) {
            return iter_node; // Found the node
        }

        // Recursively search in children (if any)
        // For child list, the head marker for the child list traversal is iter_node->child itself.
        if (iter_node->child != NULL) {
            found_node = find_node_by_value_recursive(iter_node->child, value_to_find, iter_node->child);
            if (found_node != NULL) {
                return found_node; // Found in a descendant
            }
        }

        // Move to the next sibling in the current list
        iter_node = iter_node->right;
    } while (iter_node != list_head_marker); // Stop after traversing the circular list

    return NULL; // Not found in this list or its descendants
}


void destroy_fib_heap(Fibonacci_Heap *fh) {
    if (fh == NULL) {
        return;
    }
    // Ensure fh->n is reliable. If not, this loop could be problematic.
    // extract_min_fib_heap should decrement fh->n.
    while (fh->n > 0 && fh->min != NULL) { // Added fh->min != NULL for extra safety
        void *key_ptr = extract_min_fib_heap(fh); // This should return the int* key
        if (key_ptr != NULL) {
            free(key_ptr); // Free the int* allocated by the wrapper's insert
        } else {
            // If extract_min_fib_heap returns NULL but fh->n > 0, something is wrong.
            // This might indicate heap corruption or a bug in extract_min_fib_heap.
            // To prevent an infinite loop, break. Consider logging an error.
            fprintf(stderr, "Warning: destroy_fib_heap encountered NULL key from extract_min_heap while n > 0.\n");
            break; 
        }
    }
    // All nodes and their keys should have been freed via extract_min_fib_heap.
    // Now free the heap structure itself.
    fh->min = NULL; // Defensive nulling
    fh->root_list = NULL; // Defensive nulling
    fh->n = 0; // Defensive
    free(fh);
}
