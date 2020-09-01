#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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

int main(int argc, char *argv[])
{
  if (argc != 3) {
    puts("Usage: ./a.out number1 number2");
    return 1;
  }

  if (!is_number(argv[1]) || !is_number(argv[2])) {
    puts("This program only supports numbers as command line arguments");
    return 1;
  }

  int number1 = atoi(argv[1]);
  int number2 = atoi(argv[2]);
  int done = 0;

  if (number1 < 0 || number2 < 0) {
    puts("This program only supports positive numbers");
    return 1;
  } else if (number1 == 0 && number2 > 0) {
    printf("gcd(%d, %d) = %d\n", number1, number2, number2);
    done = 1;
  } else if (number1 > 0 && number2 == 0) {
    printf("gcd(%d, %d) = %d\n", number1, number2, number1);
    done = 1;
  }

  // We dont want to modify the original values, since they
  // are used when printing
  int a = number1;
  int b = number2;

  while (done == 0) {
    if (a == b) {
      printf("gcd(%d, %d) = %d\n", number1, number2, a);
      done = 1;
    } else if (a > b) {
      a -= b;
    } else {
      b -= a;
    }
  }

  return 0;
}
