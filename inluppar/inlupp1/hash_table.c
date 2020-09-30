#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

#define DEFAULT_BUCKETS 17
#define DEFAULT_LOAD_FACTOR 0.75

typedef struct entry entry_t;

struct entry {
  elem_t key;     // holds the key
  elem_t value;   // holds the value
  entry_t *next;  // points to the next entry (possibly NULL)
};

struct hash_table {
  size_t size;
  unsigned long total_buckets;
  float load_factor;
  entry_t *buckets[DEFAULT_BUCKETS];
  ioopm_eq_function eq_key;
  ioopm_eq_function eq_value;
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

/// @brief Inserts dummy nodes into each bucket
static void create_dummies(entry_t **buckets, unsigned long total_buckets) {
  for (unsigned long i = 0; i < total_buckets; i++){
    //Create a dummy value in each bucket with some random values (they will never be read)
    buckets[i] = entry_create(int_elem(0), ptr_elem(NULL), NULL);
  }
}

/// @brief Checks if the current size exceeds the current load factor
static bool should_increase_buckets(ioopm_hash_table_t *ht) {
  return ht->load_factor * ht->total_buckets < ht->size;
}

static unsigned long extract_hash_code(elem_t key) {
  return key.unsigned_long;
}

/// @brief Finds the previous entry for a hash code (key)
/// @param hash_func the hash function used by the hash table to calculate hash codes from keys
/// @param entry the entry to start searching from (generally the dummy)
/// @param hash_code the hash code to find
/// @returns the previous entry or sets errno to EINVAL if the key was not found
static entry_t *find_previous_entry_for_key(ioopm_eq_function eq_key, entry_t *entry, elem_t key) {
  entry_t *current = entry;

  //Söker igenom tills next == null, eller om nästa i tablen har nyckeln som vi ska sätta in.
  while (current->next != NULL && !eq_key(current->next->key, key)) {
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

/// @brief Increases the value of a pointer by one each call
/// @param x a pointer to a size_t variable
static void count_elements(elem_t key, elem_t *value, void *x) {
  *(size_t*)x += 1;
}

/// @brief Used in conjuction with apply_to_all to insert keys into a linked list
/// @param x a pointer to a linked list
static void append_key_to_list(elem_t key, elem_t *value, void *x) {
  ioopm_list_t *list = x;
  ioopm_linked_list_append(list, key);
}

/// @brief Used in conjuction with apply_to_all to insert values into a linked list
/// @param x a pointer to a linked list
static void append_value_to_list(elem_t key, elem_t *value, void *x) {
  ioopm_list_t *list = x;
  ioopm_linked_list_append(list, *value);
}

static bool value_compare_pred(elem_t key, elem_t value, void *x) {
  compare_data_t *data = (compare_data_t*)x;
  return data->eq_func(value, data->element);
}

static bool key_compare_pred(elem_t key, elem_t value, void *x) {
  compare_data_t *data = (compare_data_t*)x;
  return data->eq_func(key, data->element);
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_eq_function eq_key, ioopm_eq_function eq_value, ioopm_hash_function hash_func) {
  return ioopm_hash_table_create_load_factor(eq_key, eq_value, hash_func, DEFAULT_LOAD_FACTOR);
}

ioopm_hash_table_t *ioopm_hash_table_create_load_factor(
  ioopm_eq_function eq_key,
  ioopm_eq_function eq_value,
  ioopm_hash_function hash_func,
  float load_factor
) {
  // Allocate space for a ioopm_hash_table_t = NO_BUCKETS pointers to
  // entry_t's, which will be set to NULL
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
  
  *ht = (ioopm_hash_table_t){
    .size = 0,
    .load_factor = load_factor,
    .total_buckets = DEFAULT_BUCKETS,
    .eq_key = eq_key,
    .eq_value = eq_value,
    .hash_func = extract_hash_code,
  };

  // If the user did not provide a hash func, default to the integer value
  if (hash_func == NULL) {
    ht->hash_func = extract_hash_code;
  } 

  create_dummies(ht->buckets, ht->total_buckets);

  return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
  // Deallocate all values
  ioopm_hash_table_clear(ht);

  // Deallocate dummy entries
  for (unsigned long i = 0; i < ht->total_buckets; i ++){
    free(ht->buckets[i]);
  }

  free(ht);
}

elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key) {
  unsigned long hashed_key = ht->hash_func(key);
  unsigned long bucket = hashed_key % ht->total_buckets;

  entry_t *tmp = find_previous_entry_for_key(ht->eq_key, ht->buckets[bucket], key);
  entry_t *next = tmp->next;

  if (!HAS_ERROR()) {
    return next->value;
  }

  return ptr_elem(NULL);
}


static void resize_hash_table(ioopm_hash_table_t *ht) {
  size_t primes[] = {17, 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381};
  size_t i = 0;
  
  unsigned long previous_total_buckets = ht->total_buckets;
  
  while(previous_total_buckets != primes[i]){
    i++;
  }
  
  unsigned long total_buckets2 = primes[i+1];
  // TODO: Calculate the correct amount of buckets
  unsigned long total_buckets = previous_total_buckets * 2;
  
  // Save the previous buckets array
  entry_t **previous_buckets = ht->buckets;
  
  // TODO: Resize the old array instead?
  entry_t *new_buckets = calloc(total_buckets, sizeof(entry_t*)); 
  
  //----------------------------_;:: 
  
  // Create dummy entries for each new bucket 
  create_dummies(&new_buckets, total_buckets);
  
  // TODO: Can this be used instead?
  //ioopm_hash_table_apply_to_all(ht, rehash_entry, buckets);
  
  // Set the current buckets array with the newly allocated one
  *ht->buckets = new_buckets;
  ht->total_buckets = total_buckets;
  
  entry_t *entry;
  unsigned long hashed_key;

  // Iterate through the old buckets-array and insert them into the new array
  for (unsigned long i = 0; i < previous_total_buckets; i++){
    entry = previous_buckets[i]->next;

    while (entry != NULL) {
      ioopm_hash_table_insert(ht, entry->key, entry->value);
      entry = entry->next;
    }
  }
  
  free(previous_buckets);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
  unsigned long hashed_key = ht->hash_func(key);

  /// Calculate the bucket for this entry
  unsigned long bucket = hashed_key % ht->total_buckets;

  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht->eq_key, ht->buckets[bucket], key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (!HAS_ERROR()) {
    next->value = value;
  } else {
    entry->next = entry_create(key, value, next);
    ht->size++;

    // TODO: Probably should not run this each time we insert when resizing
    if (should_increase_buckets(ht)) {
      resize_hash_table(ht);
    }

    // Reset errno
    SUCCESS();
  }
}

elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key) {
  unsigned long hashed_key = ht->hash_func(key);
  unsigned long bucket = hashed_key % ht->total_buckets;
  entry_t *dummy = ht->buckets[bucket];

  // If the bucket is not empty and the key is valid, try to remove the key-value pair
  if (dummy->next != NULL) {
    entry_t *previous_entry = find_previous_entry_for_key(ht->eq_key, dummy, key);
    entry_t *current_entry = previous_entry->next;

    // find_previous_entry_for_key sets errno to EINVAL if no previous entry was found
    if (!HAS_ERROR()) {
      previous_entry->next = current_entry->next;

      // Save the value before deallocating
      elem_t value = current_entry->value;

      entry_destroy(current_entry);

      ht->size--;
      SUCCESS();
      return value;
    }
  }

  FAILURE();
  return ptr_elem(NULL);
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht){
  return ht->size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht) {
  return ht->size == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht) {
  //Loops through the array
  entry_t *dummy;
  entry_t *next_entry;
  entry_t *tmp;

  for (unsigned long i = 0; i < ht->total_buckets ; i ++){
    dummy = ht->buckets[i];
    next_entry = dummy->next;
    dummy->next = NULL; // make sure that the dummy does not point to an unallocated entry

    while (next_entry != NULL) {
      tmp = next_entry->next;
      entry_destroy(next_entry);
      next_entry = tmp;
    }
  }
  
  ht->size = 0;
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht) {
  ioopm_list_t *list = ioopm_linked_list_create(ht->eq_key);
  ioopm_hash_table_apply_to_all(ht, append_key_to_list, list);
  return list;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht) {
  ioopm_list_t *list = ioopm_linked_list_create(ht->eq_value);
  ioopm_hash_table_apply_to_all(ht, append_value_to_list, list);
  return list;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  entry_t *entry;

  for(unsigned long i = 0; i < ht->total_buckets; i++){
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

  for(unsigned long i = 0; i < ht->total_buckets; i++){
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      apply_fun(entry->key, &entry->value, arg);
      entry = entry->next;
    }
  }
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  entry_t *entry;

  for (unsigned long i = 0; i < ht->total_buckets; i++) {
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      if (pred(entry->key, entry->value, arg)) return true;
      entry = entry->next;
    }
  }

  return false;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key){
  compare_data_t data = { .eq_func = ht->eq_key, .element = key };
  return ioopm_hash_table_any(ht, key_compare_pred, &data);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value) {
  compare_data_t data = { .eq_func = ht->eq_value, .element = value };
  return ioopm_hash_table_any(ht, value_compare_pred, &data);
}
