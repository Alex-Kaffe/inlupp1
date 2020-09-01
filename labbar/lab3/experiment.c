#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main(void) {
  int buf_siz = 255;
  char buf[buf_siz];

  int value = ask_question_int("Give me a number:");
  char *str = ask_question_string("Give me a word:");

  printf("%d, %s\n", value, str);
}
