#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>

#include "list_linked.h"

int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

// TODO: Assert the value of HAS_ERROR() after running ioopm_linked_list_remove

void assert_link_added(ioopm_list_t *list, int value, int expected_size) {
  CU_ASSERT_TRUE(ioopm_linked_list_contains(list, value));
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), expected_size);
}

void assert_link_index(ioopm_list_t *list, int index, int value) {
  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, index), value);
}

bool value_equiv(int value, void *extra) {
  return value == *((int*)extra);
}

void update_value(int *value, void *extra) {
  *value = *((int*)extra);
}

void test_create_destroy() {
  ioopm_list_t *list = ioopm_linked_list_create();

  CU_ASSERT_PTR_NOT_NULL(list);

  // Make sure that the size is 0
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);

  ioopm_linked_list_destroy(list);
}

void test_get() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 420);
  CU_ASSERT_EQUAL_FATAL(ioopm_linked_list_get(list, 0), 420);

  ioopm_linked_list_append(list, 1337);
  CU_ASSERT_EQUAL_FATAL(ioopm_linked_list_get(list, 1), 1337);

  ioopm_linked_list_destroy(list);
}

void test_get_invalid() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_get(list, 0);
  CU_ASSERT_TRUE_FATAL(HAS_ERROR());

  ioopm_linked_list_get(list, -1);
  CU_ASSERT_TRUE_FATAL(HAS_ERROR());

  ioopm_linked_list_get(list, 100);
  CU_ASSERT_TRUE_FATAL(HAS_ERROR());

  ioopm_linked_list_destroy(list);
}

void test_size_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();

  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);

  ioopm_linked_list_destroy(list);
}

void test_size_insert() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);

  ioopm_linked_list_prepend(list, 200);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

  ioopm_linked_list_insert(list, 2, 300); // insert into index 2
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);

  ioopm_linked_list_destroy(list);
}

void test_size_remove() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  ioopm_linked_list_remove(list, 2);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

  ioopm_linked_list_remove(list, 1);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);

  ioopm_linked_list_remove(list, 0);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);

  ioopm_linked_list_destroy(list);
}

void test_append() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int values_length = 3;
  int last_index = values_length - 1;
  int values[] = { 100, 200, 300 };

  for (int i = 0; i < values_length; i++) {
    ioopm_linked_list_append(list, values[i]);
    assert_link_added(list, values[i], i+1);
  }

  // Make sure that the first and last link of the linked list
  // contains the value that was added first and last respectively
  assert_link_index(list, last_index, values[last_index]);
  assert_link_index(list, 0, values[0]);

  ioopm_linked_list_destroy(list);
}

void test_prepend() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int values_length = 3;
  int last_index = values_length - 1;
  int values[] = { 99, 1337, 420 };

  for (int i = 0; i < values_length; i++) {
    ioopm_linked_list_prepend(list, values[i]);
    assert_link_added(list, values[i], i+1);
  }

  // Make sure that the first and last link of the linked list
  // contains the value that was added last and first respectively
  assert_link_index(list, last_index, values[0]);
  assert_link_index(list, 0, values[last_index]);

  ioopm_linked_list_destroy(list);
}

void test_insert() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int values_length = 3;
  int values[] = { 99, 1337, 420 };

  for (int i = 0; i < values_length; i++) {
    ioopm_linked_list_insert(list, i, values[i]);

    // Make sure that the value gets added to the correct index
    assert_link_index(list, i, values[i]);
  }

  ioopm_linked_list_destroy(list);
}

void test_insert_too_large() {
  ioopm_list_t *list = ioopm_linked_list_create();

  // Make sure that when inserting a value at an index that is larger than
  // the next index (e.g inserting at index 2 when we only have 1 element),
  // the new value gets added to the correct index
  ioopm_linked_list_insert(list, 999, 100);
  assert_link_index(list, 0, 100);

  ioopm_linked_list_insert(list, 999, 200);
  assert_link_index(list, 1, 200);

  ioopm_linked_list_destroy(list);
}

void test_insert_too_small() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_insert(list, -100, 100);
  assert_link_index(list, 0, 100);

  ioopm_linked_list_insert(list, -100, 200);
  assert_link_index(list, 0, 200);

  ioopm_linked_list_destroy(list);
}

