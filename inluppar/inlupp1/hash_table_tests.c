#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

// TODO: Remove this
#define NO_BUCKETS 17

int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

// Hash function for strings
unsigned long string_sum_hash(elem_t key) {
  unsigned int result = 0;
  char *str = (char*)key.extra;

  do {
    result += *str;
  } while (*++str != '\0');

  return result;
}

bool value_equiv(elem_t key, elem_t value, void *x) {
  return strcmp(value.extra, ((elem_t*)x)->extra) == 0;
}

void change_all_values(elem_t key, elem_t *value, void *x) {
  *value = *(elem_t*)x;
}

void assert_elems_equal(elem_t a, elem_t b) {
  CU_ASSERT_TRUE(a.integer == b.integer);
}

void assert_hash_table_size(ioopm_hash_table_t *ht, size_t expected_size) {
  size_t size = ioopm_hash_table_size(ht);

  CU_ASSERT_EQUAL(size, expected_size);
}

void assert_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t value, bool should_have_error) {
  elem_t lookup_value = ioopm_hash_table_lookup(ht, key);

  CU_ASSERT_EQUAL(HAS_ERROR(), should_have_error);

  if (!should_have_error) {
    // Only assert string equality when we expect no errors
    // since string comparisons will cause a segfault when a value is NULL
    CU_ASSERT_TRUE(eq_elem_string(lookup_value, value));
  }
}

// Inserts an entry into the hash table and makes sure that it is inserted correctly using lookup
void assert_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
  ioopm_hash_table_insert(ht, key, value);
  assert_lookup(ht, key, value, false);
}

// Removes an entry from the hash table and makes sure that it no longer exists
// using lookup
void assert_remove(ioopm_hash_table_t *ht, elem_t key) {
  ioopm_hash_table_remove(ht, key);
  assert_lookup(ht, key, ptr_elem(NULL), true);
}

void assert_insert_and_remove(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
  assert_insert(ht, key, value);
  assert_remove(ht, key);
}

void assert_keys_array(ioopm_hash_table_t *ht, elem_t expected_keys[], size_t expected_size) {
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);

  CU_ASSERT_EQUAL(ioopm_linked_list_size(keys), expected_size);

  for (int i = 0; i < expected_size; i++) {
    // Make sure that all keys are present in the keys array
    // and that they are in the expected order.
    assert_elems_equal(ioopm_linked_list_get(keys, i), expected_keys[i]);
  }

  // Make sure to clean up the allocated keys array
  ioopm_linked_list_destroy(keys);
}

void assert_values_array(ioopm_hash_table_t *ht, elem_t expected_values[], size_t expected_size) {
  ioopm_list_t *values = ioopm_hash_table_values(ht);

  CU_ASSERT_EQUAL(ioopm_linked_list_size(values), expected_size);

  elem_t value;

  for (int i = 0; i < expected_size; i++) {
    value = ioopm_linked_list_get(values, i);
    // Make sure that all values are present in the values array
    // and that they are in the expected order.
    CU_ASSERT_TRUE(eq_elem_string(value, expected_values[i]));
  }

  // Make sure to clean up the allocated values array
  ioopm_linked_list_destroy(values);
}

void test_create_destroy() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);
  CU_ASSERT_PTR_NOT_NULL(ht);
  ioopm_hash_table_destroy(ht);
}

// Test that there are no valid keys after creating an empty hash table
void test_lookup() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  // Make sure that each bucket is empty (except for the dummy entry)
  // Also make sure that accessing a bucket with a key larger than the amount of buckets
  // resolves to NULL
  for (int i = 0; i < NO_BUCKETS; ++i) {
    assert_lookup(ht, int_elem(i), ptr_elem(NULL), true);
  }

  assert_lookup(ht, int_elem(-1), ptr_elem(NULL), true);

  ioopm_hash_table_destroy(ht);
}

void test_lookup_string_key() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_string, eq_elem_string, string_sum_hash);

  elem_t key1 = ptr_elem("hello");
  elem_t key2 = ptr_elem("goodbye");

  elem_t value1 = ptr_elem("world");
  elem_t value2 = ptr_elem("cruel world");

  assert_lookup(ht, key1, ptr_elem(NULL), true);
  assert_lookup(ht, key2, ptr_elem(NULL), true);

  ioopm_hash_table_insert(ht, key1, value1);
  ioopm_hash_table_insert(ht, key2, value2);

  assert_lookup(ht, key1, value1, false);
  assert_lookup(ht, key2, value2, false);

  ioopm_hash_table_remove(ht, key1);
  ioopm_hash_table_remove(ht, key2);

  assert_lookup(ht, key1, ptr_elem(NULL), true);
  assert_lookup(ht, key2, ptr_elem(NULL), true);

  ioopm_hash_table_destroy(ht);
}

