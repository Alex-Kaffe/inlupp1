#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    puts("Usage: ./a.out number");
    return 1;
  }

  int number = atoi(argv[1]);
  int limit = floor(sqrt(number)) + 1;
  int result = 1;

  if (number < 0) {
    puts("Negative numbers are not supported");
    return 1;
  } else if (number == 1 || number == 0) {
    // A prime number must be greater than 1
    result = 0;
  } else {
    for (int i = 2; i < limit; i++) {
      if (number % i == 0) {
        result = 0;
        break;
      }
    }
  }

  if (result == 0) {
    printf("%d is not a prime number\n", number);
  } else {
    printf("%d is a prime number\n", number);
  }

  return 0;
}
