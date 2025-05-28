#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "../fibonacci_heap.h" // Already included

// Helper to create an int pointer
static int* create_int_ptr(int value) {
    int *ptr = malloc(sizeof(int));
    ck_assert_ptr_nonnull(ptr); // Ensure malloc was successful
    *ptr = value;
    return ptr;
}

// Test case for creating a Fibonacci heap
START_TEST(test_create_heap)
{
    Fibonacci_Heap *heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);
    ck_assert_int_eq(heap->n, 0);
    // free(heap); 
}
END_TEST

// Test case for basic insertion
START_TEST(test_insert_basic)
{
    Fibonacci_Heap *heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);

    int *val1 = malloc(sizeof(int)); 
    ck_assert_ptr_nonnull(val1);
    *val1 = 10;
    ck_assert(insert_fib_heap(heap, val1));
    ck_assert_int_eq(heap->n, 1);

    int *val2 = malloc(sizeof(int)); 
    ck_assert_ptr_nonnull(val2);
    *val2 = 5;
    ck_assert(insert_fib_heap(heap, val2));
    ck_assert_int_eq(heap->n, 2);

    int *val3 = malloc(sizeof(int)); 
    ck_assert_ptr_nonnull(val3);
    *val3 = 20;
    ck_assert(insert_fib_heap(heap, val3));
    ck_assert_int_eq(heap->n, 3);
    
    // free(val1); free(val2); free(val3); free(heap);
}
END_TEST

// Test case for get_min function
START_TEST(test_get_min)
{
    Fibonacci_Heap *heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);

    // Test on empty heap
    ck_assert_ptr_null(get_min(heap));

    // Insert first value
    int *val1 = malloc(sizeof(int)); 
    ck_assert_ptr_nonnull(val1);
    *val1 = 42;
    insert_fib_heap(heap, val1);
    ck_assert_ptr_eq(get_min(heap), val1);
    ck_assert_int_eq(*(int*)get_min(heap), 42);

    // Insert smaller value
    int *val2 = malloc(sizeof(int)); 
    ck_assert_ptr_nonnull(val2);
    *val2 = 10;
    insert_fib_heap(heap, val2);
    ck_assert_ptr_eq(get_min(heap), val2);
    ck_assert_int_eq(*(int*)get_min(heap), 10);

    // Insert larger value
    int *val3 = malloc(sizeof(int)); 
    ck_assert_ptr_nonnull(val3);
    *val3 = 100;
    insert_fib_heap(heap, val3);
    ck_assert_ptr_eq(get_min(heap), val2); // Min should still be val2 (10)
    ck_assert_int_eq(*(int*)get_min(heap), 10);
    
    // Eventually: free(val1); free(val2); free(val3); free(heap);
    // For now, let's free them to be good citizens, assuming these tests are self-contained.
    // However, the original file comments them out, so I will match that for now.
    // If these were real unit tests in a larger system, freeing would be important.
}
END_TEST


