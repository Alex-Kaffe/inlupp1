#include <stdio.h>
#include "utils.h"

struct item {
  char *name;
  char *desc;
  int price;
  char *shelf;
};

typedef struct item item_t;

void print_item(item_t *item) {
  printf("Name:  %s\n", item->name);
  printf("Desc:  %s\n", item->desc);
  printf("Price: %d.%d\n", (item->price / 100), (item->price % 100));
  printf("Shelf: %s\n", item->shelf);
}

item_t make_item(char *name, char *desc, int price, char* shelf) {
  return (item_t) {
    .name = name,
    .desc = desc,
    .price = price,
    .shelf = shelf,
  };
}

item_t input_item() {
  char *name, *desc, *shelf;
  int price;

  puts("Create new item:");
  name = ask_question_string("Name:");
  desc = ask_question_string("Desc:");
  price = ask_question_int("Price:");
  shelf = ask_question_shelf("Shelf:");

  return make_item(name, desc, price, shelf);
}

char *magick(char *arr1[], char *arr2[], char *arr3[], int arr_length) {
  char buf[255];

  return strdup(buf);
}

int main(void) {
  char *array1[] = { "Laser",        "Polka",    "Extra" };
  char *array2[] = { "förnicklad",   "smakande", "ordinär" };
  char *array3[] = { "skruvdragare", "kola",     "uppgift" };

  item_t item = input_item();
  print_item(&item);
}
