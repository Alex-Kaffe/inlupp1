#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if (argc != 3) {
    puts("Usage: ./a.out rows growth");
    return 1;
  }

  int total = 0;
  int rows = atoi(argv[1]);
  int growth = atoi(argv[2]);

  for (int i = 1; i <= rows; i++)
  {
    for (int j = 0; j < (growth * i); j += growth) {
      printf("*");
      total += growth;
    }

    printf("\n");
  }

  printf("Totalt: %d\n", total);

  return 0;
}
