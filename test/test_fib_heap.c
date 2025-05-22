#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "../fibonacci_heap.h"

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
    suite_add_tcase(s, tc_core);

    // Test case for get_min
    tc_get_min_case = tcase_create("GetMin"); // Use the renamed TCase variable
    tcase_add_test(tc_get_min_case, test_get_min); // test_get_min is the START_TEST block
    suite_add_tcase(s, tc_get_min_case);

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
