#include "utils.h"
#include <stdio.h>
#include <string.h>

void clear_input_buffer() {
  int c;

  do {
    c = getchar();
  } while (c != '\n' &&c != EOF);
}

int ask_question_int(char *question) {
  int result = 0;
  int conversions = 0;

  do {
    printf("%s\n", question);
    conversions = scanf("%d", &result);
    clear_input_buffer();
    putchar('\n');
  } while (conversions < 1);

  return result;
}

int read_string(char *buf, int buf_siz) {
  int count = 0;
  char c;

  for (int i = 0; i < buf_siz; i++) {
    c = getchar();

    if (i == buf_siz - 1) {
      buf[i] = '\0';
      clear_input_buffer();
      break;
    } else if (c == '\n' || c == EOF) {
      buf[i] = '\0';
      break;
    }

    buf[i] = c;
    count++;
  }

  return count;
}

char *ask_question_string(char *question, char *buf, int buf_siz) {
  int read = 0;

  do {
    printf("%s\n", question);
    read = read_string(buf, buf_siz);
  } while (read == 0);

  return strdup(buf);
}
