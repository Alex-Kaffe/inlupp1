#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "hash_table.h"

int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

void test_create_destroy() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  CU_ASSERT_PTR_NOT_NULL(ht);
  ioopm_hash_table_destroy(ht);
}

// Test that there are no valid keys after creating an empty hash table
void test_lookup() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  // The constant 18 checks to see if the key resolves correctly to the second bucket,
  // since the bucket size is fixed to 17 and the index is calculated using: 18 % 17 = 1
  for (int i = 0; i < 18; ++i) {
    CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, i));
  }

  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, -1));
  ioopm_hash_table_destroy(ht);
}

// Test if an invalid key gives an error
void test_lookup1() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *value = ioopm_hash_table_lookup(ht, 1);
  CU_ASSERT(errno == EINVAL);
  CU_ASSERT_PTR_NULL(value);

  ioopm_hash_table_destroy(ht);
}

// TODO: Add test for checking that a key exist after being inserted
// TODO: Add test for checking that a key does not exist after being removed

int main() {
  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Hash table", init_suite, clean_suite);
  if (NULL == test_suite1) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
    (NULL == CU_add_test(test_suite1, "it creates and returns a pointer to an allocated hash table", test_create_destroy)) ||
    (NULL == CU_add_test(test_suite1, "it returns NULL for keys that does not exist", test_lookup)) ||
    (NULL == CU_add_test(test_suite1, "it returns an error for keys that does not exist", test_lookup1))
   ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
