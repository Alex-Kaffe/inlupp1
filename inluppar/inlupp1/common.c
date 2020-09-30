#include <string.h>
#include "common.h"

/// @brief Compares the char* pointers of two elem_t
bool eq_elem_string(elem_t a, elem_t b){
  char *p1 = a.extra;
  char *p2 = b.extra;

  if (p1 == NULL && p1 == p2) {
    return true;
  }

  return strcmp((char*)p1, (char*)p2) == 0;
}

/// @brief Compares the integer value of two elem_t
bool eq_elem_int(elem_t a, elem_t b) {
  return a.integer == b.integer;
}