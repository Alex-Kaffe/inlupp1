#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "linked_list.h"
#include "common.h"

typedef struct link link_t;
typedef struct compare_data compare_data_t;

// TODO: Move to common.h?
/// @brief Used as the extra argument in predicates to allow the use of eq_func
struct compare_data {
  ioopm_eq_function eq_func;  // The function used in the comparison
  elem_t element;             // The element to compare to
};

struct link {
  elem_t value;
  link_t *next;
};

struct list {
  link_t *first;
  link_t *last;
  size_t size;
  ioopm_eq_function eq_func;
};

struct iter {
  size_t index;
  link_t *current;
  ioopm_list_t *list;
};

static link_t *link_create(elem_t value, link_t *next) {
  // Allocate memory for the new entry.
  link_t *result = calloc(1, sizeof(link_t));

  // The allocated memory is filled with the new entry.
  *result = (link_t){
    .value = value,
    .next = next,
  };

  return result;
}

static void link_destroy(link_t *link) {
  free(link);
}

/// @brief Removes a link from the linked list and deallocates the memory
/// @param list the list to remove a link from
/// @param previous the previous link to the link that should be removed (may not be NULL)
/// @param remove the link to be removed (may not be NULL)
static elem_t remove_link(ioopm_list_t *list, link_t *previous, link_t *remove) {
  // Save the value of the link that should be removed
  elem_t value = remove->value;

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

// TODO: Add index check in this function
/// @param index the index to get from list (must be a valid index [0..n-1])
static link_t *get_link_from_index(ioopm_list_t *list, size_t index) {
  link_t *previous = list->first->next;

  while (index != 0) {
    previous = previous->next;
    index--;
  }

  return previous;
}

static bool value_equiv(elem_t value, void *x) {
  compare_data_t *data = x;
  return data->eq_func(value, data->element);
}

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq_func) {
  ioopm_list_t *result = calloc(1, sizeof(ioopm_list_t));
  link_t *dummy = link_create(int_elem(0), NULL);

  // Create an empty hash table and assign to the allocated memory
  *result = (ioopm_list_t){
    .size = 0,
    .first = dummy,
    .last = dummy,
    .eq_func = eq_func,
  };

  return result;
}

void ioopm_linked_list_destroy(ioopm_list_t *list) {
  // Deallocate all links
  ioopm_linked_list_clear(list);

  // Deallocate dummy entry
  free(list->first);

  free(list);
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t value) {
  link_t *new_link = link_create(value, NULL);

  // No need to handle the case where the list is empty, since
  // in that case first and last is the same (dummy entry) and updating
  // one of them will ultimately update both
  list->last->next  = new_link;
  list->last = new_link;

  list->size++;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value) {
  link_t *new_link = link_create(value, list->first->next);

  // Make sure that we update the last pointer if the list is empty
  if (ioopm_linked_list_size(list) == 0) {
    list->last = new_link;
  }

  list->first->next = new_link;
  list->size++;
}

void ioopm_linked_list_insert(ioopm_list_t *list, size_t index, elem_t value) {
  // If invalid index, return failure.
  if (index > list->size || index < 0){
    FAILURE();
    return;
  }

  if (index == list->size) {
    ioopm_linked_list_append(list, value);
  } else if (index == 0 || ioopm_linked_list_size(list) == 0) {
    ioopm_linked_list_prepend(list, value);
  } else {
    link_t *previous = get_link_from_index(list, index - 1);

    // Insert new link at the chosen index
    link_t *new_link = link_create(value, previous->next);
    previous->next = new_link;

    list->size++;
  }

  SUCCESS();
}

elem_t ioopm_linked_list_remove(ioopm_list_t *list, size_t index) {
  // Make sure that the index is in the range [0..n-1]
  // and that we have at least one element in the linked list
  if (!is_valid_index(list, index)){
    FAILURE();
    return int_elem(-1);
  }

  elem_t removed_value;
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
  link_t *first = list->first;
  link_t *link  = first->next;
  link_t *tmp;

  //destroy each link whilst decrementing the size of list by 1.
  while (link != NULL) {
    tmp = link->next;
    link_destroy(link);
    link = tmp;
  }

  list->size = 0;

  // Set next of dummy node to NULL to avoid memory leaks
  first->next = NULL;
  list->last = first;
}

bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t value) {
  compare_data_t data = { .eq_func = list->eq_func, .element = value };
  return ioopm_linked_list_any(list, value_equiv, &data);
}

elem_t ioopm_linked_list_get(ioopm_list_t *list, size_t index) {
  //if the linked list doesn't have that index, set errno to EINVAL and return.
  if (!is_valid_index(list, index)) {
    FAILURE();
    return int_elem(-1);
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
    .index = 0,
    .current = list->first,
    .list = list,
  };

  return result;
}

elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter) {
  if (!ioopm_iterator_has_next(iter)) {
    FAILURE();
    return ptr_elem(NULL);
  }

  iter->current = iter->current->next;

  // Do not increase the index if we are now
  // positioned at the last element
  if (iter->current->next != NULL) {
    iter->index++;
  }

  SUCCESS();
  return iter->current->value;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter){
  return iter->current->next != NULL;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter){
  free(iter);
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter){
  iter->current = iter->list->first;
  iter->index = 0;
}

void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t value) {
  ioopm_linked_list_insert(iter->list, iter->index, value);

  if (!ioopm_iterator_has_next(iter)) {
    // If we were previously positioned at the last element, we will no longer
    // be positioned at the last element after linserting. This means that
    // iter->current should point to the element previous to the newly inserted one.
    // Therefore, we get the link using index-1 instead of just index.
    iter->current = get_link_from_index(iter->list, iter->index - 1);
  }
}

elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter) {
  if (iter->list->size == 0) {
    FAILURE();
    return ptr_elem(NULL);
  }

  SUCCESS();

  // Prevent segfault when positioned at the last element
  if (ioopm_iterator_has_next(iter)) {
    return iter->current->next->value;
  }

  return iter->current->value;
}

elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter) {
  link_t *next = iter->current->next;

  // Delete the element at the current index
  elem_t remove_value = ioopm_linked_list_remove(iter->list, iter->index);

  // Check if we were at the last element before removing
  if (next == NULL) {
    iter->current = iter->list->last;
    iter->index--;
  }

  return remove_value;
}
