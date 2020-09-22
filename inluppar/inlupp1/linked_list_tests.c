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

void test_size_empty() {
  ioopm_list_t *list = ioopm_linked_list_create();

  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);

  ioopm_linked_list_destroy(list);
}

void test_size() {
  ioopm_list_t *list = ioopm_linked_list_create();

  ioopm_linked_list_append(list, 100);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);

  ioopm_linked_list_prepend(list, 200);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 2);

  ioopm_linked_list_insert(list, 2, 300); // insert into index 2
  CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 3);

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
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after inserting and removing", test_size)) ||
    (NULL == CU_add_test(test_suite1, "it appends links into the linked list", test_append)) ||
    (NULL == CU_add_test(test_suite1, "it prepends links into the linked list", test_prepend))
   ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
