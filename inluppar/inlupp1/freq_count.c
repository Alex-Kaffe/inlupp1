#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

#define Delimiters "+-#@()[]{}.,:;!? \t\n\r"

unsigned long string_hash(elem_t key) {
  unsigned long result = 0;
  char *str = (char*)key.extra;

  do {
    result = result * 31 + *str;
  } while (*++str != '\0');
  
  return result;
}
/*
bool eq_elem_string(elem_t a, elem_t b){
  char *p1 = a.extra;
  char *p2 = b.extra;

  if (p1 == NULL && p1 == p2) {
    return true;
  }

  return strcmp((char*)p1, (char*)p2) == 0;
}

bool eq_elem_int(elem_t a, elem_t b) {
  return a.integer == b.integer;
}*/

//Compares two strings.
static int cmpstringp(const void *p1, const void *p2) {
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

//Sort keys in an array.
void sort_keys(char *keys[], size_t no_keys) {
  qsort(keys, no_keys, sizeof(char*), cmpstringp);
}

//Process the words and insert.
void process_word(ioopm_hash_table_t *ht, char *word, size_t count) {
  ioopm_hash_table_insert(ht, ptr_elem(word), int_elem(count));
}

void process_file(char *filename, ioopm_hash_table_t *ht) {
  FILE *f = fopen(filename, "r");

  while (true) {
    char *word_dup = NULL;
    char *buf = NULL;
    size_t len = 0;
    getline(&buf, &len, f);

    if (feof(f)) {
      free(buf);
      break;
    }

  //Allokeras i process_file kallelsen i strdup
  //Frigörs i main senare efter att det blivit uppropat från array
  // Invalid read av storleken som kommit från find_prev_entry

    for (char *word = strtok(buf, Delimiters);
       word && *word;
       word = strtok(NULL, Delimiters)
    ) {
      elem_t count = ioopm_hash_table_lookup(ht, ptr_elem(word));

      if (HAS_ERROR()) {
        // Only duplicate the word in memory if the word does not exist.
        // If we were to always duplicate the string, multiple insertions would
        // each cause a single memory leak, since new duplications will not
        // be saved in the hash table and will therefore not be deallocated at the end.
        word_dup = strdup(word);
        process_word(ht, word_dup, 1);
      } else {
        process_word(ht, word, count.integer + 1);
      }
    }

    free(buf);
  }

  fclose(f);
}

int main(int argc, char *argv[]) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_string, eq_elem_int, string_hash);

  if (argc <= 1) {
    ioopm_hash_table_destroy(ht);
    puts("Usage: freq-count file1 ... filen");
    return 1;
  }
  
  for (int i = 1; i < argc; ++i) {
    process_file(argv[i], ht);
  }


  size_t size = ioopm_hash_table_size(ht);
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(keys);

  printf("Total unique words: %zu\n", size);
  
  char *arr[size];
  size_t i = 0;
  elem_t current;
  
  // Create array with words
  while (ioopm_iterator_has_next(iterator)) {
    current = ioopm_iterator_next(iterator);
    arr[i] = current.extra; 
    i++;
  }
  
  sort_keys(arr, size);
  
  // Print all words
  char *current_word;
  for (i = 0; i < size; i++) {
    current_word = arr[i];
    printf("%s: %d\n", current_word, ioopm_hash_table_lookup(ht, ptr_elem(current_word)).integer);
  }

  for (i = 0 ; i < size ; i++){
    // Deallocate each duplicated word string.
    // This is not handled by the hash table, since the value is not stored in the hash table
    // directly, but rather as a pointer.
    free(arr[i]);
  }
  
  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(keys);
  ioopm_hash_table_destroy(ht);
}
