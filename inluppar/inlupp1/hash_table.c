#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

#define NO_BUCKETS 17

typedef struct entry entry_t;

// Errors when looking up a key will be saved into the global 'errno' variable
extern int errno;

struct entry {
  elem_t key;       // holds the key
  elem_t value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
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

static int extract_int_hash_key(elem_t key) {
  return key.i;
}

static entry_t *find_previous_entry_for_key(entry_t *entry, int key) {
  entry_t *current = entry;

  //Söker igenom tills next == null, eller om nästa i tablen har nyckeln som vi ska sätta in.
  while (current->next != NULL && extract_int_hash_key(current->next->key) != key) {
    current = current->next;
  }

  return current;
}

static bool is_valid_key(int key) {
  return key >= 0;
}

static bool key_equiv(elem_t key, elem_t value_ignored, void *x) {
  return key.i == ((elem_t*)x)->i;
}

/// @brief Compares two union types and checks if the pointer values are the same (char*)
static bool value_equiv(elem_t key, elem_t value, void *x) {
  char *a = value.p;
  char *b = ((elem_t*)x)->p;

  // Running strcmp with NULL will cause a segfault
  // so we compare it separately
  if (a == NULL || b == NULL) {
    return a == b;
  }

  return strcmp(a, b) == 0;
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_eq_function eq_func, ioopm_hash_function hash_func) {
  // Allocate space for a ioopm_hash_table_t = NO_BUCKETS pointers to
  // entry_t's, which will be set to NULL
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));

  // Set the hash function
  if (hash_func == NULL) {
    ht->hash_func = extract_int_hash_key;
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
  int hashed_key = ht->hash_func(key);
  
  if (is_valid_key(hashed_key)) {
    int bucket = hashed_key % NO_BUCKETS;
  
    entry_t *tmp = find_previous_entry_for_key(ht->buckets[bucket], hashed_key);
    entry_t *next = tmp->next;
  
    if (next && extract_int_hash_key(next->key) == hashed_key) {
      SUCCESS();
      return next->value;
    }
  }

  FAILURE();
  return ptr_elem(NULL);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
  int hashed_key = ht->hash_func(key);
  
  if (is_valid_key(hashed_key)) {
    /// Calculate the bucket for this entry
    int bucket = hashed_key % NO_BUCKETS;
  
    /// Search for an existing entry for a key
    entry_t *entry = find_previous_entry_for_key(ht->buckets[bucket], hashed_key);
    entry_t *next = entry->next;
  
    /// Check if the next entry should be updated or not
    if (next != NULL && extract_int_hash_key(next->key) == hashed_key) {
      next->value = value;
    } else {
      entry->next = entry_create(int_elem(hashed_key), value, next);
    }
  
    SUCCESS();
  } else {
    FAILURE();
  }
}

elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key) {
  int hashed_key = ht->hash_func(key);
  
  if (is_valid_key(hashed_key)) {
    int bucket = hashed_key % NO_BUCKETS;
    entry_t *dummy = ht->buckets[bucket];
    
    // If the bucket is not empty and the key is valid, try to remove the key-value pair
    if (dummy->next != NULL) {
      entry_t *previous_entry = find_previous_entry_for_key(dummy, hashed_key);
      entry_t *current_entry = previous_entry->next;
      elem_t value = current_entry->value;
      
      // find_previous_entry_for_key returns the dummy entry if no entry was found
      // so we check if the returned next entry has the correct key
      if (extract_int_hash_key(current_entry->key) == hashed_key) {
        previous_entry->next = current_entry->next;
      
        entry_destroy(current_entry);
      
        return value;
      }
    }
  }
  
  FAILURE();
  return ptr_elem(NULL);
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht) {
  size_t counter = 0;
  entry_t *current_entry;

  for (int i = 0; i < NO_BUCKETS ; i++) {
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
  entry_t *current_entry;

  for (int i = 0; i < NO_BUCKETS ; i++){
    current_entry = ht->buckets[i];

    //If the dummy entry points to another entry, the hash_table isn't empty
    if (current_entry->next != NULL){
      return false;
    }
  }

  return true;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht) {
  //Loops through the array
  entry_t *dummy;
  entry_t *next_entry;
  entry_t *tmp;

  for (int i = 0; i < NO_BUCKETS ; i ++){
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

  for (int i = 0; i < NO_BUCKETS; i++) {
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

  for (int i = 0; i < NO_BUCKETS; i++) {
    current = ht->buckets[i]->next;

    while (current != NULL) {
      ioopm_linked_list_append(list, current->value);
      current = current->next;
    }
  }

  return list;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  bool result = true;
  entry_t *entry;

  for(int i = 0; i < NO_BUCKETS ; i++){
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      result = result && pred(entry->key, entry->value, arg);
      entry = entry->next;
    }
  }

  return result;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg){
  entry_t *entry;

  for(int i = 0; i < NO_BUCKETS ; i++){
    entry = ht->buckets[i]->next;

    while(entry != NULL) {
      apply_fun(entry->key, &entry->value, arg);
      entry = entry->next;
    }
  }
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg){
  for (int i = 0; i < NO_BUCKETS; i++){
    entry_t *entry = ht->buckets[i];

    while(entry != NULL) {
      if (pred(entry->key, entry->value, arg)) return true;
      entry = entry->next;
    }
  }

  return false;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key){
  return ioopm_hash_table_any(ht, key_equiv, &key);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value) {
  return ioopm_hash_table_any(ht, value_equiv, &value);
}