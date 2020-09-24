#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "linked_list.h"
#include "common.h"

typedef struct link link_t;

struct link {
  int value;
  link_t *next;
};

struct list {
  link_t *first;
  link_t *last;
  size_t size;
};

struct iter {
  size_t index;
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

/// @brief Checks if an index is in the range of [0..n-1]
static bool is_valid_index(ioopm_list_t *list, size_t index) {
  return (index >= 0 && index < ioopm_linked_list_size(list));
}

/// @brief Checks if an iterator has a current value/the user has called ioopm_iterator_next
static bool iterator_has_current(ioopm_list_iterator_t *iter) {
  return iter->current != NULL;
}

static link_t *get_link_from_index(ioopm_list_t *list, size_t index) {
  link_t *previous = list->first->next;

  // We want to go through the linked list index - 1 times
  // to get the previous link
  while (index != 0) {
    previous = previous->next;
    index--;
  }

  return previous;
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

  //iterate through the list, destroying each link.
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

void ioopm_linked_list_insert(ioopm_list_t *list, size_t index, int value) {
  // If invalid index, return failure.
  if (index > list->size || index < 0){
    FAILURE();
    return;
  }
  
  if (index == list->size) {
    ioopm_linked_list_append(list, value);
    //If the index is 0, or the list is empty, prepend the value.
  } else if (index == 0 || ioopm_linked_list_size(list) == 0) {
    ioopm_linked_list_prepend(list, value);
  } else {
    link_t *previous = get_link_from_index(list, index - 1);

    //Put our new link at chosen index.
    link_t *new_link = link_create(value, previous->next);
    previous->next = new_link;

    list->size++;
  }
  //Sucessful.
  SUCCESS();
}

int ioopm_linked_list_remove(ioopm_list_t *list, size_t index) {
  // Make sure that the index is in the range [0..n-1]
  // and that we have at least one element in the linked list
  if (!is_valid_index(list, index)){
    FAILURE();
    return -1;
  }

  int removed_value;
  link_t *previous;
  link_t *dummy = list->first;

  // We know that if this code runs, the size must be >= 1
  // and therefore there must be at least one non-dummy entry
  link_t *current = dummy->next;

  // If the first index is specified, we simply remove that element
  // directly, rather than going through the entire list
  if (index == 0) {
    removed_value = remove_link(list, dummy, current);
  } else {
    previous = get_link_from_index(list, index - 1);

    // Check if the last pointer in ioopm_list_t should be updated
    if (index == ioopm_linked_list_size(list) - 1) {
      list->last = previous;
    }

    removed_value = remove_link(list, previous, previous->next);
  }

  SUCCESS();
  return removed_value;
}

size_t ioopm_linked_list_size(ioopm_list_t *list) {
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

  //Check first if the first or last value is what we search for.
  //This makes for a better time-complexity in a best-case scenario.
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

int ioopm_linked_list_get(ioopm_list_t *list, size_t index) {
  //if the linked list doesn't have that index, set errno to EINVAL and return.
  if (!is_valid_index(list, index)) {
    FAILURE();
    return -1;
  }

  link_t *link = get_link_from_index(list, index);

  SUCCESS();
  return link->value;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_char_predicate prop, void *extra){
  link_t *link = list->first->next;

  //Loop through the linked list, applying the predicate on each element.
  while (link != NULL){
    //If the predicate isn't valid, return false.
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

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list) {
  ioopm_list_iterator_t *result = calloc(1, sizeof(ioopm_list_iterator_t));

  //Set the current pointer to dummy at start.
  *result = (ioopm_list_iterator_t){
    .current = list->first->next,
    .index = 0,
    .list = list,
  };

  return result;
}

int ioopm_iterator_next(ioopm_list_iterator_t *iter) {
  //Iterator goes to the next link and increase the index by 1.
  if (ioopm_iterator_has_next(iter)){
    iter->index++;
    iter->current = iter->current->next;

    SUCCESS();
    return iter->current->value;
  }

  //Errno if there is no next link.
  FAILURE();
  return 0;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter){
  return iter->current != NULL && iter->current->next != NULL;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter){
  free(iter);
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter){
  //Set index to 0 and reset the iterator on the first index.
  iter->index = 0;
  iter->current = iter->list->first->next;
}

void ioopm_iterator_insert(ioopm_list_iterator_t *iter, int value) {
  if (!iterator_has_current(iter)) {
    // If the list is empty, prepend the link and set current to it.
    ioopm_linked_list_prepend(iter->list, value);
    iter->current = iter->list->first->next;
  } else {
    // Insert as usual.
    ioopm_linked_list_insert(iter->list, iter->index, value);
    iter->current = get_link_from_index(iter->list, iter->index);
  }
}

int ioopm_iterator_current(ioopm_list_iterator_t *iter) {
  //If the iterator is on an invalid index in a list, set errno to EINVAL.
  if (!iterator_has_current(iter)) {
    FAILURE();
    return 0;
  }

  SUCCESS();
  return iter->current->value;
}

int ioopm_iterator_remove(ioopm_list_iterator_t *iter){
  //Errno if removal on a link that doesn't exist.
  if (!iterator_has_current(iter)){
    FAILURE();
    return 0;
  }

  //Save the next link.
  link_t *next_link = iter->current->next;
  int remove_value = ioopm_linked_list_remove(iter->list, iter->index);

  if (iter->index == iter->list->size) {
    // If we are at the last index, removing the current element means that
    // we must shift 1 step to the left
    iter->current = iter->list->last;
    iter->index--;
  } else {
    //After removal, the links to the right shifts one step to the left.
    iter->current = next_link;
  }

  SUCCESS();
  return remove_value;
}
