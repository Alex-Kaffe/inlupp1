#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "hash_table.h"
#include "linked_list.h"

#define Delimiters "+-#@()[]{}.,:;!? \t\n\r"



int string_knr_hash(elem_t key) {
  unsigned int result = 0;
  char *str = (char*)key.p;
  
  do {
    result = result * 31 + *str;
  } while (*++str != '\0');
  
  return result;
}

bool eq_elem_int(elem_t a, elem_t b) {
  return a.i == b.i;
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
  printf("%s\n", word);
}

void process_file(char *filename, ioopm_hash_table_t *ht) {
  FILE *f = fopen(filename, "r");

  while (true) {
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
      printf("%s\n", word);
      elem_t count = ioopm_hash_table_lookup(ht, ptr_elem(word));
      
      if (HAS_ERROR()) {
        process_word(ht, word, 1);
      } else {
        process_word(ht, word, count.i + 1);
      }
    }

    free(buf);
  }
  
  fclose(f);
}

int main(int argc, char *argv[]) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(eq_elem_int, string_knr_hash); /// FIXME: initialise with your hash_table
  
  if (argc <= 1) {
    puts("Usage: freq-count file1 ... filen");
    return 1;
  }
  
  for (int i = 1; i < argc; ++i) {
    process_file(argv[i], ht);
  }

  int size = ioopm_hash_table_size(ht);
  printf("%d\n\n", size);
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);
  
  //printf("%d", ioopm_hash_table_lookup(ht, ptr_elem("hello")));
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(keys);
  printf("keys: %d\n\n", ioopm_linked_list_size(keys));
  printf("keys: %d\n\n", ioopm_linked_list_get(keys, ));
  
  //sort_keys(keys, size);
  elem_t current, next;
  
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
  
  while (ioopm_iterator_has_next(iterator)) {
    current = ioopm_iterator_next(iterator);
    printf("%s: %d\n", (char*)current.p, ioopm_hash_table_lookup(ht, current).i);
  }
  
  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(keys);
  ioopm_hash_table_destroy(ht);
}