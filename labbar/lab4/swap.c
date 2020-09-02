#include <stdio.h>

void swap(int *a, int *b) {
  int tmp = *a; // set 'tmp' to the integer value of a
  *a = *b;      // set the integer value of 'a' to be the integer value of 'b'
  *b = tmp;     // set the integer value of 'b' to be the integer value of 'a'
}

void print(char *str) {
  char *end = str;
  while (*end != '\0' && *end != EOF) {
    putchar(*end);
    *end++;
  }
}

int main(void) {
  int x = 7;
  int y = 42;

  swap(&x, &y);
  printf("%d, %d\n", x, y);
  print("hello world\n");

  return 0;
}
