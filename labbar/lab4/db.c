#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

struct item {
  char *name;
  char *desc;
  int price;
  char *shelf;
};

typedef struct item item_t;

void print_item(item_t *item) {
  printf("Namn:       %s\n", item->name);
  printf("Beskriving: %s\n", item->desc);
  printf("Pris:       %d.%d\n", (item->price / 100), (item->price % 100));
  printf("Hylla:      %s\n", item->shelf);
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

  puts("Skapa ny vara:");
  name = ask_question_string("Namn:");
  desc = ask_question_string("Beskriving:");
  price = ask_question_int("Pris:");
  shelf = ask_question_shelf("Hylla:");

  return make_item(name, desc, price, shelf);
}

// does not automatically insert termination char unless the buffer is full,
// make sure to set the delimiter to '\0' for the last word.
void insert_magick_string(char *buf, int buf_siz, char *arr[], int arr_length, int *cursor, char delimiter) {
  int index = rand() % arr_length;
  char *str = arr[index];

  // should figure out some way of removing the three separate *cursor += 1 calls
  do {
    if (*cursor == (buf_siz - 1)) {
      // if the buffer is full, we must insert the termination char
      buf[*cursor] = '\0';
      *cursor += 1;
    } else if (*str == '\0') {
      // as long as the buffer is not full, we can insert any delimiter
      buf[*cursor] = delimiter;
      *cursor += 1;
      break;
    } else {
      buf[*cursor] = *str;
      *cursor += 1;
      str++;
    }
  } while (*cursor < buf_siz);
}

char *magick(char *arr1[], char *arr2[], char *arr3[], int arr_length) {
  int buf_siz = 255;
  char buf[buf_siz];
  int cursor = 0;

  insert_magick_string(buf, buf_siz, arr1, arr_length, &cursor, '-');
  insert_magick_string(buf, buf_siz, arr2, arr_length, &cursor, ' ');
  insert_magick_string(buf, buf_siz, arr3, arr_length, &cursor, '\0');

  return strdup(buf);
}

void list_db(item_t *items, int no_items) {
  for (int i = 0; i < no_items; i++) {
    // align the printed item names
    char *spacing = "   ";
    if (i >= 9) {
      spacing = "  ";
    } else if (i >= 99) {
      spacing = " ";
    }

    printf("%d.%s%s\n", (i+1), spacing, items[i].name);
  }
}

void edit_db(item_t *items, int no_items) {
  int item_id;
  item_t updated_item;

  puts("");
  item_id = ask_question_int_limit("Vilken vara vill du ändra?", 1, no_items) - 1; // we want the index
  printf("\nNuvarande produktinfo:\n");
  print_item(&items[item_id]);
  puts("");
  updated_item = input_item();
  puts("");

  items[item_id] = updated_item;
}

int main(int argc, char *argv[])
{
  srandom(time(NULL)); // initialize random generator

  char *array1[] = { "Laser",        "Polka",    "Extra" };
  char *array2[] = { "förnicklad",   "smakande", "ordinär" };
  char *array3[] = { "skruvdragare", "kola",     "uppgift" };

  if (argc < 2) {
    printf("Usage: %s number\n", argv[0]);
  } else {
    item_t db[16]; // Array med plats för 16 varor
    int db_siz = 0; // Antalet varor i arrayen just nu

    int items = atoi(argv[1]); // Antalet varor som skall skapas

    if (items > 0 && items <= 16) {
      for (int i = 0; i < items; ++i) {
        // Läs in en vara, lägg till den i arrayen, öka storleksräknaren
        item_t item = input_item();
        db[db_siz] = item;
        ++db_siz;
      }
    } else {
      /* puts("Sorry, must have [1-16] items in database."); */
      /* return 1; */
    }

    for (int i = db_siz; i < 16; ++i) {
      char *name = magick(array1, array2, array3, 3);
      char *desc = magick(array1, array2, array3, 3);
      int price = random() % 200000;
      char shelf[] = {
        random() % ('Z'-'A') + 'A',
        random() % 10 + '0',
        random() % 10 + '0',
        '\0'
      };

      item_t item = make_item(name, desc, price, shelf);

      db[db_siz] = item;
      ++db_siz;
    }

    list_db(db, db_siz);
    edit_db(db, db_siz);
    list_db(db, db_siz);
  }

  return 0;
}
