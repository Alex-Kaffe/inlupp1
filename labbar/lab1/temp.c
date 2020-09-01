#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool is_number(char *str) {
  int length = strlen(str);
  int startIndex = 0;

  if (length == 0) {
    return false;
  } else if (str[0] == '-' && length > 1) {
    startIndex = 1; // no need to include the '-' in the for-loop
  }

  for (int i = startIndex; i < length; i++) {
    if (!isdigit(str[i])) {
      return false;
    }
  }

  return true;
}

int main(int argc, char *argv[]) {
  if (argc > 1 && is_number(argv[1])) {
    printf("%s is a number\n", argv[1]);
  } else {
    if (argc > 1) {
      printf("%s is not a number\n", argv[1]);
    } else {
      printf("Please provide a command line argument!\n");
    }
  }
}
