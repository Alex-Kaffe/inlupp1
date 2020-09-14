#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "hash_table.h"

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create();
   CU_ASSERT_PTR_NOT_NULL(ht);
   ioopm_hash_table_destroy(ht);
}

void test_lookup() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  // The constant 18 is used to make sure that if we add more than 17 elements (the size of our buckets)
  // the 18th element is put into a new bucket
  for (int i = 0; i < 18; ++i) {
    CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, i));
  }
  
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, -1));
  ioopm_hash_table_destroy(ht);
}

// Test if an invalid key gives an error
void test_lookup1() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  
  CU_ASSERT(errno != EINVAL);
  char *value = ioopm_hash_table_lookup(ht, 1);
  CU_ASSERT(errno == EINVAL);
  CU_ASSERT_PTR_NULL(value);
  
  ioopm_hash_table_destroy(ht);
}

int main()
{
  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Hash table", init_suite, clean_suite);
  if (NULL == test_suite1)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
    (NULL == CU_add_test(test_suite1, "it creates and destroys hash tables", test_create_destroy)) ||
    (NULL == CU_add_test(test_suite1, "it finds elements by key", test_lookup)) ||
    (NULL == CU_add_test(test_suite1, "it adds a new key", test_lookup1))
  )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }


    CU_basic_set_mode(CU_BRM_VERBOSE); //Detaljerna utav testerna skrivs ut.
    CU_basic_run_tests();               //Kör alla testen.
    CU_cleanup_registry();              //Städar upp testerna (avallokerar minnen bland annat)
    return CU_get_error();              //Returnerar alla fel som hänt
}