void test_remove() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int removed_value;

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  // Remove the value 200
  removed_value = ioopm_linked_list_remove(list, 1);
  CU_ASSERT_EQUAL(removed_value, 200);
  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, 200));

  // Make sure that the last value has shifted to index 1 instead of 2
  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1), 300);

  // Try removing the invalid index and check that we get an error
  ioopm_linked_list_remove(list, 2);
  CU_ASSERT_TRUE(HAS_ERROR());

  ioopm_linked_list_destroy(list);
}

void test_remove_edges() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int removed_value;

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  removed_value = ioopm_linked_list_remove(list, 0);
  CU_ASSERT_EQUAL(removed_value, 100);
  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, 100));

  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0), 200);

  // Remove the last element
  removed_value = ioopm_linked_list_remove(list, 1);
  CU_ASSERT_EQUAL(removed_value, 300);
  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, 300));

  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0), 200);

  // Make sure that the last value has been removed
  ioopm_linked_list_get(list, 1);
  CU_ASSERT_TRUE(HAS_ERROR());

  ioopm_linked_list_destroy(list);
}

void test_remove_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();

  // Try removing non existant indices, as well as invalid indices (e.g -1)
  ioopm_linked_list_remove(list, 0);
  CU_ASSERT_TRUE(HAS_ERROR());

  ioopm_linked_list_remove(list, -1);
  CU_ASSERT_TRUE(HAS_ERROR());

  ioopm_linked_list_remove(list, 20);
  CU_ASSERT_TRUE(HAS_ERROR());

  ioopm_linked_list_destroy(list);
}

void test_clear() {
  ioopm_list_t *list = ioopm_linked_list_create();

  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);

  ioopm_linked_list_clear(list);

  // Make sure that all elements has been deleted and that
  // the size has updated correctly
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);
  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, 100));
  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, 200));
  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, 300));

  ioopm_linked_list_destroy(list);
}

void test_all() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int initial_value = 420;

  ioopm_linked_list_append(list, initial_value);
  ioopm_linked_list_append(list, initial_value);
  ioopm_linked_list_append(list, initial_value);
  CU_ASSERT_TRUE(ioopm_linked_list_all(list, value_equiv, &initial_value));

  ioopm_linked_list_append(list, 1337);

  CU_ASSERT_FALSE(ioopm_linked_list_all(list, value_equiv, &initial_value));

  ioopm_linked_list_destroy(list);
}

void test_all_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int value = 200;

  CU_ASSERT_TRUE(ioopm_linked_list_all(list, value_equiv, &value));

  ioopm_linked_list_destroy(list);
}

void test_apply_all() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int initial_value = 420;
  int new_value = 1337;

  ioopm_linked_list_append(list, initial_value);
  ioopm_linked_list_append(list, initial_value);
  ioopm_linked_list_append(list, initial_value);
  CU_ASSERT_TRUE(ioopm_linked_list_all(list, value_equiv, &initial_value));

  ioopm_linked_apply_to_all(list, update_value, &new_value);

  CU_ASSERT_FALSE(ioopm_linked_list_all(list, value_equiv, &initial_value));
  CU_ASSERT_TRUE(ioopm_linked_list_all(list, value_equiv, &new_value));

  ioopm_linked_list_destroy(list);
}

void test_apply_all_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int new_value = 1337;

  ioopm_linked_apply_to_all(list, update_value, &new_value);

  CU_ASSERT_TRUE(ioopm_linked_list_all(list, value_equiv, &new_value));

  ioopm_linked_list_destroy(list);
}

void test_clear_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_clear(list);

  ioopm_linked_list_destroy(list);
}

//////    TEST: ITERATOR

void test_iterator_create_destroy() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  CU_ASSERT_PTR_NOT_NULL(iterator);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_has_next() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 52);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  CU_ASSERT_TRUE(ioopm_iterator_has_next(iterator));

  ioopm_iterator_destroy(iterator);

  ioopm_linked_list_destroy(list);
}

void test_iterator_next() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_next_invalid() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove_update() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove_update_first() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove_update_last() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert_update() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert_update_first() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert_update_last() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_reset() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_current() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_current_invalid() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);



  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

