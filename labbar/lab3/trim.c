#include "utils.h"
#include <stdio.h>

int main(void) {
  char str1[] = "  hej  ";
  char str2[] = "  h ej  ";
  char str3[] = "  hej\t ";
  char str4[] = "  hej\t \n";
  char str5[] = "heeeeeeej";
  char str6[] = "          ";

  char *tests[] = { str1, str2, str3, str4, str5, str6 };

  for (int i = 0; i < 6; i++) {
    printf("Utan trim: '%s'\n", tests[i]);
    printf("Med trim: '%s'\n", trim(tests[i]));
  }

  return 0;
}