void test_insert() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  assert_insert(ht, int_elem(1), ptr_elem("test"));
  assert_insert(ht, int_elem(2), ptr_elem("test"));
  assert_insert(ht, int_elem(3), ptr_elem("test"));

  ioopm_hash_table_destroy(ht);
}

void test_insert_replace() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t dummy_key = int_elem(1);

  assert_insert(ht, dummy_key, ptr_elem("test"));
  assert_insert(ht, dummy_key, ptr_elem("hello"));

  ioopm_hash_table_destroy(ht);
}

void test_insert_multiple() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  assert_insert(ht, int_elem(1), ptr_elem("test1"));
  assert_insert(ht, int_elem(2), ptr_elem("test2"));
  assert_insert(ht, int_elem(3), ptr_elem("test3"));

  ioopm_hash_table_destroy(ht);
}

void test_remove_invalid_key() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  char *test_value = "test";

  ioopm_hash_table_insert(ht, int_elem(1), ptr_elem(test_value));

  // Try to remove an invalid key
  ioopm_hash_table_remove(ht, int_elem(-1));

  assert_lookup(ht, int_elem(1), ptr_elem(test_value), false);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_resize() {
  unsigned long initial_buckets = 17;
  size_t expected_size = 4;

  ioopm_hash_table_t *ht = ioopm_hash_table_create_custom(
    eq_elem_int,
    eq_elem_string,
    NULL,
    0.5,
    initial_buckets
  );

  // Insert 4 elements to cause a resize (since 0.5*7 == 3.5)
  elem_t expected_keys[] = {
    int_elem(100),
    int_elem(300),
    int_elem(600),
    int_elem(700),
  };

  elem_t expected_values[] = {
    ptr_elem("hello"),
    ptr_elem("world"),
    ptr_elem("goodbye"),
    ptr_elem("dude"),
  };

  for (size_t i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, expected_keys[i], expected_values[i]);
  }

  ioopm_list_t *keys = ioopm_hash_table_keys(ht);
  ioopm_list_t *values = ioopm_hash_table_values(ht);

  CU_ASSERT_EQUAL(ioopm_linked_list_size(keys), expected_size);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(values), expected_size);

  elem_t key;

  // Rehashing the hash table will cause a reorder of elements,
  // since they will most likely get put into different buckets than previously.
  // Because of this, we can not use the assert_*_array function
  for (size_t i = 0; i < expected_size; i++) {
    // Make sure that key-value
    key = ioopm_linked_list_get(keys, i);

    for (size_t j = 0; j < expected_size; j++) {
      if (eq_elem_int(expected_keys[j], key)) {
        CU_ASSERT_TRUE(eq_elem_string(ioopm_linked_list_get(values, j), expected_values[i]));
      }
    }
  }

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_remove(ht, expected_keys[i]);
  }

  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);
  ioopm_hash_table_destroy(ht);
}

void test_remove_deletes_entry() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  assert_insert_and_remove(ht, int_elem(1), ptr_elem("test"));

  ioopm_hash_table_destroy(ht);
}

void test_remove_unlinks_entry() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  ioopm_hash_table_insert(ht, int_elem(NO_BUCKETS * 0), ptr_elem("test1"));
  ioopm_hash_table_insert(ht, int_elem(NO_BUCKETS * 1), ptr_elem("test2"));
  ioopm_hash_table_insert(ht, int_elem(NO_BUCKETS * 2), ptr_elem("test3"));

  // Remove the entry in the middle
  ioopm_hash_table_remove(ht, int_elem(NO_BUCKETS * 1));

  // Make sure that the entry with key 2 has been removed
  // and that the link between entry 1 and 3 is not broken
  assert_lookup(ht, int_elem(NO_BUCKETS * 0), ptr_elem("test1"), false);
  assert_lookup(ht, int_elem(NO_BUCKETS * 1), ptr_elem(NULL), true);
  assert_lookup(ht, int_elem(NO_BUCKETS * 2), ptr_elem("test3"), false);

  ioopm_hash_table_destroy(ht);
}

