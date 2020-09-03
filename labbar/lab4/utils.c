#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

bool is_number(char *str) {
  if (*str == '-') {
    str++;
  }

  do {
    if (!isdigit(*str)) return false;
    str++;
  } while (*str != '\0');

  return true;
}

bool not_empty(char *str)
{
  return strlen(str) > 0;
}

bool is_shelf(char *str) {
  // the two if-statements should be combined somehow
  if (!isalpha(*str)) return false;
  str++;
  if (*str == '-') return false;

  return is_number(str);
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
    printf("%s ", question);
    read_string(buf, buf_siz); // we dont need to use the return value, since we use the 'check' function

    if (check(buf)) {
      break;
    }
  }

  return convert(buf);
}

int ask_question_int(char *question) {
  return ask_question(question, is_number, (convert_func) atoi).int_value;
}

int ask_question_int_limit(char *question, int min, int max) {
  int value;

  do {
    value = ask_question_int(question);
  } while (value < min || value > max);

  return value;
}

char *ask_question_string(char *question) {
  return ask_question(question, not_empty, (convert_func) strdup).string_value;
}

char *ask_question_shelf(char *question) {
  return ask_question(question, is_shelf, (convert_func) strdup).string_value;
}

// mutates the original string
char *trim(char *str) {
  // if first char is the termination character, then the length is 0
  if (str[0] == '\0') {
    return str;
  }

  int length = strlen(str);
  int start_index = 0;
  int end_index = length - 1;

  for (int i = 0; i < end_index; i++) {
    if (isspace(str[i])) {
      start_index++;
    } else {
      break;
    }
  }

  // if the first index of non-space characters is the same as the last index,
  // it means that all the characters in 'str' are trash.
  if (start_index == end_index) {
    str[0] = '\0';
    return str;
  }

  for (int i = end_index; i >= 0; i--) {
    if (isspace(str[i])) {
      end_index--;
    } else {
      break;
    }
  }

  int new_length = end_index - start_index + 1;

  // if the new length is not smaller than the original length,
  // there is no need to modify the string
  if (new_length < length) {
    for (int i = 0; i < new_length; i++) {
      str[i] = str[start_index + i];
    }

    // we know that 'new_length' is smaller than 'length',
    // so now overflow error possible
    str[new_length] = '\0';
  }

  return str;
}
