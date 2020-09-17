#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include "hash_table.h"

#define NO_BUCKETS 17

typedef struct entry entry_t;

// Errors when looking up a key will be saved into the global 'errno' variable
extern int errno;

struct entry {
  int key;       // holds the key
  char *value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table {
  entry_t *buckets[NO_BUCKETS];
};

static entry_t *entry_create(int key, char *value, entry_t *next){
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

static entry_t *find_previous_entry_for_key(entry_t *entry, int key)
{
  entry_t *current = entry;

  //Söker igenom tills next == null, eller om nästa i tablen har nyckeln som vi ska sätta in.
  while (current->next != NULL && current->next->key != key) {
    current = current->next;
  }

  return current;
}

static bool is_valid_key(int key) {
  return key >= 0;
}

ioopm_hash_table_t *ioopm_hash_table_create() {
  // Allocate space for a ioopm_hash_table_t = NO_BUCKETS pointers to
  // entry_t's, which will be set to NULL
  ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));

  for (int i = 0 ; i < NO_BUCKETS ; i++){
    //Create a dummy value in each bucket.
    result->buckets[i] = entry_create(0, NULL, NULL);
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

char *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key) {
  if (is_valid_key(key)) {
    /// Find the previous entry for key
    entry_t *tmp = find_previous_entry_for_key(ht->buckets[key % NO_BUCKETS], key);
    entry_t *next = tmp->next;

    if (next && next->key == key) {
      SUCCESS();
      return next->value;
    }
  }

  FAILURE();
  return NULL;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value) {
  if (!is_valid_key(key)) {
    FAILURE();
    return;
  }

  /// Calculate the bucket for this entry
  int bucket = key % NO_BUCKETS;
  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht->buckets[bucket], key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (next != NULL && next->key == key) {
    next->value = value;
  } else {
    entry->next = entry_create(key, value, next);
  }

  SUCCESS();
}

char *ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key){
  if (!is_valid_key(key)) {
    FAILURE();
    return "Invalid key";
  }

  char *str = ioopm_hash_table_lookup(ht, key);
  int bucket = key % NO_BUCKETS;

  if (HAS_ERROR()) {
    // No need to call FAILURE macro since errno already is set to EINVAL after
    // calling 'ioopm_hash_table_lookup'
    return "Does not exist in the hash table";
  }

  entry_t *previous_entry = find_previous_entry_for_key(ht->buckets[bucket], key);
  entry_t *current_entry = previous_entry->next;

  previous_entry->next = current_entry->next;

  free(current_entry);

  return str;
}

int ioopm_hash_table_size(ioopm_hash_table_t *ht) {
  int counter = 0;
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

int *ioopm_hash_table_keys(ioopm_hash_table_t *ht) {
  int size = ioopm_hash_table_size(ht);
  
  // Allocate memory for an empty keys array (storing only the termination value -1)
  int *keys = calloc(size + 1, sizeof(int)); // reserve one element for the termination value
  
  entry_t *bucket;
  entry_t *current;
 
  int iteration = 0;
   
  for (int i = 0 ; i < NO_BUCKETS; i++) {
    bucket = ht->buckets[i];
    current = bucket->next;
    
    while (current != NULL) {
      keys[iteration] = current->key;
      current = current->next;
      iteration++;
    }
  }
  
  // Set the termination value
  keys[size] = -1;

  return keys;
}

char **ioopm_hash_table_values(ioopm_hash_table_t *ht) {
  int size = ioopm_hash_table_size(ht);
  
  // Allocate memory for an empty values array (storing only the termination value NULL)
  char **values = calloc(size + 1, sizeof(char*));
  
  entry_t *bucket;
  entry_t *current;
 
  int iteration = 0;
   
  for (int i = 0 ; i < NO_BUCKETS; i++) {
    bucket = ht->buckets[i];
    current = bucket->next;
    
    while (current != NULL) {
      values[iteration] = current->value;
      current = current->next;
      iteration++;
    }
  }
  
  values[size] = NULL;

  return values;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, int key) {
  ioopm_hash_table_lookup(ht, key);
  
  // HAS_ERROR() returns true if the key does NOT exist
  // so we must invert the value since true should be returned
  // only if the key DOES exist
  return !HAS_ERROR();
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, char *value) {
  entry_t *bucket;
  entry_t *current;
  
  for (int i = 0 ; i < NO_BUCKETS; i++) {
    bucket = ht->buckets[i];
    current = bucket->next;
    
    while (current != NULL) {
      char *current_value = current->value;
      
      if (
        ((current_value == NULL || value == NULL) && current_value == value) ||
        (strcmp(current_value, value) == 0) 
      ) {
        return true; 
      }
      
      current = current->next;
    }
  }
  
  return false;
}