// Previously, we had a bug where a insertion would fail when the key was 0
// This was fixed by using 'find_previous_entry_for_key' and the dummy entries
// This will make sure that it does not reappear later
void test_insert_key_0() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  assert_insert_and_remove(ht, int_elem(0), ptr_elem("test"));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_size_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  // A newly allocated hash table is empty (though it does have dummy entries)
  assert_hash_table_size(ht, 0);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_size_not_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t test_key1 = int_elem(1);
  elem_t test_key2 = int_elem(2);

  assert_hash_table_size(ht, 0);

  ioopm_hash_table_insert(ht, test_key1, ptr_elem("test"));
  assert_hash_table_size(ht, 1);

  ioopm_hash_table_insert(ht, test_key2, ptr_elem("test"));
  assert_hash_table_size(ht, 2);

  ioopm_hash_table_remove(ht, test_key2);
  assert_hash_table_size(ht, 1);

  ioopm_hash_table_remove(ht, test_key1);
  assert_hash_table_size(ht, 0);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_is_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t dummy_key = int_elem(1);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_insert(ht, dummy_key, ptr_elem("test"));
  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_remove(ht, dummy_key);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_clear() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  ioopm_hash_table_insert(ht, int_elem(1), ptr_elem("test"));
  ioopm_hash_table_insert(ht, int_elem(2), ptr_elem("test"));
  ioopm_hash_table_insert(ht, int_elem(3), ptr_elem("test"));

  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_clear(ht);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_clear_on_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  size_t expected_size = 3;
  elem_t expected_keys[] = {
    int_elem(1),
    int_elem(2),
    int_elem(3),
  };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, expected_keys[i], ptr_elem(NULL));
  }

  assert_keys_array(ht, expected_keys, expected_size);

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys_strings() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_string, eq_elem_int, NULL);

  size_t expected_size = 3;
  elem_t expected_keys[] = {
    ptr_elem("hello"),
    ptr_elem("world"),
    ptr_elem("dude"),
  };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, expected_keys[i], int_elem(0));
  }

  ioopm_list_t *keys = ioopm_hash_table_keys(ht);

  CU_ASSERT_EQUAL(ioopm_linked_list_size(keys), expected_size);

  for (int i = 0; i < expected_size; i++) {
    // Make sure that all keys are present in the keys array
    // and that they are in the expected order.
    CU_ASSERT_TRUE(ioopm_linked_list_contains(keys, expected_keys[i]));
  }

  CU_ASSERT_FALSE(ioopm_linked_list_contains(keys, ptr_elem("goodbye")));
  CU_ASSERT_FALSE(ioopm_linked_list_contains(keys, ptr_elem("hellos")));

  // Make sure to clean up the allocated keys array
  ioopm_linked_list_destroy(keys);

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  size_t expected_size = 0;
  elem_t expected_keys[expected_size]; // create an empty array

  assert_keys_array(ht, expected_keys, expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_keys_modified() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  size_t expected_size = 3;
  elem_t expected_keys[] = {
    int_elem(0),
    int_elem(1),
    int_elem(2),
  };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, expected_keys[i], ptr_elem(NULL));
  }

  assert_keys_array(ht, expected_keys, expected_size);

  ioopm_hash_table_remove(ht, expected_keys[expected_size - 1]); // remove the last element from expected_keys

  size_t expected_modified_size = 2;
  elem_t expected_modified_keys[] = {
    int_elem(0),
    int_elem(1),
  };

  // Make sure that the keys array is updated and that the key that was removed
  // is no longer present
  assert_keys_array(ht, expected_modified_keys, expected_modified_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_values() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  size_t expected_size = 3;
  elem_t expected_values[] = {
    ptr_elem("test"),
    ptr_elem(NULL),
    ptr_elem("world"),
  };

  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht, int_elem(i), expected_values[i]);
  }

  assert_values_array(ht, expected_values, expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_values_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  size_t expected_size = 0;
  elem_t expected_values[expected_size];// create an empty array

  assert_values_array(ht, expected_values, expected_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_values_modified() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  size_t expected_size = 3;
  elem_t expected_values[] = {
    ptr_elem("test"),
    ptr_elem("hello"),
    ptr_elem("world"),
  };

  // insert entries with a value from expected_values into the same bucket
  for (int i = 0; i < expected_size; i++) {
    ioopm_hash_table_insert(ht,  int_elem(i), expected_values[i]);
  }

  assert_values_array(ht, expected_values, expected_size);

  // We do not explicitly say which keys should be used for each value,
  // but we know that the for-loop inserts each value at key 0..expected_size-1.
  // This means that to remove the last element from expected_values in the hash table
  // we simply remove the key (expected_size - 1).
  ioopm_hash_table_remove(ht, int_elem(expected_size - 1));

  size_t expected_modified_size = 2;
  elem_t expected_modified_values[] = {
    ptr_elem("test"),
    ptr_elem("hello"),
  };

  assert_values_array(ht, expected_modified_values, expected_modified_size);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_key() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t key = int_elem(999);

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, key));

  ioopm_hash_table_insert(ht, key, ptr_elem("hello world"));

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, key));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_key_string() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_string, eq_elem_string, string_sum_hash);

  elem_t key = ptr_elem("hello");

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, key));

  ioopm_hash_table_insert(ht, key, ptr_elem("world"));

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, key));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_key_invalid() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, int_elem(999)));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_invalid() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, ptr_elem("hello world")));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_equivalent() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  char *value = "hello world";
  char *value_copy = strdup(value);

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, ptr_elem(value_copy)));

  ioopm_hash_table_insert(ht, int_elem(999), ptr_elem(value));

  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, ptr_elem(value_copy)));

  free(value_copy);

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_identity() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t value = ptr_elem("hello world");

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, value));

  ioopm_hash_table_insert(ht, int_elem(100), value);

  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_value_null() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t value = ptr_elem(NULL);

  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, value));

  ioopm_hash_table_insert(ht, int_elem(100), value);

  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_all() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t initial_value = ptr_elem("hello");

  ioopm_hash_table_insert(ht, int_elem(1), initial_value);
  ioopm_hash_table_insert(ht, int_elem(2), initial_value);
  ioopm_hash_table_insert(ht, int_elem(3), initial_value);
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, &initial_value));

  // Replace the value of key 3 to another value which should cause
  // ioopm_hash_table_all to return false
  ioopm_hash_table_insert(ht, int_elem(3), ptr_elem("goodbye"));

  CU_ASSERT_FALSE(ioopm_hash_table_all(ht, value_equiv, &initial_value));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_all_empty() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  // In our implementation, running 'ioopm_hash_table_all' on an empty hash table
  // always returns true
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, "hello world"));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_apply_all() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, eq_elem_string, NULL);

  elem_t initial_value = ptr_elem("hello");
  elem_t new_value = ptr_elem("goodbye");

  ioopm_hash_table_insert(ht, int_elem(1), initial_value);
  ioopm_hash_table_insert(ht, int_elem(2), initial_value);
  ioopm_hash_table_insert(ht, int_elem(3), initial_value);
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, &initial_value));

  ioopm_hash_table_apply_to_all(ht, change_all_values, &new_value);

  CU_ASSERT_FALSE(ioopm_hash_table_all(ht, value_equiv, &initial_value));
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, &new_value));

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_hash_function() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_string, eq_elem_int, string_sum_hash);

  elem_t keys[] = {
    ptr_elem("key1"),
    ptr_elem("key2"),
    ptr_elem("key3"),
  };

  elem_t values[] = {
    int_elem(1),
    int_elem(2),
    int_elem(3),
  };

  elem_t lookup_value;

  for (int i = 0; i < 3; i++) {
    ioopm_hash_table_insert(ht, keys[i], values[i]);
    lookup_value = ioopm_hash_table_lookup(ht, keys[i]);

    CU_ASSERT_EQUAL(HAS_ERROR(), false);
    CU_ASSERT_TRUE(eq_elem_int(lookup_value, values[i]));

    ioopm_hash_table_remove(ht, keys[i]);
    lookup_value = ioopm_hash_table_lookup(ht, keys[i]);

    CU_ASSERT_EQUAL(HAS_ERROR(), true);
  }

  ioopm_hash_table_destroy(ht);
}

