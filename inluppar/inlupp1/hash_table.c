#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "hash_table.h"

// Following the SIMPLE methodology, we are going to dodge and simplify the specification.
// Our first hash table implementation will only support integer keys and string values,
// and only support a fixed number of buckets (17).

// 1. Each index in the array covers a dynamic number of keys7 instead of a single key; and */
// 2. Each value in the array is a sequence of (key,value) pairs so that each entry in the map is represented by a corresponding (key,value) pair (that we will call an entry). */

// ACHIEVEMENT: A3
typedef struct entry entry_t;

// Errors when looking up a key will be saved into the global 'errno' variable
extern int errno;

struct entry
{
  int key;       // holds the key
  char *value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  entry_t *buckets[17];
};

static entry_t *entry_create(int key, char *value, entry_t *next){
  //Allocate memory for the new entry.
  entry_t *result = calloc(1, sizeof(entry_t));

  //Create the new entry.
  entry_t new_entry = {.key = key, .value = value,  .next = next};

  //The allocated memory is filled with the new entry.
  *result = new_entry;
  //Return the created entry.
  return result;
}


ioopm_hash_table_t *ioopm_hash_table_create() {
  // Allocate space for a ioopm_hash_table_t = 17 pointers to
  // entry_t's, which will be set to NULL
  ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));

  //Dummy values.
  int dummy_key = 0;
  char *dummy_value = NULL;
  entry_t *dummy_next = NULL;
  for (int i = 0 ; i < 17 ; i++){
    //Create a dummy value in each bucket.
    result->buckets[i] = entry_create(dummy_key, dummy_value, dummy_next);
  }
  
  return result;
}

void entry_destroy(entry_t *entry){
  free(entry);
}


void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
  // TODO: Using just free(ht) will not be enough once we are able to insert elements
  //       since each bucket is a linked list.

  
  //Loops through the array
  entry_t *entry;
  entry_t *tmp;
  
  for (int i = 0; i < 17 ; i ++){
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



char *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key) {
  // Make sure that key is greater than 0 since e.g. -1 % 17 == -1
  if (key > 0) {
    // Reset the errno to make sure that consecutive calls to this function
    // does not give an invalid result
    errno = 0;

    entry_t *first = ht->buckets[key % 17];
    entry_t *current = first;

    while (current != NULL) {
      if (current->key == key) {
        return current->value;
      }

      current = current->next;
    }
  };

  // No entry found, set error and return
  errno = EINVAL;
  return NULL;
}


void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value)
{
  /// Calculate the bucket for this entry
  int bucket = key % 17;
  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht->buckets[bucket], key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (next != NULL && next->key == key)
    {
      next->value = value;
    }
  else
    {
      entry->next = entry_create(key, value, next);
    }
}


char *ioopm_hash_table_remove(ioopm_hash_table_t *ht, int key){
  if (key > 0){
    char *str = ioopm_hash_table_lookup(ht, key);
    int bucket = key % 17;
    errno = 0;
  
    if (errno == EINVAL){
      str = "Does not exist in the hash table";
      return str;
    }
    entry_t *previous_entry = find_previous_entry_for_key(ht->buckets[bucket], key);
    entry_t *current_entry = previous_entry->next;

    previous_entry->next = current_entry->next;
  
    free(current_entry);  
  
  
    return str;
  }
  errno = EINVAL;

  return "Error, key is smaller than 0";
}
