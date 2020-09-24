#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>

#include "linked_list.h"

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

void assert_link_index(ioopm_list_t *list, int value, int index) {
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
  CU_ASSERT_FALSE(HAS_ERROR());

  ioopm_linked_list_append(list, 1337);
  CU_ASSERT_EQUAL_FATAL(ioopm_linked_list_get(list, 1), 1337);
  CU_ASSERT_FALSE(HAS_ERROR());

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
  assert_link_index(list, values[last_index], last_index);
  assert_link_index(list, values[0], 0);

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
  assert_link_index(list, values[0], last_index);
  assert_link_index(list, values[last_index], 0);

  ioopm_linked_list_destroy(list);
}

void test_insert() {
  ioopm_list_t *list = ioopm_linked_list_create();

  int values_length = 3;
  int values[] = { 99, 1337, 420 };

  for (int i = 0; i < values_length; i++) {
    ioopm_linked_list_insert(list, i, values[i]);

    CU_ASSERT_FALSE(HAS_ERROR());

    // Make sure that the value gets added to the correct index
    assert_link_index(list, values[i], i);
  }

  // Insert elements into the middle of the list
  // and make sure that the values get added correctly
  ioopm_linked_list_insert(list, 2, 300);
  assert_link_added(list, 300, 4);
  assert_link_index(list, 300, 2);

  ioopm_linked_list_insert(list, 2, 500);
  assert_link_added(list, 500, 5);
  assert_link_index(list, 500, 2);

  ioopm_linked_list_destroy(list);
}

void test_insert_out_of_bounds() {
  ioopm_list_t *list = ioopm_linked_list_create();

  // Make sure that when inserting a value at an index that is larger than
  // the next index (e.g inserting at index 2 when we only have 1 element),
  // the new value gets added to the correct index
  ioopm_linked_list_insert(list, 999, 100);
  CU_ASSERT_TRUE(HAS_ERROR());

  ioopm_linked_list_destroy(list);
}

void test_insert_first() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_insert(list, 0, 100);
  CU_ASSERT_FALSE(HAS_ERROR());
  assert_link_added(list, 100, 1);
  assert_link_index(list, 100, 0);

  ioopm_linked_list_destroy(list);
}

void test_insert_last() {
  ioopm_list_t *list = ioopm_linked_list_create();
  
  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  ioopm_linked_list_insert(list, 3, 400);
  CU_ASSERT_FALSE(HAS_ERROR());
  assert_link_added(list, 400, 4);
  assert_link_index(list, 400, 3);

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
  CU_ASSERT_FALSE(HAS_ERROR());
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

  CU_ASSERT_FALSE(ioopm_iterator_has_next(iterator));

  ioopm_linked_list_append(list, 200);
  CU_ASSERT_TRUE(ioopm_iterator_has_next(iterator));

  ioopm_iterator_destroy(iterator);

  ioopm_linked_list_destroy(list);
}


