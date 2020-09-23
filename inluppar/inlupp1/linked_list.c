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

struct iter {
  link_t *current;
  ioopm_list_t *list;
};

static void link_destroy(link_t *link) {
  free(link);
}

static link_t *link_create(int value, link_t *next) {
  // Allocate memory for the new entry.
  link_t *result = calloc(1, sizeof(link_t));

  // The allocated memory is filled with the new entry.
  *result  = (link_t){
    .value = value,
    .next  = next,
  };

  return result;
}

/// @brief Removes a link from the linked list and deallocates the memory
/// @param list the list to remove a link from
/// @param previous the previous link to the link that should be removed (may not be NULL)
/// @param remove the link to be removed (may not be NULL)
static int remove_link(ioopm_list_t *list, link_t *previous, link_t *remove) {
  // Save the value of the link that should be removed
  int value = remove->value;

  // Update the previous next pointer to point to the element after
  // the removed element or NULL (if the removed element is the last)
  previous->next = remove->next;
  list->size--;

  link_destroy(remove);

  return value;
}

ioopm_list_t *ioopm_linked_list_create() {
  ioopm_list_t *result = calloc(1, sizeof(ioopm_list_t));
  link_t *dummy        = link_create(0, NULL);

  // Create an empty hash table and assign to the allocated memory
  *result  = (ioopm_list_t){
    //Borde vi inte sätta så att dummy-first pekar på dummy-last?
    // Och vi borde kunna se till så att dummy-last försvinner när vi skapar en ny med append, annars hamnar den mitt i smeten?
    .first = dummy,
    .last  = dummy,
    .size  = 0,
  };

  return result;
}

void ioopm_linked_list_destroy(ioopm_list_t *list) {
  // The first link of the list is a dummy entry
  link_t *link = list->first;
  link_t *tmp;

  while (link != NULL) {
    tmp  = link->next;
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
    list->last->next  = new_link;
  }

  // Make sure to always update the last pointer when appending
  list->last = new_link;

  list->size++;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, int value) {
  // TODO: Jag har satt den här så att dummyn alltid behålls
  link_t *new_link = link_create(value, NULL);

  if (ioopm_linked_list_size(list) == 0) {
    //if the size is 0, prepend is the same as append, put it after the dummy which is always first.
    list->last = new_link;
    list->first->next = new_link;
  } else {
    // Put the first link in the list in the new_links next-pointer.
    link_t *old_first = list->first->next;
    new_link->next = old_first;
  }

  //update the pointer to be next;
  list->first->next = new_link;
  list->size++;
}

void ioopm_linked_list_insert(ioopm_list_t *list, int index, int value) {
  //Vad händer om vi skickar in ett index-värde större än size?
  //Svar: Vi sätter den sist woop woop
  if (index >= list->size) {
    ioopm_linked_list_append(list, value);
  } else if (index <= 0) {
    ioopm_linked_list_prepend(list, value);
  } else {
    link_t *first      = list->first;
    link_t *prev_link  = first->next;

    //Iterate until we're at the key before our chosen index.
    while (index != 1) {
      prev_link = prev_link->next;
      index--;
    }

    //Extract our new values next link.
    link_t *next     = prev_link->next;

    link_t *new_link = link_create(value, next);
    //Put our new link at chosen index.
    prev_link->next  = new_link;
    //Increment size of list by 1.
    list->size++;
  }
}

int ioopm_linked_list_remove(ioopm_list_t *list, int index) {
  int size = ioopm_linked_list_size(list);

  // Make sure that the index is in the range [0..n-1]
  // and that we have at least one element in the linked list
  if (index < 0 || index >= size){
    FAILURE();
    return -1;
  }

  int removed_value;
  bool should_update_last = false;
  link_t *dummy = list->first;

  // We know that if this code runs, the size must be >= 1
  // and therefore there must be at least one non-dummy entry
  link_t *current = dummy->next;

  // If the first index is specified, we simply remove that element
  // directly, rather than going through the entire list
  if (index == 0) {
    removed_value = remove_link(list, dummy, current);
  } else {
    // Check if the last pointer in ioopm_list_t should be updated
    should_update_last = index == size - 1;

    // Find the previous link to the one we want to remove.
    // We only have to check until index == 1 to get the previous element
    // since we will only go [0..index-1] steps
    while (index != 1) {
      current = current->next;
      index--;
    }

    // Update the last pointer to be the previous link
    if (should_update_last) {
      list->last = current;
    }

    // Since current refers to the previous link, current->next will never be NULL
    removed_value = remove_link(list, current, current->next);
  }

  SUCCESS();
  return removed_value;
}

int ioopm_linked_list_size(ioopm_list_t *list) {
  return list->size;
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list) {
  return list->size == 0;
}

void ioopm_linked_list_clear(ioopm_list_t *list) {
  //Save the dummy node.
  link_t *first = list->first;
  link_t *link  = first->next;
  link_t *tmp;

  //destroy each link whilst decrementing the size of list by 1.
  while (list->size != 0) {
    tmp = link->next;
    link_destroy(link);
    link = tmp;
    list->size--;
  }

  //Set dummy-nodes next to NULL to avoid leaks, and set last pointer to dummy.
  first->next = NULL;
  list->last = first;
}

bool ioopm_linked_list_contains(ioopm_list_t *list, int value) {
  link_t *first = list->first;
  link_t *last = list->last;
  link_t *current;

  if (first->value == value || last->value == value) {
    return true;
  }

  current = first->next;
  while(current != NULL) {
    if (current->value == value) {
      return true;
    }

    current = current->next;
  }

  return false;
}

int ioopm_linked_list_get(ioopm_list_t *list, int index) {
  //TODO RENSKRIV SKITEN
  if (index < 0 || index >= list->size){
    FAILURE();
    return -1;
  }

  link_t *link = list->first->next;


  while (index != 0){
    link = link->next;
    index--;
  }

  SUCCESS();
  return link->value;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_char_predicate prop, void *extra){
  link_t *link = list->first->next;

  while (link != NULL){
    if (!prop(link->value, extra)) return false;

    link = link->next;
  }

  return true;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_char_predicate prop, void *extra){
  link_t *link = list->first->next;

  while (link != NULL) {
    if (prop(link->value, extra)) return true;

    link = link->next;
  }

  return false;
}

void ioopm_linked_apply_to_all(ioopm_list_t *list, ioopm_apply_char_function fun, void *extra){
  link_t *link = list->first->next;

  while(link != NULL){
    fun(&link->value, extra);
    link = link->next;
  }
}

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list){
  ioopm_list_iterator_t *result = calloc(1, sizeof(ioopm_list_iterator_t));

  *result = (ioopm_list_iterator_t){
    .current = list->first,
  };

  return result;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter){
  return iter->current != NULL;
}

int ioopm_iterator_current(ioopm_list_iterator_t *iter){
  return iter->current->value;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter){
  free(iter);
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter){
  iter->current = iter->list->first;
}
