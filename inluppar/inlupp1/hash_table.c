#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

#define NO_BUCKETS 17

typedef struct entry entry_t;
typedef struct compare_data compare_data_t;

/// @brief Used as the extra argument in predicates when comparing values or keys in the hash table
/// This is needed since the value and key are generic types and they are compared using either
/// the hash_func or eq_func. The predicate and apply functions only accept one extra argument
/// of an arbitrary type, meaning that we need a struct to pass in more than one value.
struct compare_data {
  void *func;     // The function used in the comparison
  elem_t element; // The element to compare to
};

struct entry {
  elem_t key;     // holds the key
  elem_t value;   // holds the value
  entry_t *next;  // points to the next entry (possibly NULL)
};

struct hash_table {
  entry_t *buckets[NO_BUCKETS];
  ioopm_eq_function eq_func;
  ioopm_hash_function hash_func;
};

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next) {
  // Allocate memory for the new entry.
  entry_t *result = calloc(1, sizeof(entry_t));

  // Create the new entry.
  *result = (entry_t){
    .key = key,
    .value = value,
    .next = next,
  };

  return result;
}

static void entry_destroy(entry_t *entry){
  free(entry);
}

static unsigned int extract_hash_code(elem_t key) {
  return key.unsigned_int;
}

/// @brief Finds the previous entry for a hash code (key)
/// @param hash_func the hash function used by the hash table to calculate hash codes from keys
/// @param entry the entry to start searching from (generally the dummy)
/// @param hash_code the hash code to find
/// @returns the previous entry or sets errno to EINVAL if the key was not found
static entry_t *find_previous_entry_for_key(ioopm_hash_function hash_func, entry_t *entry, unsigned int hash_code) {
  entry_t *current = entry;

  //Söker igenom tills next == null, eller om nästa i tablen har nyckeln som vi ska sätta in.
  while (current->next != NULL && hash_func(current->next->key) != hash_code) {
    current = current->next;
  }

  if (current->next == NULL) {
    // If no previous entry was found, set errno
    FAILURE();
    return entry;
  }

  SUCCESS();
  return current;
}

static bool value_compare_pred(elem_t key, elem_t value, void *x) {
  compare_data_t *data = (compare_data_t*)x;
  ioopm_eq_function eq_func = (ioopm_eq_function)data->func;
  
  return eq_func(value, data->element);
}

static bool key_compare_pred(elem_t key, elem_t value, void *x) {
  compare_data_t *data = (compare_data_t*)x;
  ioopm_hash_function hash_func = (ioopm_hash_function)data->func;
  
  return hash_func(key) == hash_func(data->element);
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_eq_function eq_func, ioopm_hash_function hash_func) {
  // Allocate space for a ioopm_hash_table_t = NO_BUCKETS pointers to
  // entry_t's, which will be set to NULL
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));

  // Set the hash function
  if (hash_func == NULL) {
    ht->hash_func = extract_hash_code;
  } else {
    ht->hash_func = hash_func;
  }

  ht->eq_func = eq_func;

  for (int i = 0; i < NO_BUCKETS; i++){
    //Create a dummy value in each bucket with some random values (they will never be read)
    ht->buckets[i] = entry_create(int_elem(0), ptr_elem(NULL), NULL);
  }

  return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
  // Deallocate all values
  ioopm_hash_table_clear(ht);

  // Deallocate dummy entries
  for (int i = 0; i < NO_BUCKETS; i ++){
    free(ht->buckets[i]);
  }

  free(ht);
}

elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key) {
  unsigned int hashed_key = ht->hash_func(key);
  unsigned int bucket = hashed_key % NO_BUCKETS;

  entry_t *tmp = find_previous_entry_for_key(ht->hash_func, ht->buckets[bucket], hashed_key);
  entry_t *next = tmp->next;

  if (!HAS_ERROR()) {
    return next->value;
  }

  return ptr_elem(NULL);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
  unsigned int hashed_key = ht->hash_func(key);

  /// Calculate the bucket for this entry
  unsigned int bucket = hashed_key % NO_BUCKETS;

  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht->hash_func, ht->buckets[bucket], hashed_key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (!HAS_ERROR()) {
    next->value = value;
  } else {
    entry->next = entry_create(key, value, next);

    // Reset errno
    SUCCESS();
  }
}

elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key) {
  unsigned int hashed_key = ht->hash_func(key);
  unsigned int bucket = hashed_key % NO_BUCKETS;
  entry_t *dummy = ht->buckets[bucket];

  // If the bucket is not empty and the key is valid, try to remove the key-value pair
  if (dummy->next != NULL) {
    entry_t *previous_entry = find_previous_entry_for_key(ht->hash_func, dummy, hashed_key);
    entry_t *current_entry = previous_entry->next;

    // find_previous_entry_for_key sets errno to EINVAL if no previous entry was found
    if (!HAS_ERROR()) {
      previous_entry->next = current_entry->next;

      // Save the value before deallocating
      elem_t value = current_entry->value;

      entry_destroy(current_entry);

      SUCCESS();
      return value;
    }
  }

  FAILURE();
  return ptr_elem(NULL);
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht) {
  size_t counter = 0;
  entry_t *current_entry;

  for (unsigned int i = 0; i < NO_BUCKETS; i++) {
    current_entry = ht->buckets[i];
    while (current_entry->next != NULL){
      //If bucket_size > 1, It counts the dummy entry, whilst skipping the last entry.
      current_entry = current_entry->next;
      counter++;
    }
  }

  return counter;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht) {
  return ioopm_hash_table_size(ht) == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht) {
  //Loops through the array
  entry_t *dummy;
  entry_t *next_entry;
  entry_t *tmp;

  for (unsigned int i = 0; i < NO_BUCKETS ; i ++){
    dummy = ht->buckets[i];
    next_entry = dummy->next;
    dummy->next = NULL; // make sure that the dummy does not point to an unallocated entry

    while (next_entry != NULL) {
      tmp = next_entry->next;
      entry_destroy(next_entry);
      next_entry = tmp;
    }
  }
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht) {
  ioopm_list_t *list = ioopm_linked_list_create(ht->eq_func);
  entry_t *current;

  for (unsigned int i = 0; i < NO_BUCKETS; i++) {
    current = ht->buckets[i]->next;

    while (current != NULL) {
      ioopm_linked_list_append(list, current->key);
      current = current->next;
    }
  }

  return list;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht) {
  ioopm_list_t *list = ioopm_linked_list_create(ht->eq_func);
  entry_t *current;

  for (unsigned int i = 0; i < NO_BUCKETS; i++) {
    current = ht->buckets[i]->next;

    while (current != NULL) {
      ioopm_linked_list_append(list, current->value);
      current = current->next;
    }
  }

  return list;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  entry_t *entry;

  for(unsigned int i = 0; i < NO_BUCKETS ; i++){
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      if (!pred(entry->key, entry->value, arg)) return false;
      entry = entry->next;
    }
  }

  return true;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg){
  entry_t *entry;

  for(unsigned int i = 0; i < NO_BUCKETS; i++){
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      apply_fun(entry->key, &entry->value, arg);
      entry = entry->next;
    }
  }
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  entry_t *entry;

  for (unsigned int i = 0; i < NO_BUCKETS; i++) {
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      if (pred(entry->key, entry->value, arg)) return true;
      entry = entry->next;
    }
  }

  return false;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key){
  compare_data_t data = { .func = ht->hash_func, .element = key };
  return ioopm_hash_table_any(ht, key_compare_pred, &data);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value) {
  compare_data_t data = { .func = ht->eq_func, .element = value };
  return ioopm_hash_table_any(ht, value_compare_pred, &data);
}
