#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

#define DEFAULT_CAPACITY 17
#define DEFAULT_LOAD_FACTOR 0.75
#define GROWTH_FACTOR 2

typedef struct entry entry_t;

//@brief the entries that reside within the hash table.
struct entry {
  elem_t key;     // holds the key
  elem_t value;   // holds the value
  entry_t *next;  // points to the next entry (possibly NULL)
};

//@brief a hash table, containing its equality functions, the size, buckets containing the entries, and the hash function.
struct hash_table {
  size_t size;                   // Holds the amount of entries.
  size_t capacity;               // How many buckets there are in the table. 
  float load_factor;             // How many entries/bucket before resizing.
  ioopm_eq_function eq_key;      // equality function for keys.
  ioopm_eq_function eq_value;    // equality function for the values.
  ioopm_hash_function hash_func; // The hashing function.
  entry_t **buckets;             // Pointer towards the buckets.
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

/// @brief Checks if the current size exceeds the current load factor
static bool should_increase_buckets(float load_factor, size_t capacity, size_t size) {
  return load_factor * capacity < size;
}

/// @brief Calculates the new capacity for the hash table based on the previous capacity
static size_t get_new_capacity(ioopm_hash_table_t *ht) {
  return ht->capacity * GROWTH_FACTOR;
}

static unsigned long extract_hash_code(elem_t key) {
  return key.unsigned_long;
}

/// @brief Inserts dummy nodes into each bucket
static entry_t **create_buckets(size_t capacity) {
  entry_t **buckets = calloc(capacity, sizeof(entry_t*));
  
  for (size_t i = 0; i < capacity; i++){
    //Create a dummy value in each bucket with some random values (they will never be read)
    buckets[i] = entry_create(int_elem(0), ptr_elem(NULL), NULL);
  }
  
  return buckets;
}


// @brief Resizing the hashtable creating
static void resize_hash_table(ioopm_hash_table_t *ht) {
  entry_t **old_buckets = ht->buckets;
  size_t old_capacity = ht->capacity;
  
  // Update the capacity
  ht->capacity = get_new_capacity(ht);
  
  // Update the capacity of the hash table and allocate memory
  // for the resized hash table and insert dummy entries
  ht->buckets = create_buckets(ht->capacity);
  
  // Reset the size, since inserting will increase the size
  ht->size = 0;

  entry_t *entry, *tmp;

  for (size_t i = 0; i < old_capacity; i++){
    entry = old_buckets[i]->next;

    while (entry != NULL) {
      tmp = entry->next;
      ioopm_hash_table_insert(ht, entry->key, entry->value);

      // Insert is pass by value, meaning that we can safely destroy the previous entry
      entry_destroy(entry);
      entry = tmp;
    }

    free(old_buckets[i]);
  }

  free(old_buckets);
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


ioopm_hash_table_t *ioopm_hash_table_create(
  ioopm_eq_function eq_key,
  ioopm_eq_function eq_value,
  ioopm_hash_function hash_func
) {
  return ioopm_hash_table_create_custom(eq_key, eq_value, hash_func, DEFAULT_LOAD_FACTOR, DEFAULT_CAPACITY);
}

ioopm_hash_table_t *ioopm_hash_table_create_custom(
  ioopm_eq_function eq_key,
  ioopm_eq_function eq_value,
  ioopm_hash_function hash_func,
  float load_factor,
  size_t capacity
) {
  // Allocate space for a ioopm_hash_table_t and an array of buckets with the size of capacity
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));

  *ht = (ioopm_hash_table_t){
    .size = 0,
    .capacity = capacity,
    .load_factor = load_factor,
    .eq_key = eq_key,
    .eq_value = eq_value,
  };

  // If the user did not provide a hash func, default to the integer value
  if (hash_func == NULL) {
    ht->hash_func = extract_hash_code;
  } else {
    ht->hash_func = hash_func;
  }

  // Allocate memory for the buckets and insert dummies
  ht->buckets = create_buckets(capacity);

  return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
  // Deallocate all values
  ioopm_hash_table_clear(ht);

  // Deallocate dummy entries
  for (size_t i = 0; i < ht->capacity; i ++){
    free(ht->buckets[i]);
  }

  free(ht->buckets);
  free(ht);
}

elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key) {
  unsigned long hashed_key = ht->hash_func(key);
  unsigned long bucket = hashed_key % ht->capacity;

  entry_t *tmp = find_previous_entry_for_key(ht->eq_key, ht->buckets[bucket], key);
  entry_t *next = tmp->next;

  //Check if the entry existed in the hashtable.
  if (!HAS_ERROR()) {
    return next->value;
  }

  return ptr_elem(NULL);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
  unsigned long hashed_key = ht->hash_func(key);

  /// Calculate the bucket for this entry
  unsigned long bucket = hashed_key % ht->capacity;

  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht->eq_key, ht->buckets[bucket], key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (!HAS_ERROR()) {
    next->value = value;
  } else {
    entry->next = entry_create(key, value, next);
    ht->size++;

    if (should_increase_buckets(ht->load_factor, ht->capacity, ht->size)) {
      resize_hash_table(ht);
    }

    // Reset errno
    SUCCESS();
  }
}

elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key) {
  unsigned long hashed_key = ht->hash_func(key);
  unsigned long bucket = hashed_key % ht->capacity;
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

  for (unsigned long i = 0; i < ht->capacity ; i ++){
    dummy = ht->buckets[i];
    next_entry = dummy->next;
    dummy->next = NULL; // make sure that the dummy does not point to an unallocated entry

    while (next_entry != NULL) {
      //Iterate through the bucket, destroying each entry.
      tmp = next_entry->next;
      entry_destroy(next_entry);
      next_entry = tmp;
    }
  }

  ht->size = 0;
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht) {
  //Creates a list, appending each key to the list and returns it.
  ioopm_list_t *list = ioopm_linked_list_create(ht->eq_key);
  ioopm_hash_table_apply_to_all(ht, append_key_to_list, list);
  return list;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht) {
  //Creates a list, appending each value to the list and returns it.
  ioopm_list_t *list = ioopm_linked_list_create(ht->eq_value);
  ioopm_hash_table_apply_to_all(ht, append_value_to_list, list);
  return list;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  entry_t *entry;

  for(size_t i = 0; i < ht->capacity; i++){
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      //If an entry doesn't conform to the predicate, return false.
      if (!pred(entry->key, entry->value, arg)) return false;
      entry = entry->next;
    }
  }

  return true;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg){
  entry_t *entry;

  for(size_t i = 0; i < ht->capacity; i++){
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      apply_fun(entry->key, &entry->value, arg);
      entry = entry->next;
    }
  }
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  entry_t *entry;

  for (size_t i = 0; i < ht->capacity; i++) {
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
