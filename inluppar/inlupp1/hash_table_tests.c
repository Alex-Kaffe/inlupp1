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

void assert_hash_table_size(ioopm_hash_table_t *ht, int expected_size) {
  int size = ioopm_hash_table_size(ht);

  CU_ASSERT_EQUAL(size, expected_size);
}

void assert_insert_and_remove_entry(ioopm_hash_table_t *ht, int key, char *value) {
  char *lookup_value;

  ioopm_hash_table_insert(ht, key, value);

  lookup_value = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_FALSE(HAS_ERROR());
  CU_ASSERT_EQUAL(value, lookup_value);

  ioopm_hash_table_remove(ht, key);
  lookup_value = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_TRUE(HAS_ERROR());
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
void test_lookup_gives_error() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  // Lookup a key that does not exist (since the hash table is empty)
  char *value = ioopm_hash_table_lookup(ht, 1);
  CU_ASSERT_TRUE(HAS_ERROR());
  CU_ASSERT_PTR_NULL(value);

  ioopm_hash_table_destroy(ht);
}

void test_insert() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int dummy_key = 1;
  char *dummy_value = "test";

  // Insert a dummy entry with key 1
  ioopm_hash_table_insert(ht, dummy_key, dummy_value);
  char *value = ioopm_hash_table_lookup(ht, dummy_key);

  // 'errno' gets reset to 0 when calling 'ioopm_hash_table_lookup', so assuming
  // that there were no errors, errno will be 0.
  CU_ASSERT_FALSE(HAS_ERROR());
  CU_ASSERT_EQUAL(value, dummy_value);

  ioopm_hash_table_destroy(ht);
}

void test_insert_replace() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *value;
  int dummy_key = 1;
  char *initial_value = "test";
  char *replaced_value = "hello";

  ioopm_hash_table_insert(ht, dummy_key, initial_value);

  value = ioopm_hash_table_lookup(ht, dummy_key);
  CU_ASSERT_FALSE(HAS_ERROR());
  CU_ASSERT_EQUAL(value, initial_value);

  // Replace the value of the entry with 'dummy_key' as key
  ioopm_hash_table_insert(ht, dummy_key, replaced_value);

  value = ioopm_hash_table_lookup(ht, dummy_key);
  CU_ASSERT_FALSE(HAS_ERROR());
  CU_ASSERT_EQUAL(value, replaced_value);

  ioopm_hash_table_destroy(ht);
}

void test_remove_non_existant_key() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *value;

  // 17 in this case refers to the fixed amount of buckets in our hash table
  int buckets_in_hash_table = 17;
  char *dummy_value = "test";

  for (int i = 0; i < buckets_in_hash_table; i++) {
    // Populate the hash table with 17 different entries, one for each bucket
    ioopm_hash_table_insert(ht, i, dummy_value);
  }

  // Try to remove an invalid key
  ioopm_hash_table_remove(ht, -1);

  // Make sure that all the dummy entries are still present in the hash table
  // and that the value is the same as before
  for (int i = 0; i < buckets_in_hash_table; i++) {
    value = ioopm_hash_table_lookup(ht, i);
    CU_ASSERT_FALSE(HAS_ERROR());
    CU_ASSERT_EQUAL(value, dummy_value);
  }

  ioopm_hash_table_destroy(ht);
}

void test_remove_deletes_entry() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  assert_insert_and_remove_entry(ht, 1, "test");

  ioopm_hash_table_destroy(ht);
}

// Previously, we had a bug where a insertion would fail when the key was 0
// This was fixed by using 'find_previous_entry_for_key' and the dummy entries
// This will make sure that it does not reappear later
void test_insert_key_0() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  assert_insert_and_remove_entry(ht, 0, "test");

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_size_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  // A newly allocated hash table is empty (though it does have dummy entries)
  assert_hash_table_size(ht, 0);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_size_not_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int dummy_key = 1;

  assert_hash_table_size(ht, 0);

  ioopm_hash_table_insert(ht, dummy_key, "test");

  assert_hash_table_size(ht, 1);

  ioopm_hash_table_remove(ht, dummy_key);

  assert_hash_table_size(ht, 0);

  ioopm_hash_table_destroy(ht);
}

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
    (NULL == CU_add_test(test_suite1, "it returns an error for keys that does not exist", test_lookup_gives_error)) ||
    (NULL == CU_add_test(test_suite1, "it inserts a new entry at specified key", test_insert)) ||
    (NULL == CU_add_test(test_suite1, "it replaces an entry when inserting with existing key", test_insert_replace)) ||
    (NULL == CU_add_test(test_suite1, "it does nothing when trying to remove a non existant key", test_remove_non_existant_key)) ||
    (NULL == CU_add_test(test_suite1, "it removes an existing entry", test_remove_deletes_entry)) ||
    (NULL == CU_add_test(test_suite1, "it inserts and removes an entry when the key is 0", test_insert_key_0))
   ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
