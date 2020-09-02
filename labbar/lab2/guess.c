#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const int max_guesses = 15;
  const int secret_number = random() % 100;

  int buf_siz = 255;
  char buf[buf_siz];

  int guess = 0;
  char *name;

  name = ask_question_string("Skriv in ditt namn:", buf, buf_siz);
  printf("Du %s, jag tänker på ett tal... kan du gissa vilket?\n", name);

  for (int i = 0; i < max_guesses; i++) {
    guess = ask_question_int("");

    if (guess < secret_number) {
      puts("För litet!");
    } else if (guess > secret_number) {
      puts("För stort!");
    } else {
      puts("Bingo!");
      printf("Det tog %s %d gissningar att komma fram till %d\n", name, (i+1), secret_number);
      return 0;
    }
  }

  printf("Nu har du slut på gissningar! Jag tänkte på %d!\n", secret_number);

  return 0;
}
