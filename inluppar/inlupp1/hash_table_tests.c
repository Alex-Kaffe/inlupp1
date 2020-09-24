#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>

#include "common.h"
#include "hash_table.h"

// TODO: Should we define a global constant to be the amount of buckets?

int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

void assert_hash_table_size(ioopm_hash_table_t *ht, int expected_size) {
  size_t size = ioopm_hash_table_size(ht);

  CU_ASSERT_EQUAL(size, expected_size);
}

void assert_lookup(ioopm_hash_table_t *ht, int key, char *value, bool should_have_error) {
  char *lookup_value = ioopm_hash_table_lookup(ht, key);

  CU_ASSERT_EQUAL(HAS_ERROR(), should_have_error);

  if (should_have_error || value == NULL) {
    CU_ASSERT_EQUAL(lookup_value, value);
  } else {
    // Only assert string equality when we expect no errors
    // since CU_ASSERT_STRING_EQUAL will cause a segfault when
    // a value is NULL
    CU_ASSERT_STRING_EQUAL(lookup_value, value);
  }
}

// Inserts an entry into the hash table and makes sure that it is inserted correctly using lookup
void assert_insert(ioopm_hash_table_t *ht, int key, char *value) {
  ioopm_hash_table_insert(ht, key, value);
  assert_lookup(ht, key, value, false);
}

// Removes an entry from the hash table and makes sure that it no longer exists
// using lookup
void assert_remove(ioopm_hash_table_t *ht, int key) {
  ioopm_hash_table_remove(ht, key);
  assert_lookup(ht, key, NULL, true);
}

void assert_insert_and_remove(ioopm_hash_table_t *ht, int key, char *value) {
  assert_insert(ht, key, value);
  assert_remove(ht, key);
}

void assert_values_array_terminates(char **values, int values_size) {
  CU_ASSERT_PTR_NULL(values[values_size]);
}

void assert_keys_array(ioopm_hash_table_t *ht, int *expected_keys, int expected_size) {
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);

  CU_ASSERT_EQUAL(ioopm_linked_list_size(keys), expected_size);
  
  for (int i = 0; i < expected_size; i++) {
    // Make sure that all keys are present in the keys array
    // and that they are in the expected order.
    CU_ASSERT_EQUAL(ioopm_linked_list_get(keys, i), expected_keys[i]);
  }

  // Make sure to clean up the allocated keys array
  ioopm_linked_list_destroy(keys);
}

void assert_values_array(ioopm_hash_table_t *ht, char *expected_values[], int expected_size) {
  char **values = ioopm_hash_table_values(ht);

  assert_values_array_terminates(values, expected_size);

  for (int i = 0; i < expected_size; i++) {
    // Make sure that all values are present in the values array
    // and that they are in the expected order.
    if (values[i] == NULL) {
      // Prevent segfault if a value is NULL and simply assert the pointer instead.
      // Doing it like this makes it more clear as to why the test does not work,
      // rather than just crashing
      CU_ASSERT_PTR_NOT_NULL(values[i]);
    } else {
      CU_ASSERT_STRING_EQUAL(values[i], expected_values[i]);
    }
  }

  // Make sure to clean up the allocated values array
  free(values);
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
  // Also make sure that accessing a bucket with a key larger than the amount of buckets
  // resolves to NULL
  for (int i = 0; i < 18; ++i) {
    assert_lookup(ht, i, NULL, true);
  }

  assert_lookup(ht, -1, NULL, true);

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

void test_remove_invalid_key() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int buckets_in_hash_table = 17;
  char *test_value = "test";

  for (int i = 0; i < buckets_in_hash_table; i++) {
    // Populate the hash table with 17 different entries, one for each bucket
    ioopm_hash_table_insert(ht, i, test_value);
  }

  // Try to remove an invalid key
  ioopm_hash_table_remove(ht, -1);

  // Make sure that all the dummy entries are still present in the hash table
  // and that the value is the same as before
  for (int i = 0; i < buckets_in_hash_table; i++) {
    assert_lookup(ht, i, test_value, false);
  }

  ioopm_hash_table_destroy(ht);
}

void test_remove_deletes_entry() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  assert_insert_and_remove(ht, 1, "test");

  ioopm_hash_table_destroy(ht);
}

