#include <stdio.h>
#include <stdbool.h>

void insert_line_nr(int *line_nr) {
  printf("%-4d ", *line_nr);
  *line_nr += 1;
}

void cat(char *filename, int *line_nr)
{
  FILE *f = fopen(filename, "r");
  int c = fgetc(f);
  bool should_insert_line_nr = false;

  printf("========= %s ==========\n", filename);

  if (c == EOF) {
    printf("File empty\n");
  } else {
    insert_line_nr(line_nr);
    printf("%c", c); // make sure to print the first char

    while (1) {
      c = fgetc(f);

      if (c == EOF) {
        break;
      } else if (should_insert_line_nr == true) {
        insert_line_nr(line_nr);
        should_insert_line_nr = false;
      }

      fputc(c, stdout);

      if (c == '\n') {
        // push back the insertion of line numbers until the next iteration.
        // this is because after printing a '\n', we may get an EOF char
        should_insert_line_nr = true;
      }
    }
  }

  fclose(f);
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stdout, "Usage: %s fil1 ...\n", argv[0]);
  } else {
    int line_nr = 1;

    for (int i = 1; i < argc; i++) {
      cat(argv[i], &line_nr);
    }
  }

  return 0;
}