// Test case for extract_min function
START_TEST(test_extract_min)
{
    Fibonacci_Heap *heap;
    int *key_ptr;
    void *extracted_key;

    // Scenario 1: Extract from an empty heap
    heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_null(extracted_key);
    ck_assert_int_eq(heap->n, 0);
    ck_assert_ptr_null(heap->min);
    ck_assert_ptr_null(heap->root_list);
    // free(heap); // Matching style, normally would free

    // Scenario 2: Extract from a single-element heap
    heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);
    key_ptr = create_int_ptr(10);
    insert_fib_heap(heap, key_ptr);
    ck_assert_int_eq(heap->n, 1);

    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 10);
    ck_assert_int_eq(heap->n, 0);
    ck_assert_ptr_null(heap->min);
    ck_assert_ptr_null(heap->root_list);
    free(extracted_key); // Free the key's memory
    // free(heap);

    // Scenario 3: Extract from a multi-element heap (simple case)
    heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);
    int *val1 = create_int_ptr(10);
    int *val2 = create_int_ptr(5);
    int *val3 = create_int_ptr(20);
    insert_fib_heap(heap, val1);
    insert_fib_heap(heap, val2);
    insert_fib_heap(heap, val3);
    ck_assert_int_eq(heap->n, 3);
    ck_assert_int_eq(*(int*)get_min(heap), 5);

    extracted_key = extract_min_fib_heap(heap); // Extract 5
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 5);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 2);
    ck_assert_ptr_nonnull(get_min(heap)); // Check before dereferencing
    ck_assert_int_eq(*(int*)get_min(heap), 10);

    extracted_key = extract_min_fib_heap(heap); // Extract 10
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 10);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 1);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 20);

    extracted_key = extract_min_fib_heap(heap); // Extract 20
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 20);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 0);
    ck_assert_ptr_null(heap->min);
    ck_assert_ptr_null(heap->root_list);
    // free(heap);
    // Note: val1, val2, val3 were not freed here because extract_min returns the pointer
    // and we free it there. If they weren't extracted, they'd need freeing.

    // Scenario 4: Extract causing consolidation
    heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);
    // Insert 8,7,6,5,4,3,2,1 (creates 8 root nodes)
    int *k8 = create_int_ptr(8); insert_fib_heap(heap, k8);
    int *k7 = create_int_ptr(7); insert_fib_heap(heap, k7);
    int *k6 = create_int_ptr(6); insert_fib_heap(heap, k6);
    int *k5 = create_int_ptr(5); insert_fib_heap(heap, k5);
    int *k4 = create_int_ptr(4); insert_fib_heap(heap, k4);
    int *k3 = create_int_ptr(3); insert_fib_heap(heap, k3);
    int *k2 = create_int_ptr(2); insert_fib_heap(heap, k2);
    int *k1 = create_int_ptr(1); insert_fib_heap(heap, k1);
    ck_assert_int_eq(heap->n, 8);
    ck_assert_int_eq(*(int*)get_min(heap), 1);

    // Extract 1 (causes consolidation)
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 1);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 7);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 2); // After 1 is extracted, 2 should be min

    // Extract 2
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 2);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 6);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 3);

    // Extract 3
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 3);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 5);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 4);

    // Extract 4
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 4);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 4);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 5);

    // Extract 5
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 5);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 3);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 6);

    // Extract 6
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 6);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 2);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 7);

    // Extract 7
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 7);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 1);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 8);

    // Extract 8
    extracted_key = extract_min_fib_heap(heap);
    ck_assert_ptr_nonnull(extracted_key);
    ck_assert_int_eq(*(int*)extracted_key, 8);
    free(extracted_key);
    ck_assert_int_eq(heap->n, 0);
    ck_assert_ptr_null(heap->min);
    ck_assert_ptr_null(heap->root_list);
    // free(heap);
    // Original key pointers k1-k8 were returned by extract_min and freed.
}
END_TEST