void test_remove_unlinks_entry() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int buckets_in_hash_table = 17;

  ioopm_hash_table_insert(ht, buckets_in_hash_table * 0, "test1");
  ioopm_hash_table_insert(ht, buckets_in_hash_table * 1, "test2");
  ioopm_hash_table_insert(ht, buckets_in_hash_table * 2, "test3");

  // Remove the entry in the middle
  ioopm_hash_table_remove(ht, buckets_in_hash_table * 1);

  // Make sure that the entry with key 2 has been removed
  // and that the link between entry 1 and 3 is not broken
  assert_lookup(ht, buckets_in_hash_table * 0, "test1", false);
  assert_lookup(ht, buckets_in_hash_table * 1, NULL, true);
  assert_lookup(ht, buckets_in_hash_table * 2, "test3", false);

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

  int test_key1 = 1;
  int test_key2 = 2;

  assert_hash_table_size(ht, 0);

  ioopm_hash_table_insert(ht, test_key1, "test");
  assert_hash_table_size(ht, 1);

  ioopm_hash_table_insert(ht, test_key2, "test");
  assert_hash_table_size(ht, 2);

  ioopm_hash_table_remove(ht, test_key2);
  assert_hash_table_size(ht, 1);

  ioopm_hash_table_remove(ht, test_key1);
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

  ioopm_hash_table_remove(ht, buckets_in_hash_table * 1);
  assert_hash_table_size(ht, 1);

  ioopm_hash_table_remove(ht, buckets_in_hash_table * 0);
  assert_hash_table_size(ht, 0);

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

  int expected_size = 3;
  int expected_keys[] = { 1, 2, 3 };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, expected_keys[i], NULL);
  }

  assert_keys_array(ht, expected_keys, expected_size);
  
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int expected_keys[0]; // create an empty array

  assert_keys_array(ht, expected_keys, 0);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys_same_bucket() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int buckets_in_hash_table = 17;
  int expected_size = 3;
  int expected_keys[] = {
    buckets_in_hash_table * 0,
    buckets_in_hash_table * 1,
    buckets_in_hash_table * 2
  };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, expected_keys[i], NULL);
  }

  assert_keys_array(ht, expected_keys, expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys_modified() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int expected_size = 3;
  int expected_keys[] = { 0, 1, 2 };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, expected_keys[i], NULL);
  }

  assert_keys_array(ht, expected_keys, expected_size);

  ioopm_hash_table_remove(ht, expected_keys[expected_size - 1]); // remove the last element from expected_keys

  int expected_modified_size = 2;
  int expected_modified_keys[] = { 0, 1 };

  // Make sure that the keys array is updated and that the key that was removed
  // is no longer present
  assert_keys_array(ht, expected_modified_keys, expected_modified_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_values() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int expected_size = 3;
  char *expected_values[] = {
    "test",
    "hello",
    "world",
  };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, i, expected_values[i]);
  }

  assert_values_array(ht, expected_values, expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_values_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int expected_size = 0;
  char *expected_values[expected_size];// create an empty array

  assert_values_array(ht, expected_values, expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_values_same_bucket() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int buckets_in_hash_table = 17;
  int expected_size = 3;
  char *expected_values[] = {
    "test",
    "hello",
    "world",
  };

  // insert entries with a value from expected_values into the same bucket
  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, buckets_in_hash_table * i, expected_values[i]);
  }

  assert_values_array(ht, expected_values, expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_values_modified() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int expected_size = 3;
  char *expected_values[] = {
    "test",
    "hello",
    "world",
  };

  // insert entries with a value from expected_values into the same bucket
  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht,  i, expected_values[i]);
  }

  assert_values_array(ht, expected_values, expected_size);

  // We do not explicitly say which keys should be used for each value,
  // but we know that the for-loop inserts each value at key 0..expected_size-1.
  // This means that to remove the last element from expected_values in the hash table
  // we simply remove the key (expected_size - 1).
  ioopm_hash_table_remove(ht, expected_size - 1);

  int expected_modified_size = 2;
  char *expected_modified_values[] = {
    "test",
    "hello",
  };

  assert_values_array(ht, expected_modified_values, expected_modified_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_key() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  int key = 999;

  ioopm_hash_table_insert(ht, key, "hello world");

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, key));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_key_invalid() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, 999));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_invalid() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, "hello world"));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_equivalent() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *value = "hello world";
  char *value_copy = strdup(value);

  ioopm_hash_table_insert(ht, 999, value);

  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value_copy));

  free(value_copy);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_identity() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *value = "hello world";

  ioopm_hash_table_insert(ht, 100, value);

  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_null() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *value = NULL;

  ioopm_hash_table_insert(ht, 100, value);

  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value));

  ioopm_hash_table_destroy(ht);
}

bool value_equiv(int key, char *value, void *x) {
  return strcmp(value, (char*)x) == 0;
}

void change_all_values(int key, char **value, void *x) {
  *value = (char*)x;
}

