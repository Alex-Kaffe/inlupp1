#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const int max_guesses = 15;
  const int secret_number = random() % 100;

  int guess = 0;
  int guesses = 0;
  bool won = false;
  char *name;

  name = ask_question_string("Skriv in ditt namn:");
  printf("Du %s, jag tänker på ett tal... kan du gissa vilket?\n", name);

  for (guesses = 0; guesses < max_guesses; guesses++) {
    guess = ask_question_int("");

    if (guess < secret_number) {
      puts("För litet!");
    } else if (guess > secret_number) {
      puts("För stort!");
    } else {
      puts("Bingo!");
      won = true;
      break;
    }
  }

  if (won == true) {
    printf("Det tog %s %d gissningar att komma fram till %d\n", name, guesses, secret_number);
  } else {
    printf("Nu har du slut på gissningar! Jag tänkte på %d!\n", secret_number);
  }

  return 0;
}