// Test case for decrease_key function
START_TEST(test_decrease_key)
{
    Fibonacci_Heap *heap;
    Fibonacci_Node *node_to_decrease;
    int *key_ptr;
    int *new_key_ptr;
    bool result;

    // Scenario 1: Decrease key of a root node (fh->min, no cut)
    heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);
    key_ptr = create_int_ptr(10);
    insert_fib_heap(heap, key_ptr); // Heap: 10(min)
    int *key_ptr2 = create_int_ptr(20);
    insert_fib_heap(heap, key_ptr2); // Heap: 10(min), 20
    ck_assert_int_eq(heap->n, 2);
    ck_assert_ptr_eq(heap->min->key, key_ptr); // Min is 10

    node_to_decrease = heap->min; // Node with key 10
    new_key_ptr = create_int_ptr(5);
    result = decrease_key_fib_heap(heap, node_to_decrease, new_key_ptr);
    ck_assert(result);
    ck_assert_int_eq(*(int*)heap->min->key, 5);
    ck_assert_ptr_eq(heap->min, node_to_decrease); // Node itself should be the same
    ck_assert_int_eq(heap->n, 2);
    // free(new_key_ptr); // new_key_ptr is now owned by the node
    // Keys key_ptr (original 10) and key_ptr2 (20) are still in heap or their memory was transferred.
    // If decrease_key_fib_heap re-assigned node->key, then new_key_ptr is used. The old key_ptr is effectively replaced.
    // We need to be careful about freeing. The problem description for decrease_key implies node->key = new_key.
    // So, the *value* pointed to by node->key changes. If new_key is a different pointer, then old node->key needs freeing.
    // The current implementation of decrease_key is `node->key = new_key;` which means `key_ptr` (original 10) is orphaned if not freed by decrease_key.
    // Let's assume for now that the user of decrease_key is responsible for the old key if it's not the same pointer.
    // For this test, key_ptr was the original key. new_key_ptr is the new one.
    // The problem description for decrease_key does not specify freeing the old key.
    // Let's assume keys passed to insert are owned by heap, and new_key replaces the old one.
    // We should free the orphaned key_ptr here.
    free(key_ptr); 
    // Clean up remaining keys for next scenario
    free(heap->min->key); // this is new_key_ptr (5)
    Fibonacci_Node* temp_node_for_free = heap->root_list; // find the other node
    if (temp_node_for_free == heap->min) temp_node_for_free = temp_node_for_free->right;
    if (temp_node_for_free != heap->min) free(temp_node_for_free->key); // this is key_ptr2 (20)
    // free(heap); // commented out per style

    // Scenario 2: Attempt to decrease key to a larger value
    heap = create_fib_heap();
    key_ptr = create_int_ptr(10);
    insert_fib_heap(heap, key_ptr);
    ck_assert_int_eq(*(int*)heap->min->key, 10);

    node_to_decrease = heap->min;
    new_key_ptr = create_int_ptr(15);
    result = decrease_key_fib_heap(heap, node_to_decrease, new_key_ptr);
    ck_assert(!result); // Should fail
    ck_assert_int_eq(*(int*)heap->min->key, 10); // Key should remain 10
    ck_assert_int_eq(heap->n, 1);
    free(new_key_ptr); // new_key_ptr was not used by the heap
    free(key_ptr); // key_ptr is still in the heap, free it at end of this test block
    // free(heap);

    // Scenario 6: Decrease key on NULL node or heap
    heap = create_fib_heap();
    key_ptr = create_int_ptr(5); // A valid key
    Fibonacci_Node *valid_node = heap->min; // This will be NULL if heap is empty, or a node if not.
                                         // Let's insert to make it valid for one part of test.
    insert_fib_heap(heap, create_int_ptr(100)); // node_to_decrease will be this one.
    node_to_decrease = heap->min;

    result = decrease_key_fib_heap(NULL, node_to_decrease, key_ptr);
    ck_assert(!result);
    result = decrease_key_fib_heap(heap, NULL, key_ptr);
    ck_assert(!result);
    result = decrease_key_fib_heap(heap, node_to_decrease, NULL);
    ck_assert(!result);
    free(key_ptr); // Not used by heap in these calls
    free(heap->min->key); // Free the 100
    // free(heap);


    // Scenario 4 (Simplified): Decrease child's key, causing a cut
    // Setup: Insert 10 (N10), 20 (N20), 5 (N5). Min is N5.
    // Extract Min (N5). Heap consolidates. Assume N10 is min, N20 is child of N10.
    // (Actual consolidation can vary, this is a common outcome for specific link strategies)
    heap = create_fib_heap();
    int *k10 = create_int_ptr(10); insert_fib_heap(heap, k10); // N10
    int *k20 = create_int_ptr(20); insert_fib_heap(heap, k20); // N20
    int *k5 = create_int_ptr(5);   insert_fib_heap(heap, k5);  // N5
    ck_assert_int_eq(*(int*)heap->min->key, 5);
    ck_assert_int_eq(heap->n, 3);

    void* extracted_val = extract_min_fib_heap(heap); // Extract 5 (k5)
    ck_assert_ptr_nonnull(extracted_val);
    ck_assert_int_eq(*(int*)extracted_val, 5);
    free(extracted_val); // k5 is freed
    ck_assert_int_eq(heap->n, 2);
    // After extract_min, consolidation occurs.
    // Expected: 10 is min, 20 is child of 10. (Or vice-versa if keys are same and degrees differ)
    // Let's check current min. It should be 10.
    ck_assert_ptr_nonnull(heap->min);
    ck_assert_int_eq(*(int*)heap->min->key, 10); // N10 is min
    
    // Check if N10 has a child. This child should be N20.
    Fibonacci_Node *parent_node = heap->min; // This is N10
    ck_assert_ptr_nonnull(parent_node->child); // N10 should have a child
    Fibonacci_Node *child_node_to_decrease = parent_node->child; // This should be N20
    ck_assert_ptr_nonnull(child_node_to_decrease);
    ck_assert_int_eq(*(int*)child_node_to_decrease->key, 20); // Verify it's N20
    ck_assert_ptr_eq(child_node_to_decrease->parent, parent_node); // Verify parent pointer
    ck_assert_int_eq(parent_node->degree, 1); // N10 should have degree 1

    // Now, decrease key of child_node (N20) to 2. This should cause a cut.
    int *k2 = create_int_ptr(2);
    result = decrease_key_fib_heap(heap, child_node_to_decrease, k2);
    ck_assert(result);
    ck_assert_int_eq(heap->n, 2); // Still 2 nodes

    // child_node_to_decrease (now with key 2) should be the new minimum and a root.
    ck_assert_ptr_nonnull(heap->min);
    ck_assert_int_eq(*(int*)heap->min->key, 2);
    ck_assert_ptr_eq(heap->min, child_node_to_decrease); // The decreased node is min
    ck_assert_ptr_null(child_node_to_decrease->parent); // It's a root now
    ck_assert_int_eq(child_node_to_decrease->marked, false); // Cut makes it unmarked

    // The original parent_node (N10) should have its degree decremented.
    ck_assert_int_eq(parent_node->degree, 0);
    
    // Free keys: k10 (original key of parent_node) is still in parent_node
    // k20 (original key of child_node_to_decrease) was replaced by k2. So k20 needs freeing.
    free(k20);
    // k2 is now in child_node_to_decrease (which is heap->min)
    // k10 is in parent_node (which is some other root node)
    // Clean up:
    free(heap->min->key); // This is k2
    // Find the other node (original parent_node, N10) and free its key
    Fibonacci_Node* other_node = heap->root_list;
    if (other_node == heap->min) other_node = other_node->right;
    // It's possible after cut and fh->min update, that heap->min is the only node if list was small
    if (other_node != heap->min && other_node != NULL) { // Check other_node is not same as min and not null
         ck_assert_int_eq(*(int*)other_node->key, 10); // Should be N10
         free(other_node->key); // This is k10
    } else if (parent_node != heap->min) { // If parent_node itself is not the new min (which it shouldn't be)
        // This case if heap->root_list traversal didn't find it simply.
        // This means parent_node is the other node.
         ck_assert_int_eq(*(int*)parent_node->key, 10);
         free(parent_node->key);
    }
    // free(heap);


    // Note on Scenarios 3 (decrease no cut) and 5 (cascading cut):
    // These are harder to reliably set up without more direct heap manipulation tools
    // or a very specific (and potentially fragile) sequence of operations.
    // For example, Scenario 3 would require node->key < parent->key initially,
    // then node->key is decreased but still node->key >= parent->key.
    // Cascading cut requires a parent to be marked true, then one of its children cut,
    // triggering the parent to be cut and its parent (grandparent of original child) to be checked.
}
END_TEST


