#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include "list_linked.h"

typedef struct link link_t;

struct link {
  int element;
  link_t *next;
};

struct list {
  link_t *links[17];
  link_t *first;
  link_t *last;
  int size;
};

static void link_destroy(link_t *link){
  free(link);
}


ioopm_list_t *ioopm_linked_list_create(){
  ioopm_list_t *result = calloc(1, sizeof(ioopm_list_t));
  /*
  for (int i = 0 ; i < 17 ; i++){
    //Create a dummy value in each bucket.
    result->links[i] = link_create(0, NULL);
  }
  */
  return result;
}


void ioopm_linked_list_destroy(ioopm_list_t *list){
  free(list);
}
/*  //Loops through the array
  list_t *link;
  list_t *tmp;

  for (int i = 0; i < 17 ; i ++){
    entry = ht->links[i];
    while (link->next != NULL){
      tmp = link->next;
      link_destroy(link);
      link = tmp;
    }

    free(entry);
  }
*/

void ioopm_linked_list_append(ioopm_list_t *list, int value){
  link_t *result = calloc(1, sizeof(link_t));
  
  link_t *first = list->first;
  link_t new_link = { .element = value, .next = first};
  
  *result = new_link;
  
  list->first = result;
  
}
