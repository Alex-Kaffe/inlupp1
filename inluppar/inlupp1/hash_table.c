#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include "hash_table.h"

#define NO_BUCKETS 17

// Following the SIMPLE methodology, we are going to dodge and simplify the specification.
// Our first hash table implementation will only support integer keys and string values,
// and only support a fixed number of buckets (NO_BUCKETS).

// 1. Each index in the array covers a dynamic number of keys7 instead of a single key; and */
// 2. Each value in the array is a sequence of (key,value) pairs so that each entry in the map is represented by a corresponding (key,value) pair (that we will call an entry). */

// ACHIEVEMENT: A3
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

void entry_destroy(entry_t *entry){
  free(entry);
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

static entry_t *find_previous_entry_for_key(entry_t *entry, int key)
{
  entry_t *first_entry = entry; // Spara första entry om inget annat hittas.

  //Söker igenom tills next == null, eller om nästa i tablen har nyckeln som vi ska sätta in.
  while (entry->next != NULL && entry->next->key != key) {
    entry = entry->next;
  }

  if (entry->next == NULL){
    return first_entry;
  }

  return entry;
}

bool is_valid_key(int key) {
  return key >= 0;
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

int ioopm_hash_table_size(ioopm_hash_table_t *ht){
  int i = 0;
  int counter = 0;

  for (; i < NO_BUCKETS ; i++){
    entry_t *first_entry = ht->buckets[i];
    while (first_entry->next != NULL){
      //If bucket_size > 1, It counts the dummy entry, whilst skipping the last entry.
      counter ++;
      first_entry = first_entry->next;
    }
  }
  return counter;
}


bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht){
  int i = 0;

  for (; i < NO_BUCKETS ; i++){
    entry_t *first_entry = ht->buckets[i];
    //If the dummy entry points to another entry, the hash_table isn't empty
    if (first_entry->next != NULL){
      return false;
    }
  }
  return true;
}



//
//void ioopm_hash_table_clear(ioopm_hash_table_t *h);



