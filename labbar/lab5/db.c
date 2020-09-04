#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
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

void edit_db(item_t *db, int db_siz) {
  int item_id;
  item_t updated_item;

  list_db(db, db_siz);
  item_id = ask_question_int_limit("Vilken vara vill du ändra?", 1, db_siz) - 1; // we want the index
  printf("\nNuvarande produktinfo:\n");
  print_item(&db[item_id]);
  puts("");
  updated_item = input_item();
  puts("");

  db[item_id] = updated_item;
}

void print_menu() {
  printf(
    "[L]ägg till en vara\n"
    "[T]a bort en vara\n"
    "[R]edigera en vara\n"
    "Ån[g]ra senaste raderingen\n"
    "Lista [h]ela varukatalogen\n"
    "[A]vsluta\n"
  );
}

bool is_action(char *str) {
  if (strlen(str) != 1) {
    return false;
  }

  return char_has_match(*str, "LlTtRrGgHhAa");
}

char ask_question_menu() {
  printf("\n");
  print_menu();
  printf("\n");
  return ask_question("Vad vill du göra?", is_action, (convert_func) to_upper_char).char_value;
}

void shift_db(item_t *db, int *db_siz, int shift_index) {
  while (shift_index < (*db_siz - 1)) {
    db[shift_index] = db[shift_index + 1];
    shift_index++;
  }

  *db_siz -= 1;
}

// assumes that db_size is less than 16 (as defined in main)
void add_item_to_db(item_t *db, int *db_siz) {
  db[*db_siz] = input_item();
  *db_siz += 1;
}

void remove_item_from_db(item_t *db, int *db_siz) {
  int item_id;

  list_db(db, *db_siz);
  item_id = ask_question_int_limit("Vilken vara vill du ta bort?", 1, *db_siz) - 1; // we want the index
  shift_db(db, db_siz, item_id);
}

void event_loop(item_t *db, int *db_siz) {
  char selection;
  bool exit = false;

  while (exit == false) {
    selection = ask_question_menu();
    printf("\n");
    switch (selection) {
      case 'L':
        add_item_to_db(db, db_siz);
        break;
      case 'T':
        remove_item_from_db(db, db_siz);
        break;
      case 'R':
        edit_db(db, *db_siz);
        break;
      case 'G':
        printf("Not yet implemented!\n");
        break;
      case 'H':
        list_db(db, *db_siz);
        break;
      case 'A':
        exit = true;
        break;
    }
  }
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

    printf("===== Databasmeny =====\n");
    event_loop(db, &db_siz);
  }

  return 0;
}
