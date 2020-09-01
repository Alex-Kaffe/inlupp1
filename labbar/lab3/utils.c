#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

bool is_number(char *str) {
  int length = strlen(str);
  int startIndex = 0;

  if (length == 0) {
    return false;
  } else {
    if (str[0] == '-' && length > 1) {
      startIndex = 1;
    }
  }

  for (int i = startIndex; i < length; i++) {
    if (!isdigit(str[i])) {
      return false;
    }
  }

  return true;
}

bool not_empty(char *str)
{
  return strlen(str) > 0;
}

void clear_input_buffer() {
  int c;

  do {
    c = getchar();
  } while (c != '\n' &&c != EOF);
}

int read_string(char *buf, int buf_siz) {
  int count;
  char c;

  for (count = 0; count < buf_siz; count++) {
    c = getchar();

    if (c == '\n' || c == EOF) {
      break;
    } else if (count == buf_siz - 1) {
      // if we haven't seen any termination chars as of now,
      // make sure to clear the input buffer before exit
      clear_input_buffer();
    } else {
      buf[count] = c;
    }
  }

  buf[count] = '\0';

  return count;
}

answer_t ask_question(char *question, check_func check, convert_func convert) {
  int buf_siz = 255;
  char buf[buf_siz];

  while (1) {
    printf("%s\n", question);
    read_string(buf, buf_siz); // we dont need to use the return value, since we use the 'check' function

    if (check(buf)) {
      break;
    }
  }

  return convert(buf);
}

int ask_question_int(char *question)
{
  return ask_question(question, is_number, (convert_func) atoi).int_value;
}

char *ask_question_string(char *question)
{
  return ask_question(question, not_empty, (convert_func) strdup).string_value;
}
