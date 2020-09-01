#include "utils.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 2)
  {
    printf("Usage: %s words or string\n", argv[0]);
  }
  else
  {
    for (int i = 1; i < argc; ++i)
    {
      int expected = strlen(argv[i]);
      int actual   = string_length(argv[i]);
      printf("strlen(\"%s\")=%d\t\tstring_length(\"%s\")=%d\n",
             argv[i], expected, argv[i], actual);
    }
  }
  return 0;
}