void test_hash_table_all() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *initial_value = "hello";

  ioopm_hash_table_insert(ht, 1, initial_value);
  ioopm_hash_table_insert(ht, 2, initial_value);
  ioopm_hash_table_insert(ht, 3, initial_value);
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, initial_value));

  // Replace the value of key 3 to another value which should cause
  // ioopm_hash_table_all to return false
  ioopm_hash_table_insert(ht, 3, "goodbye");

  CU_ASSERT_FALSE(ioopm_hash_table_all(ht, value_equiv, initial_value));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_all_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  // In our implementation, running 'ioopm_hash_table_all' on an empty hash table
  // always returns true
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, "hello world"));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_apply_all() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *initial_value = "hello";
  char *new_value = "goodbye";

  ioopm_hash_table_insert(ht, 1, initial_value);
  ioopm_hash_table_insert(ht, 2, initial_value);
  ioopm_hash_table_insert(ht, 3, initial_value);
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, initial_value));

  ioopm_hash_table_apply_to_all(ht, change_all_values, new_value);

  CU_ASSERT_FALSE(ioopm_hash_table_all(ht, value_equiv, initial_value));
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, new_value));

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
    (NULL == CU_add_test(test_suite1, "it inserts a new entry", test_insert)) ||
    (NULL == CU_add_test(test_suite1, "it inserts multiple entries", test_insert_multiple)) ||
    (NULL == CU_add_test(test_suite1, "it inserts multiple entires into the same bucket", test_insert_same_bucket)) ||
    (NULL == CU_add_test(test_suite1, "it replaces an entry when inserting with existing key", test_insert_replace)) ||
    (NULL == CU_add_test(test_suite1, "it does nothing when trying to remove an invalid key", test_remove_invalid_key)) ||
    (NULL == CU_add_test(test_suite1, "it removes an existing entry", test_remove_deletes_entry)) ||
    (NULL == CU_add_test(test_suite1, "it unlinks an existing entry", test_remove_unlinks_entry)) ||
    (NULL == CU_add_test(test_suite1, "it inserts and removes an entry when the key is 0", test_insert_key_0)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the size to 0 on a newly allocated hash table", test_hash_table_size_empty)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after inserting and removing", test_hash_table_size_not_empty)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after inserting entries into the same bucket", test_hash_table_size_same_bucket)) ||
    (NULL == CU_add_test(test_suite1, "it detects if the hash table is empty or not", test_hash_table_is_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears all entries in a non-empty hash table", test_hash_table_clear)) ||
    (NULL == CU_add_test(test_suite1, "it does not fail when trying to clear an empty hash table", test_hash_table_clear_on_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears a hash table where all entries are in the same bucket", test_hash_table_clear_same_bucket)) ||
    (NULL == CU_add_test(test_suite1, "it returns an array of all keys", test_hash_table_keys)) ||
    (NULL == CU_add_test(test_suite1, "it returns an empty array of keys when the hash table is empty", test_hash_table_keys_empty)) ||
    (NULL == CU_add_test(test_suite1, "it returns an array of all keys when inserted into the same bucket", test_hash_table_keys_same_bucket)) ||
    (NULL == CU_add_test(test_suite1, "it returns an updated array of keys after removing", test_hash_table_keys_modified)) ||
    (NULL == CU_add_test(test_suite1, "it returns an array of all values", test_hash_table_values)) ||
    (NULL == CU_add_test(test_suite1, "it returns an empty array of values when the hash table is empty", test_hash_table_values_empty)) ||
    (NULL == CU_add_test(test_suite1, "it returns an array of all values when inserted into the same bucket", test_hash_table_values_same_bucket)) ||
    (NULL == CU_add_test(test_suite1, "it returns an updated array of values after removing", test_hash_table_values_modified)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with a valid key", test_hash_table_has_key)) ||
    (NULL == CU_add_test(test_suite1, "it returns false when searching for an entry with an invalid key", test_hash_table_has_key_invalid)) ||
    (NULL == CU_add_test(test_suite1, "it returns false when searching for an entry with an invalid value", test_hash_table_has_value_invalid)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with value being a copy", test_hash_table_has_value_equivalent)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with value being of the same identity", test_hash_table_has_value_identity)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with value set to NULL", test_hash_table_has_value_null)) ||
    (NULL == CU_add_test(test_suite1, "it returns true if all entries matches the predicate", test_hash_table_all)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when applying a predicate to an empty hash table", test_hash_table_all_empty)) ||
    (NULL == CU_add_test(test_suite1, "it applies a function to all entries and updates the values", test_hash_table_apply_all))
   ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