int main() {
  CU_pSuite test_suite1 = NULL;
  CU_pSuite test_suite2 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Linked list", init_suite, clean_suite);
  if (NULL == test_suite1) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  test_suite2 = CU_add_suite("Iterator", init_suite, clean_suite);
  if (NULL == test_suite2) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
    (NULL == CU_add_test(test_suite1, "it creates and returns a pointer to an allocated linked list", test_create_destroy)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the size to 0 on an empty linked list", test_size_empty)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after inserting", test_size_insert)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after removing", test_size_remove)) ||
    (NULL == CU_add_test(test_suite1, "it returns the value of a link given a valid index", test_get)) ||
    (NULL == CU_add_test(test_suite1, "it returns an error when getting a link with an invalid index", test_get_invalid)) ||
    (NULL == CU_add_test(test_suite1, "it appends links into the linked list", test_append)) ||
    (NULL == CU_add_test(test_suite1, "it prepends links into the linked list", test_prepend)) ||
    (NULL == CU_add_test(test_suite1, "it inserts links into the linked list at the specified index", test_insert)) ||
    (NULL == CU_add_test(test_suite1, "it appends links when specifying an index greater or equal to the size", test_insert_too_large)) ||
    (NULL == CU_add_test(test_suite1, "it prepends links when specifying an index smaller than 0", test_insert_too_small)) ||
    (NULL == CU_add_test(test_suite1, "it removes links from the linked list", test_remove)) ||
    (NULL == CU_add_test(test_suite1, "it removes the first and last links from the linked list", test_remove_edges)) ||
    (NULL == CU_add_test(test_suite1, "it gives an error when removing invalid indices", test_remove_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears an empty linked list", test_clear_empty)) ||
    (NULL == CU_add_test(test_suite1, "it returns true if all values matches the predicate", test_all)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when applying a predicate to an empty linked list", test_all_empty)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when applying a predicate to an empty linked list", test_all_empty)) ||
    (NULL == CU_add_test(test_suite1, "it applies a function to all elements and updates the values", test_apply_all)) ||
    (NULL == CU_add_test(test_suite1, "it applies a function to an empty linked list", test_apply_all_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears a non empty linked list", test_clear))
  ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
    (NULL == CU_add_test(test_suite2, "it creates and returns a pointer to an allocated iterator", test_iterator_create_destroy)) ||
    (NULL == CU_add_test(test_suite2, "it returns true or false based on if the iterator has a next", test_iterator_has_next)) ||
    (NULL == CU_add_test(test_suite2, "it returns the value of the next element in the linked list", test_iterator_next)) ||
    (NULL == CU_add_test(test_suite2, "it gives an error when trying to get the next element when there are none", test_iterator_next_invalid)) ||
    (NULL == CU_add_test(test_suite2, "it removes an element from the linked list", test_iterator_remove)) ||
    (NULL == CU_add_test(test_suite2, "it updates the current element in the iterator after removing", test_iterator_remove_update)) ||
    (NULL == CU_add_test(test_suite2, "it updates the current element in the iterator after removing the first element", test_iterator_remove_update_first)) ||
    (NULL == CU_add_test(test_suite2, "it updates the current element in the iterator after removing the last element", test_iterator_remove_update_last)) ||
    (NULL == CU_add_test(test_suite2, "it inserts links into the linked list", test_iterator_insert)) ||
    (NULL == CU_add_test(test_suite2, "it updates the current element in the iterator after inserting", test_iterator_insert_update)) ||
    (NULL == CU_add_test(test_suite2, "it updates the current element in the iterator after prepending", test_iterator_insert_update_first)) ||
    (NULL == CU_add_test(test_suite2, "it updates the current element in the iterator after appending", test_iterator_insert_update_last)) ||
    (NULL == CU_add_test(test_suite2, "it resets the iterator to the start of the linked list", test_iterator_reset)) ||
    (NULL == CU_add_test(test_suite2, "it returns the value of the current iterator element", test_iterator_current)) ||
    (NULL == CU_add_test(test_suite2, "it gives an error when getting the current value before calling iterator_next", test_iterator_current_invalid))
  ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
