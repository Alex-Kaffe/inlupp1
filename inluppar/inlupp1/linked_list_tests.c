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

void assert_link_added(ioopm_list_t *list, int value, int expected_size) {
  CU_ASSERT_TRUE(ioopm_linked_list_contains(list, value));
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), expected_size);
}

void assert_link_index(ioopm_list_t *list, int index, int value) {
  CU_ASSERT_EQUAL(ioopm_linked_list_get(list, index), value);
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

void test_clear_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();
  
  ioopm_linked_list_clear(list);
  
  ioopm_linked_list_destroy(list);
}

int main() {
  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Linked list", init_suite, clean_suite);
  if (NULL == test_suite1) {
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
    (NULL == CU_add_test(test_suite1, "it gives an error when removing invalid indices", test_remove_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears an empty linked list", test_clear_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears an non empty linked list", test_clear)) 
   ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
