#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "list_linked.h"

typedef struct link link_t;

struct link {
  int value;
  link_t *next;
};

struct list {
  link_t *first;
  link_t *last;
  int size;
};

static void link_destroy(link_t *link) {
  free(link);
}

static link_t *link_create(int value, link_t *next) {
  // Allocate memory for the new entry.
  link_t *result = calloc(1, sizeof(link_t));

  // The allocated memory is filled with the new entry.
  *result = (link_t){
    .value = value,
    .next = next,
  };

  return result;
}

ioopm_list_t *ioopm_linked_list_create() {
  ioopm_list_t *result = calloc(1, sizeof(ioopm_list_t));
  link_t *dummy = link_create(0, NULL);

  // Create an empty hash table and assign to the allocated memory
  *result = (ioopm_list_t){
    //Borde vi inte sätta så att dummy-first pekar på dummy-last?
    // Och vi borde kunna se till så att dummy-last försvinner när vi skapar en ny med append, annars hamnar den mitt i smeten?
    .first = dummy,
    .last = dummy,
    .size = 0,
  };

  return result;
}

void ioopm_linked_list_destroy(ioopm_list_t *list) {
  // The first link of the list is a dummy entry
  link_t *link = list->first;
  link_t *tmp;

  while (link != NULL) {
    tmp = link->next;
    link_destroy(link);
    link = tmp;
  }

  free(list);
}

void ioopm_linked_list_append(ioopm_list_t *list, int value) {
  link_t *new_link = link_create(value, NULL);

  if (ioopm_linked_list_size(list) == 0) {
    // If the size is 0, first and last points to the dummy link
    list->first->next = new_link;
  } else {
    list->last->next = new_link;
  }

  // Make sure to always update the last pointer when appending
  list->last = new_link;

  list->size++;
}

void iooopm_linked_list_prepend(ioopm_list_t *list, int value){
  // TODO: Jag har satt den här så att dummyn alltid behålls
  link_t *new_link = link_create(value, NULL);

  if (ioopm_linked_list_size(list) == 0) {
    //if the size is 0, prepend is the same as append, put it after the dummy which is always first.
    list->last = new_link;
    list->first->next = new_link;
  }
  else {
    // Put the first link in the list in the new_links next-pointer.
    link_t *old_first = list->first->next;
    new_link->next = old_first;
  }

  //update the pointer to be next;
  list->first->next = new_link;
  list->size++;
}


int ioopm_linked_list_size(ioopm_list_t *list) {
  return list->size;
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list){
  return list->size == 0;
}


void ioopm_linked_list_insert(ioopm_list_t *list, int index, int value){
  //Vad händer om vi skickar in ett index-värde större än size?
  printf("hej");
}



void ioopm_linked_list_clear(ioopm_list_t *list){
  int size = list->size;
  link_t *first = list->first;
  
  while (size != 0){
    
  }
  
}



bool ioopm_linked_list_contains(ioopm_list_t *list, int value) {
  link_t *first = list->first;
  link_t *last  = list->last;

  if (first->value == value || last->value == value){
    return true;
  }
  while(first != NULL){
    if (first-> value == value){
      return true;
    }
    first = first->next;
  }
  return false;
}


