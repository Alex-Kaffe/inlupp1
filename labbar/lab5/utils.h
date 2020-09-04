#pragma once
#include <stdbool.h>

extern char *strdup(const char *);

typedef union {
  int   int_value;
  float float_value;
  char *string_value;
  char char_value;
} answer_t;

typedef bool(*check_func)(char*);
typedef answer_t(*convert_func)(char*);

bool is_number(char *str);
bool not_empty(char *str);
bool is_shelf(char *str);
char to_upper_char(char *str);
void clear_input_buffer();
int read_string(char *buf, int buf_siz);
answer_t ask_question(char *question, check_func check, convert_func convert);
char *ask_question_string(char *question);
char *ask_question_shelf(char *question);
int ask_question_int(char *question);
int ask_question_int_limit(char *question, int min, int max);
char *trim(char *str);
bool char_has_match(char action, char *match_string);
