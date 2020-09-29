#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

#define Delimiters "+-#@()[]{}.,:;!? \t\n\r"

unsigned int string_hash(elem_t key) {
  unsigned int result = 0;
  char *str = (char*)key.extra;

  do {
    result = result + *str;
  } while (*++str != '\0');

  return result;
}

bool eq_elem_int(elem_t a, elem_t b) {
  return a.integer == b.integer;
}

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
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, string_hash);

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

  printf("Total words: %zu\n", size);

  //sort_keys(keys, size);

  /*while (ioopm_iterator_has_next(iterator)) {
    current = ioopm_iterator_next(iterator);
    next = ioopm_iterator_current(iterator);

    if (strcmp((char*)current.p, (char*)next.p) > 0) {
      current = ioopm_iterator_remove(iterator);
      ioopm_iterator_reset(iterator);
      ioopm_iterator_insert(iterator, current);
    }
    //elem_t word = ioopm_iterator_next(iterator);
    //puts((char*)current.p);
  }*/

  //ioopm_iterator_reset(iterator);

  elem_t current;

  while (ioopm_iterator_has_next(iterator)) {
    current = ioopm_iterator_next(iterator);
    printf("%s: %d\n", (char*)current.extra, ioopm_hash_table_lookup(ht, current).integer);

    // Deallocate each duplicated word string.
    // This is not handled by the hash table, since the value is not stored in the hash table
    // directly, but rather as a pointer.
    free(current.extra);
  }

  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(keys);
  ioopm_hash_table_destroy(ht);
}
