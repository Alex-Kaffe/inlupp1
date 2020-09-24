typedef struct iter ioopm_list_iterator_t;

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step
/// @param iter the iterator
/// @return the next element
int ioopm_iterator_next(ioopm_list_iterator_t *iter);

/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return the removed element or sets errno to EINVAL if next has not been called or
///         if the the linked list is empty
int ioopm_iterator_remove(ioopm_list_iterator_t *iter);

/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted or sets errno to EINVAL if next has not been called
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, int element);

/// @brief Reposition the iterator to the dummy entry of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the current element or sets errno to EINVAL if next has not been called
int ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter);