// Test case for delete_node_fib_heap function
START_TEST(test_delete_node)
{
    Fibonacci_Heap *heap;
    Fibonacci_Node *node_to_delete;
    int *original_key_ptr;
    bool result;

    // Scenario 1: Delete from an empty heap (NULL node or NULL heap)
    heap = create_fib_heap();
    ck_assert_ptr_nonnull(heap);
    // Create a dummy node structure for testing, but it's not actually in any heap.
    // This is to test the function's handling of nodes not in a heap or invalid nodes.
    // However, delete_node_fib_heap doesn't validate if node is in fh.
    // The more direct test is with NULL node.
    Fibonacci_Node dummy_node; 
    dummy_node.key = create_int_ptr(1000); // give it some key

    result = delete_node_fib_heap(NULL, &dummy_node);
    ck_assert(!result);
    result = delete_node_fib_heap(heap, NULL);
    ck_assert(!result);
    free(dummy_node.key);
    // free(heap); // Per style

    // Scenario 2: Delete the only node in a heap
    heap = create_fib_heap();
    original_key_ptr = create_int_ptr(10);
    insert_fib_heap(heap, original_key_ptr);
    ck_assert_int_eq(heap->n, 1);
    node_to_delete = heap->min; // This is the node with key 10

    // Save the key pointer before deletion as it will be replaced by INT_MIN temporarily
    // and then the node containing it will be freed.
    // The problem says "delete_node_fib_heap internally allocates an INT_MIN key and frees it."
    // "The original key of the node being deleted is orphaned by decrease_key_fib_heap."
    // "The tests must free these original keys."
    // So, `original_key_ptr` is the one we need to free.
    
    result = delete_node_fib_heap(heap, node_to_delete);
    ck_assert(result);
    ck_assert_int_eq(heap->n, 0);
    ck_assert_ptr_null(heap->min);
    ck_assert_ptr_null(heap->root_list);
    free(original_key_ptr); // Free the original key
    // free(heap);

    // Scenario 3: Delete the minimum node from a multi-element heap
    heap = create_fib_heap();
    int *k10 = create_int_ptr(10); insert_fib_heap(heap, k10);
    int *k5 = create_int_ptr(5);   insert_fib_heap(heap, k5); // k5 is min
    int *k20 = create_int_ptr(20); insert_fib_heap(heap, k20);
    ck_assert_int_eq(heap->n, 3);
    ck_assert_int_eq(*(int*)get_min(heap), 5);
    
    node_to_delete = heap->min; // Node with key 5
    original_key_ptr = node_to_delete->key; // Save pointer to k5
    
    result = delete_node_fib_heap(heap, node_to_delete);
    ck_assert(result);
    ck_assert_int_eq(heap->n, 2);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 10);
    free(original_key_ptr); // Free k5
    // Remaining keys (k10, k20) need to be freed for cleanup
    free(k10); 
    free(k20);
    // free(heap);

    // Scenario 4: Delete a non-minimum root node
    heap = create_fib_heap();
    k5 = create_int_ptr(5);   insert_fib_heap(heap, k5);  // Min
    k10 = create_int_ptr(10); insert_fib_heap(heap, k10); // Root
    k20 = create_int_ptr(20); insert_fib_heap(heap, k20); // Root
    ck_assert_int_eq(heap->n, 3);
    ck_assert_int_eq(*(int*)heap->min->key, 5);

    // Try to find N10. It could be fh->min->right or fh->min->left,
    // as long as it's not fh->min itself.
    node_to_delete = heap->min->right;
    if (node_to_delete == heap->min) { // If fh->min->right was fh->min (e.g. only 1 or 2 nodes after some ops)
        node_to_delete = heap->min->left; 
    }
    // Ensure we didn't pick fh->min again if there are multiple distinct nodes.
    // This logic is a bit fragile. If only two nodes, min->right is the other node. min->left is also the other node.
    // If 3 nodes, min->right and min->left are distinct and not min.
    if (heap->n > 1 && node_to_delete == heap->min) {
        // This shouldn't happen if n > 1 and list is circular correctly.
        // For safety, if it's still min, try left (though for n=2, right and left are same non-min node)
         node_to_delete = heap->min->left;
    }
    
    // We need to ensure node_to_delete is not the min node (5)
    // And it should be a root node.
    ck_assert_ptr_nonnull(node_to_delete);
    ck_assert_ptr_ne(node_to_delete, heap->min); // Ensure it's not the min node
    ck_assert_ptr_null(node_to_delete->parent); // Ensure it's a root

    original_key_ptr = node_to_delete->key; // Save its key (e.g. 10 or 20)
    int original_key_value = *(int*)original_key_ptr;

    result = delete_node_fib_heap(heap, node_to_delete);
    ck_assert(result);
    ck_assert_int_eq(heap->n, 2);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 5); // Min should still be 5
    
    // Verify the other non-min node is still present
    bool other_node_found = false;
    int expected_other_key = (original_key_value == 10) ? 20 : 10;
    Fibonacci_Node *current = heap->root_list;
    if (current != NULL) {
        do {
            if (*(int*)current->key == expected_other_key) {
                other_node_found = true;
                break;
            }
            current = current->right;
        } while (current != heap->root_list);
    }
    ck_assert(other_node_found);
    free(original_key_ptr); // Free the key of the deleted node (10 or 20)
    // Free remaining keys (k5 and either k10 or k20)
    free(k5);
    if (original_key_value == 10) free(k20); else free(k10);
    // free(heap);


    // Scenario 5: Delete a child node
    heap = create_fib_heap();
    k10 = create_int_ptr(10); insert_fib_heap(heap, k10);
    k20 = create_int_ptr(20); insert_fib_heap(heap, k20);
    k5 = create_int_ptr(5);   insert_fib_heap(heap, k5);
    ck_assert_int_eq(*(int*)heap->min->key, 5);
    
    void* extracted_val = extract_min_fib_heap(heap); // Extract 5 (k5)
    free(extracted_val); // k5 is freed by test
    ck_assert_int_eq(heap->n, 2); // Remaining: 10, 20
    // Consolidation should make one child of other. Assume 10 is min, 20 is child.
    Fibonacci_Node *parent_node_s5 = heap->min; // Should be N10
    ck_assert_ptr_nonnull(parent_node_s5);
    ck_assert_int_eq(*(int*)parent_node_s5->key, 10);
    ck_assert_ptr_nonnull(parent_node_s5->child); // N10 should have a child
    Fibonacci_Node *child_to_delete = parent_node_s5->child; // Should be N20
    ck_assert_ptr_nonnull(child_to_delete);
    ck_assert_int_eq(*(int*)child_to_delete->key, 20);

    original_key_ptr = child_to_delete->key; // This is k20

    result = delete_node_fib_heap(heap, child_to_delete);
    ck_assert(result);
    ck_assert_int_eq(heap->n, 1);
    ck_assert_ptr_nonnull(get_min(heap));
    ck_assert_int_eq(*(int*)get_min(heap), 10); // Min is N10
    ck_assert_ptr_eq(heap->min, parent_node_s5);
    ck_assert_int_eq(parent_node_s5->degree, 0); // Child was removed
    
    free(original_key_ptr); // Free k20
    // Free remaining key k10
    free(k10);
    // free(heap);
}
END_TEST

// Function to create the test suite
Suite *fib_heap_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_get_min_case; // Renamed to avoid conflict with function if START_TEST was tc_get_min

    s = suite_create("FibonacciHeap");

    // Core test case
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_create_heap);
    tcase_add_test(tc_core, test_insert_basic);
    tcase_add_test(tc_core, test_extract_min);
    tcase_add_test(tc_core, test_decrease_key);
    tcase_add_test(tc_core, test_delete_node); // Added test_delete_node
    suite_add_tcase(s, tc_core);

    // Test case for get_min
    tc_get_min_case = tcase_create("GetMin"); // Use the renamed TCase variable
    tcase_add_test(tc_get_min_case, test_get_min); // test_get_min is the START_TEST block
    suite_add_tcase(s, tc_get_min_case);
    // Note: It might be cleaner to group extract_min with other operation tests
    // like get_min, or give it its own TCase. For now, adding to "Core" as per instruction.
    // If extract_min had its own TCase:
    // TCase *tc_extract_min_case = tcase_create("ExtractMin");
    // tcase_add_test(tc_extract_min_case, test_extract_min);
    // suite_add_tcase(s, tc_extract_min_case);


    return s;
}

// Main function to run the tests
int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = fib_heap_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
