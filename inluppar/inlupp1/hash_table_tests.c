#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <CUnit/Basic.h>

#include "hash_table.h"

// TODO: Should we define a global constant to be the amount of buckets?

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

void assert_insert(ioopm_hash_table_t *ht, int key, char *value) {
  char *lookup_value;

  ioopm_hash_table_insert(ht, key, value);
  lookup_value = ioopm_hash_table_lookup(ht, key);

  CU_ASSERT_FALSE(HAS_ERROR());
  CU_ASSERT_EQUAL(lookup_value, value);
}

void assert_insert_and_remove(ioopm_hash_table_t *ht, int key, char *value) {
  assert_insert(ht, key, value);

  ioopm_hash_table_remove(ht, key);
  ioopm_hash_table_lookup(ht, key);

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

  // Make sure that each bucket is empty (except for the dummy entry)
  for (int i = 0; i < 17; ++i) {
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

  assert_insert(ht, 1, "test");

  ioopm_hash_table_destroy(ht);
}

void test_insert_replace() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int dummy_key = 1;

  assert_insert(ht, dummy_key, "test");
  assert_insert(ht, dummy_key, "hello");

  ioopm_hash_table_destroy(ht);
}

void test_insert_multiple() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  assert_insert(ht, 1, "test1");
  assert_insert(ht, 2, "test2");
  assert_insert(ht, 3, "test3");

  ioopm_hash_table_destroy(ht);
}

void test_insert_same_bucket() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int buckets_in_hash_table = 17;

  // Place three different entries into the same (first) bucket
  assert_insert(ht, buckets_in_hash_table * 0, "test1");
  assert_insert(ht, buckets_in_hash_table * 1, "test2");
  assert_insert(ht, buckets_in_hash_table * 2, "test3");

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

  assert_insert_and_remove(ht, 1, "test");

  ioopm_hash_table_destroy(ht);
}

// Previously, we had a bug where a insertion would fail when the key was 0
// This was fixed by using 'find_previous_entry_for_key' and the dummy entries
// This will make sure that it does not reappear later
void test_insert_key_0() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  assert_insert_and_remove(ht, 0, "test");

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

void test_hash_table_size_same_bucket() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int buckets_in_hash_table = 17;

  assert_hash_table_size(ht, 0);

  ioopm_hash_table_insert(ht, buckets_in_hash_table * 0, "test");
  assert_hash_table_size(ht, 1);

  ioopm_hash_table_insert(ht, buckets_in_hash_table * 1, "test");
  assert_hash_table_size(ht, 2);

  ioopm_hash_table_insert(ht, buckets_in_hash_table * 2, "test");
  assert_hash_table_size(ht, 3);

  ioopm_hash_table_insert(ht, buckets_in_hash_table * 3, "test");
  assert_hash_table_size(ht, 4);

  ioopm_hash_table_insert(ht, buckets_in_hash_table * 4, "test");
  assert_hash_table_size(ht, 5);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_is_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int dummy_key = 1;

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_insert(ht, dummy_key, "test");

  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_remove(ht, dummy_key);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_clear() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, 1, "test");
  ioopm_hash_table_insert(ht, 2, "test");
  ioopm_hash_table_insert(ht, 3, "test");

  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_clear(ht);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_clear_same_bucket() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int buckets_in_hash_table = 17;

  ioopm_hash_table_insert(ht, buckets_in_hash_table * 0, "test");
  ioopm_hash_table_insert(ht, buckets_in_hash_table * 1, "test");
  ioopm_hash_table_insert(ht, buckets_in_hash_table * 2, "test");
  ioopm_hash_table_insert(ht, buckets_in_hash_table * 3, "test");

  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_clear(ht);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_clear_on_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_clear(ht);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();



  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys_empty() {

}

void test_hash_table_keys_same_bucket() {

}

void test_hash_table_keys_modified() {

}

void test_hash_table_values() {

}

void test_hash_table_values_empty() {

}

void test_hash_table_values_same_bucket() {

}

void test_hash_table_values_modified() {

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
    (NULL == CU_add_test(test_suite1, "it inserts a new entry", test_insert)) ||
    (NULL == CU_add_test(test_suite1, "it inserts multiple entries", test_insert_multiple)) ||
    (NULL == CU_add_test(test_suite1, "it inserts multiple entires into the same bucket", test_insert_same_bucket)) ||
    (NULL == CU_add_test(test_suite1, "it replaces an entry when inserting with existing key", test_insert_replace)) ||
    (NULL == CU_add_test(test_suite1, "it does nothing when trying to remove a non-existent key", test_remove_non_existant_key)) ||
    (NULL == CU_add_test(test_suite1, "it removes an existing entry", test_remove_deletes_entry)) ||
    (NULL == CU_add_test(test_suite1, "it inserts and removes an entry when the key is 0", test_insert_key_0)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the size to 0 on a newly allocated hash table", test_hash_table_size_empty)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after inserting and removing", test_hash_table_size_not_empty)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after inserting entries into the same bucket", test_hash_table_size_same_bucket)) ||
    (NULL == CU_add_test(test_suite1, "it detects if the hash table is empty or not", test_hash_table_is_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears all entries in a non-empty hash table", test_hash_table_clear)) ||
    (NULL == CU_add_test(test_suite1, "it does not fail when trying to clear an empty hash table", test_hash_table_clear_on_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears a hash table where all entries are in the same bucket", test_hash_table_clear_same_bucket))
   ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