void test_hash_table_has_int_value() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_string, eq_elem_int, string_sum_hash);

  elem_t keys[] = {
    ptr_elem("key1"),
    ptr_elem("key2"),
  };

  elem_t values[] = {
    int_elem(1),
    int_elem(2),
  };

  for (int i = 0; i < 2; i++) {
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, keys[i]));
    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, values[i]));
  }

  ioopm_hash_table_insert(ht, keys[0], values[0]);
  ioopm_hash_table_insert(ht, keys[1], values[1]);

  for (int i = 0; i < 2; i++) {
    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, keys[i]));
    CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, values[i]));
    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, keys[i]));
    CU_ASSERT_TRUE(eq_elem_int(ioopm_hash_table_lookup(ht, keys[i]), values[i]));
  }

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
    (NULL == CU_add_test(test_suite1, "it returns the value for a non-integer key", test_lookup_string_key)) ||
    (NULL == CU_add_test(test_suite1, "it inserts a new entry", test_insert)) ||
    (NULL == CU_add_test(test_suite1, "it inserts multiple entries", test_insert_multiple)) ||
    (NULL == CU_add_test(test_suite1, "it replaces an entry when inserting with existing key", test_insert_replace)) ||
    (NULL == CU_add_test(test_suite1, "it does nothing when trying to remove an invalid key", test_remove_invalid_key)) ||
    (NULL == CU_add_test(test_suite1, "it removes an existing entry", test_remove_deletes_entry)) ||
    (NULL == CU_add_test(test_suite1, "it unlinks an existing entry", test_remove_unlinks_entry)) ||
    (NULL == CU_add_test(test_suite1, "it inserts and removes an entry when the key is 0", test_insert_key_0)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the size to 0 on a newly allocated hash table", test_hash_table_size_empty)) ||
    (NULL == CU_add_test(test_suite1, "it calculates the correct size after inserting and removing", test_hash_table_size_not_empty)) ||
    (NULL == CU_add_test(test_suite1, "it detects if the hash table is empty or not", test_hash_table_is_empty)) ||
    (NULL == CU_add_test(test_suite1, "it clears all entries in a non-empty hash table", test_hash_table_clear)) ||
    (NULL == CU_add_test(test_suite1, "it does not fail when trying to clear an empty hash table", test_hash_table_clear_on_empty)) ||
    (NULL == CU_add_test(test_suite1, "it returns an array of all keys", test_hash_table_keys)) ||
    (NULL == CU_add_test(test_suite1, "it returns an array of all string-keys", test_hash_table_keys_strings)) ||
    (NULL == CU_add_test(test_suite1, "it returns an empty array of keys when the hash table is empty", test_hash_table_keys_empty)) ||
    (NULL == CU_add_test(test_suite1, "it returns an updated array of keys after removing", test_hash_table_keys_modified)) ||
    (NULL == CU_add_test(test_suite1, "it returns an array of all values", test_hash_table_values)) ||
    (NULL == CU_add_test(test_suite1, "it returns an empty array of values when the hash table is empty", test_hash_table_values_empty)) ||
    (NULL == CU_add_test(test_suite1, "it returns an updated array of values after removing", test_hash_table_values_modified)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with a valid key", test_hash_table_has_key)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with a valid string-key", test_hash_table_has_key_string)) ||
    (NULL == CU_add_test(test_suite1, "it returns false when searching for an entry with an invalid key", test_hash_table_has_key_invalid)) ||
    (NULL == CU_add_test(test_suite1, "it returns false when searching for an entry with an invalid value", test_hash_table_has_value_invalid)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with value being a copy", test_hash_table_has_value_equivalent)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with value being of the same identity", test_hash_table_has_value_identity)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an entry with value set to NULL", test_hash_table_has_value_null)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when searching for an int-entry with a valid key", test_hash_table_has_int_value)) ||
    (NULL == CU_add_test(test_suite1, "it returns true if all entries matches the predicate", test_hash_table_all)) ||
    (NULL == CU_add_test(test_suite1, "it returns true when applying a predicate to an empty hash table", test_hash_table_all_empty)) ||
    (NULL == CU_add_test(test_suite1, "it applies a function to all entries and updates the values", test_hash_table_apply_all)) ||
    (NULL == CU_add_test(test_suite1, "it can take in the hash function as an argument", test_hash_table_hash_function)) ||
    (NULL == CU_add_test(test_suite1, "it resizes when inserting 10 elements with a load factor of 0.5", test_hash_table_resize))
   ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);  // Detaljerna utav testerna skrivs ut.
  CU_basic_run_tests();               // Kör alla testen.
  CU_cleanup_registry();              // Städar upp testerna (avallokerar minnen bland annat)
  return CU_get_error();              // Returnerar alla fel som hänt
}