void test_iterator_next() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 42);
  ioopm_linked_list_append(list, 240);
  ioopm_linked_list_append(list, 420);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  CU_ASSERT_EQUAL(ioopm_iterator_next(iterator), 240);
  CU_ASSERT_FALSE(HAS_ERROR());
  
  CU_ASSERT_EQUAL(ioopm_iterator_next(iterator), 420);
  CU_ASSERT_FALSE(HAS_ERROR());

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_next_invalid() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 42);
  ioopm_linked_list_append(list, 240);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  // Go to the last element
  ioopm_iterator_next(iterator);
  CU_ASSERT_FALSE(HAS_ERROR());

  // Make sure that calling next when we are already on the last element
  // gives us an error
  ioopm_iterator_next(iterator);
  CU_ASSERT_TRUE(HAS_ERROR());

  // Make sure that the current link was not updated and still points
  // to the last element in the list
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 240);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 24);
  ioopm_linked_list_append(list, 240);
  ioopm_linked_list_append(list, 2400);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  // Remove the second element
  ioopm_iterator_next(iterator);
  ioopm_iterator_remove(iterator);
  CU_ASSERT_FALSE(HAS_ERROR());

  // Assert that the list has been shifted once to the left
  // and that the list has been updated
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 2400);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1), 2400);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  //Attempt to remove the dummy.
  ioopm_iterator_remove(iterator);
  CU_ASSERT(HAS_ERROR());

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove_first() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 20);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 2000);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  int first_value = ioopm_iterator_current(iterator);

  //Attempt to remove the first value.
  int removed_value = ioopm_iterator_remove(iterator);

  CU_ASSERT_EQUAL(first_value, removed_value);
  CU_ASSERT_NOT_EQUAL(ioopm_iterator_current(iterator), removed_value);

  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0), 200);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_remove_last() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 20);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 2000);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  // Go to the last element
  while (ioopm_iterator_has_next(iterator)) {
    ioopm_iterator_next(iterator);
  }

  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 2000);

  ioopm_iterator_remove(iterator);

  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 200);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1), 200);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  ioopm_iterator_insert(iterator, 100);
  assert_link_added(list, 100, 1);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  // Go to the second element
  ioopm_iterator_next(iterator);

  ioopm_iterator_insert(iterator, 400);
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 400);
  assert_link_added(list, 400, 4);
  assert_link_index(list, 400, 1);

  // Make sure that 200 has been shifted to the right
  assert_link_index(list, 200, 2);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert_first() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  // Insert into index 0
  ioopm_iterator_insert(iterator, 400);
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 400);
  assert_link_added(list, 400, 4);
  assert_link_index(list, 400, 0);

  // Make sure that 100 has been shifted to the right
  assert_link_index(list, 100, 1);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_insert_last() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  // Go to the last element (index == 3)
  while(ioopm_iterator_has_next(iterator)) {
    ioopm_iterator_next(iterator);
  }

  ioopm_iterator_insert(iterator, 400);
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 400);
  assert_link_added(list, 400, 4);
  assert_link_index(list, 400, 2);

  // Make sure that 300 is now the last element
  assert_link_index(list, 300, 3);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_reset() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  // Go the last element
  while(ioopm_iterator_has_next(iterator)) {
    ioopm_iterator_next(iterator);
  }

  //Assert that the current value is the same as the value of the last element
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 300);

  // Reset the iterator
  ioopm_iterator_reset(iterator);

  // Assert that sure that the iterator was reset to the dummy entry
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 100);

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

void test_iterator_current() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  ioopm_linked_list_append(list, 200);
  ioopm_linked_list_append(list, 300);

  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 100);
  CU_ASSERT_FALSE(HAS_ERROR());

  CU_ASSERT_EQUAL(ioopm_iterator_next(iterator), 200);
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 200);
  CU_ASSERT_FALSE(HAS_ERROR());

  CU_ASSERT_EQUAL(ioopm_iterator_next(iterator), 300);
  CU_ASSERT_EQUAL(ioopm_iterator_current(iterator), 300);
  CU_ASSERT_FALSE(HAS_ERROR());

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(list);
}

  void test_iterator_current_invalid() {
  ioopm_list_t *list = ioopm_linked_list_create();
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list);

  ioopm_iterator_current(iterator);
  CU_ASSERT_TRUE(HAS_ERROR());

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
    (NULL == CU_add_test(test_suite1, "it gives an error when inserting at an invalid index", test_insert_out_of_bounds)) ||
    (NULL == CU_add_test(test_suite1, "it prepends links when specifying an index equal to 0", test_insert_first)) ||
    (NULL == CU_add_test(test_suite1, "it appends links when specifying an index equal to the current size", test_insert_last)) ||
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
    (NULL == CU_add_test(test_suite2, "it removes an element from the linked list and updates the iterator", test_iterator_remove)) ||
    (NULL == CU_add_test(test_suite2, "it removes an element from the empty linked list", test_iterator_remove_empty)) ||
    (NULL == CU_add_test(test_suite2, "it removes the first element from the linked list", test_iterator_remove_first)) ||
    (NULL == CU_add_test(test_suite2, "it removes the last element from the linked list", test_iterator_remove_last)) ||
    (NULL == CU_add_test(test_suite2, "it inserts links into the empty linked list and updates the iterator", test_iterator_insert_empty)) ||
    (NULL == CU_add_test(test_suite2, "it inserts links into the linked list and updates the iterator", test_iterator_insert)) ||
    (NULL == CU_add_test(test_suite2, "it inserts links first into the linked list and updates the iterator", test_iterator_insert_first)) ||
    (NULL == CU_add_test(test_suite2, "it inserts links last into the linked list and updates the iterator", test_iterator_insert_last)) ||
    (NULL == CU_add_test(test_suite2, "it resets the iterator to the start of the linked list", test_iterator_reset)) ||
    (NULL == CU_add_test(test_suite2, "it returns the value of the current iterator element", test_iterator_current)) ||
    (NULL == CU_add_test(test_suite2, "it gives an error when trying to get the current element of an empty linked list", test_iterator_current_invalid))
  ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
