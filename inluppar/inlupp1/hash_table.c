#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "linked_list.h"
#include "hash_table.h"
#include "common.h"

#define NO_BUCKETS 17

typedef struct entry entry_t;
//TODO: STEP 12.3 Add Const to all functions;

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

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next){
  // Allocate memory for the new entry.
  entry_t *result = calloc(1, sizeof(entry_t));

  // Create the new entry.
  entry_t new_entry = { .key = key, .value = value, .next = next };

  // The allocated memory is filled with the new entry.
  *result = new_entry;

  return result;
}

static void entry_destroy(entry_t *entry){
  free(entry);
}

static entry_t *find_previous_entry_for_key(ioopm_hash_function hash_func, entry_t *entry, elem_t key) {
  entry_t *current = entry;

  //Söker igenom tills next == null, eller om nästa i tablen har nyckeln som vi ska sätta in.
  while (current->next != NULL && hash_func(current->next->key) != hash_func(key)) {
    current = current->next;
  }

  return current;
}

static bool is_valid_key(elem_t key) {
  return key.i >= 0;
}

static bool key_equiv(elem_t key, elem_t value_ignored, void *x) {
  elem_t extra = *(elem_t*)x;
  return key.i == extra.i;
}

static bool value_equiv(elem_t key, elem_t value, void *x) {
  elem_t extra_value = *(elem_t*)x;
  
  char *a = value.p;
  char *b = extra_value.p;
  
  if (a == NULL || b == NULL) {
    return a == b;
  }

  return strcmp(a, b) == 0;
}

static int extract_int_hash_key(elem_t key) {
  return key.i; 
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_eq_function eq_func, ioopm_hash_function hash_func) {
  // Allocate space for a ioopm_hash_table_t = NO_BUCKETS pointers to
  // entry_t's, which will be set to NULL
  ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
  
  if (hash_func == NULL) {
    result->hash_func = extract_int_hash_key;
  } else {
    result->hash_func = hash_func; 
  }

  for (int i = 0 ; i < NO_BUCKETS ; i++){
    //Create a dummy value in each bucket with some random values (they will never be read) 
    result->buckets[i] = entry_create(int_elem(0), int_elem(0), NULL);
  }

  return result;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
  //Loops through the array
  entry_t *entry;
  entry_t *tmp;

  for (int i = 0; i < NO_BUCKETS ; i ++){
    entry = ht->buckets[i];
    while (entry->next != NULL){
      tmp = entry->next;
      entry_destroy(entry);
      entry = tmp;
    }

    free(entry);
  }

  free(ht);
}

elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key) {
  if (is_valid_key(key)) {
    int bucket = ht->hash_func(key) % NO_BUCKETS;
    
    entry_t *tmp = find_previous_entry_for_key(ht->hash_func, ht->buckets[bucket], key);
    entry_t *next = tmp->next;

    if (next && next->key.i == key.i) {
      SUCCESS();
      return next->value;
    }
  }

  FAILURE();
  return ptr_elem(NULL);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) {
  if (!is_valid_key(key)) {
    FAILURE();
    return;
  }

  /// Calculate the bucket for this entry
  int bucket = ht->hash_func(key) % NO_BUCKETS;
  
  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht->hash_func, ht->buckets[bucket], key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (next != NULL && next->key.i == key.i) {
    next->value = value;
  } else {
    entry->next = entry_create(key, value, next);
  }

  SUCCESS();
}

elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key){
  if (!is_valid_key(key)) {
    FAILURE();
    return ptr_elem(NULL);
  }

  elem_t value = ioopm_hash_table_lookup(ht, key);
  int bucket = key.i % NO_BUCKETS;

  if (HAS_ERROR()) {
    // No need to call FAILURE macro since errno already is set to EINVAL after
    // calling 'ioopm_hash_table_lookup'
    return ptr_elem(NULL);
  }

  entry_t *previous_entry = find_previous_entry_for_key(ht->hash_func, ht->buckets[bucket], key);
  entry_t *current_entry = previous_entry->next;

  previous_entry->next = current_entry->next;

  entry_destroy(current_entry);

  return value;
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

  for (int i = 0 ; i < NO_BUCKETS; i++) {
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

  for (int i = 0 ; i < NO_BUCKETS; i++) {
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