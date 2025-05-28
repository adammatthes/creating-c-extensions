import unittest
import fibheap # This will import the compiled C extension

class TestFibHeap(unittest.TestCase):

    def test_create_heap(self):
        h = fibheap.FibHeap()
        self.assertIsNotNone(h)
        self.assertEqual(len(h), 0)

    def test_insert_basic(self):
        h = fibheap.FibHeap()
        h.insert(10)
        self.assertEqual(len(h), 1)
        self.assertEqual(h.get_min(), 10)

        h.insert(5)
        self.assertEqual(len(h), 2)
        self.assertEqual(h.get_min(), 5)

        h.insert(20)
        self.assertEqual(len(h), 3)
        self.assertEqual(h.get_min(), 5)

    def test_get_min_empty(self):
        h = fibheap.FibHeap()
        self.assertIsNone(h.get_min())

    def test_get_min_multiple(self):
        h = fibheap.FibHeap()
        h.insert(42)
        self.assertEqual(h.get_min(), 42)
        h.insert(10)
        self.assertEqual(h.get_min(), 10)
        h.insert(100)
        self.assertEqual(h.get_min(), 10)
        self.assertEqual(len(h), 3)

    def test_extract_min_empty(self):
        h = fibheap.FibHeap()
        self.assertIsNone(h.extract_min())
        self.assertEqual(len(h), 0)

    def test_extract_min_single_element(self):
        h = fibheap.FibHeap()
        h.insert(10)
        self.assertEqual(len(h), 1)
        self.assertEqual(h.extract_min(), 10)
        self.assertEqual(len(h), 0)
        self.assertIsNone(h.get_min())
        self.assertIsNone(h.extract_min()) # Try extracting again

    def test_extract_min_multiple_elements(self):
        h = fibheap.FibHeap()
        values = [10, 5, 20, 3, 15]
        for v in values:
            h.insert(v)
        
        self.assertEqual(len(h), len(values))
        
        expected_sorted_values = sorted(values)
        
        for i in range(len(expected_sorted_values)):
            self.assertEqual(h.get_min(), expected_sorted_values[i])
            extracted = h.extract_min()
            self.assertEqual(extracted, expected_sorted_values[i])
            self.assertEqual(len(h), len(values) - 1 - i)
            
        self.assertIsNone(h.get_min())
        self.assertEqual(len(h), 0)

    def test_extract_min_causing_consolidation(self):
        # This test mirrors the C test's Scenario 4 for extract_min
        # Insert 8,7,6,5,4,3,2,1 (creates 8 root nodes initially in basic list)
        h = fibheap.FibHeap()
        vals = [8, 7, 6, 5, 4, 3, 2, 1]
        for v in vals:
            h.insert(v)
        
        self.assertEqual(len(h), 8)
        self.assertEqual(h.get_min(), 1)

        extracted_order = []
        while len(h) > 0:
            min_val = h.extract_min()
            extracted_order.append(min_val)
            if len(h) > 0: # Check get_min only if heap is not empty
                # The next minimum should be the smallest of the remaining vals
                # A simple way to check this is to compare with sorted list of original values
                self.assertEqual(h.get_min(), sorted(vals)[len(extracted_order)])


        self.assertEqual(extracted_order, sorted(vals))
        self.assertEqual(len(h), 0)
        self.assertIsNone(h.get_min())

    def test_delete_empty_heap(self):
        h = fibheap.FibHeap()
        # The wrapper is expected to raise RuntimeError if delete fails (e.g. empty or not found)
        # This matches the "Failed to delete from Fibonacci Heap" message in fibheap_wrapper.c
        with self.assertRaisesRegex(RuntimeError, "Failed to delete from Fibonacci Heap"):
            h.delete(10)

    def test_delete_non_existent(self):
        h = fibheap.FibHeap()
        h.insert(5)
        h.insert(15)
        with self.assertRaisesRegex(RuntimeError, "Failed to delete from Fibonacci Heap"):
            h.delete(10)
        self.assertEqual(len(h), 2) # Length should be unchanged

    def test_delete_only_node(self):
        h = fibheap.FibHeap()
        h.insert(10)
        self.assertEqual(len(h), 1)
        h.delete(10)
        self.assertEqual(len(h), 0)
        self.assertIsNone(h.get_min())

    def test_delete_min_node_multiple(self):
        h = fibheap.FibHeap()
        h.insert(10)
        h.insert(5) # min
        h.insert(20)
        self.assertEqual(len(h), 3)
        self.assertEqual(h.get_min(), 5)
        h.delete(5)
        self.assertEqual(len(h), 2)
        self.assertEqual(h.get_min(), 10)
        # Verify other elements are still there by extracting them
        extracted_elements = sorted([h.extract_min(), h.extract_min()])
        self.assertEqual(extracted_elements, [10, 20])


    def test_delete_non_min_root_node(self):
        h = fibheap.FibHeap()
        h.insert(5)  # min
        h.insert(10) 
        h.insert(20)
        self.assertEqual(len(h), 3)
        h.delete(10)
        self.assertEqual(len(h), 2)
        self.assertEqual(h.get_min(), 5)
        # Check remaining elements
        elements = []
        while len(h) > 0:
            elements.append(h.extract_min())
        elements.sort()
        self.assertEqual(elements, [5, 20])
        
    # Tests for update_key (decrease_key scenarios)
    # Note: The C tests for decrease_key are complex to replicate exactly without 
    #       direct node access. We test value-based updates.
    #       The `update_key` function in the wrapper calls `change_fib_node_value`.

    def test_update_key_decrease_root(self):
        h = fibheap.FibHeap()
        h.insert(10)
        h.insert(20)
        self.assertEqual(h.get_min(), 10)
        h.update_key(10, 5) # Decrease 10 to 5
        self.assertEqual(len(h), 2)
        self.assertEqual(h.get_min(), 5)
        # Ensure 20 is still there
        self.assertEqual(h.extract_min(), 5) # remove 5
        self.assertEqual(h.get_min(), 20)

    def test_update_key_to_larger_value(self):
        # Behavior might depend on C implementation of change_fib_node_value
        # The subtask report for C code suggested:
        # "If new value is larger, call delete_node_fib_heap then insert_fib_heap"
        h = fibheap.FibHeap()
        h.insert(10)
        h.insert(20)
        self.assertEqual(h.get_min(), 10)
        h.update_key(10, 15) # Increase 10 to 15
        self.assertEqual(len(h), 2)
        # After increasing 10 to 15, the elements are 15 and 20. Min should be 15.
        self.assertEqual(h.get_min(), 15)
        self.assertEqual(h.extract_min(), 15) # remove 15
        self.assertEqual(h.get_min(), 20)


    def test_update_key_non_existent(self):
        h = fibheap.FibHeap()
        h.insert(10)
        # Expecting error if old_value not found
        with self.assertRaisesRegex(RuntimeError, "Failed to update key in Fibonacci Heap"):
             h.update_key(5, 2)
        self.assertEqual(len(h), 1) # Length should be unchanged
        self.assertEqual(h.get_min(), 10)

    def test_update_key_child_causes_cut(self):
        # This is analogous to Scenario 4 in C test_decrease_key
        # Setup: Insert 10, 20, 5. Min is 5.
        # Extract Min (5). Heap consolidates. Assume 10 is min, 20 is child of 10 (or similar structure).
        # Then decrease 20 to 2. This should cut 20 (now 2) and make it the new min.
        h = fibheap.FibHeap()
        h.insert(10)
        h.insert(20)
        h.insert(5)
        self.assertEqual(h.get_min(), 5)
        self.assertEqual(h.extract_min(), 5) # Removes 5, triggers consolidation
        
        # After consolidation (exact structure depends on C impl, but 10 should be min, 20 related)
        self.assertEqual(h.get_min(), 10) 
        self.assertEqual(len(h), 2) # Contains 10, 20

        # Decrease key of 20 to 2. This should make 2 the new minimum.
        # If 20 was a child of 10, decreasing its key to 2 should cut it and move to root.
        h.update_key(20, 2)
        self.assertEqual(len(h), 2)
        self.assertEqual(h.get_min(), 2)
        
        # Check remaining elements
        self.assertEqual(h.extract_min(), 2)
        self.assertEqual(h.extract_min(), 10)
        self.assertEqual(len(h), 0)

    def test_len_empty(self):
        h = fibheap.FibHeap()
        self.assertEqual(len(h), 0)

    def test_len_after_inserts_and_extracts(self):
        h = fibheap.FibHeap()
        self.assertEqual(len(h), 0)
        h.insert(10)
        h.insert(20)
        self.assertEqual(len(h), 2)
        h.insert(5)
        self.assertEqual(len(h), 3)
        h.extract_min()
        self.assertEqual(len(h), 2)
        h.delete(20) # Assuming 20 is one of the remaining values
        self.assertEqual(len(h), 1)
        h.extract_min()
        self.assertEqual(len(h), 0)

if __name__ == '__main__':
    unittest.main()
