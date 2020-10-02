# Build and run
```bash
make tests # compile and run all tests
make hash_table_tests # compile and run hash table tests only
make linked_list_tests # compile and run linked list/iterator tests only

make memtest # run all tests through valgrind for memory management information
make hash_table_mem # run hash table tests only through valgrind
make linked_list_mem # run linked list/iterator tests only through valgrind

make clean # removes all generated and compiled files
```

# Test coverage
The current tests has a 100% function coverage and a 98.9% line coverage, though the only untested lines are the error handling for non-passing tests in CUnit.
In other words, the tests currently covers all lines and branches of our code. 

Coverage is done by `gcov` and the source files are compiled with the `-fprofile-arcs -ftest-coverage` flags. 
The coverage data is then parsed through `lcov` and passed to `genhtml` for a HTML representation of the data.

All this can be ran automatically using `make coverage`.

![Test coverage](media/coverage.png)


# Documentation
## Includes and setup
Documentation of the functions are included in the header files.
The code is separated into four header files, `common.h`, `hash_table.h`, `linked_list.h`, and `iterator.h`.
Both the hash table, and the linked list needs `common.h` to function properly.
If you only want to make use of the linked list, you will need to import all the header files except for hash table.
The iterator is a part of the linked list code, as it's an iterator over the linked list.
If you only want the hashtable, you only need `common.h` and `hash_table.h`, however the functions ioopm hash table keys and
ioopm hash table values returns a linked list, so you need to import `linked_list.h` aswell if you want to make use of these functions.

`common.h` also contains a few utility functions that you can use, e.g. a string hashing function, `elem_t` creation macros, as well as macros
for getting the success/failure state of a function.

## Generic data and memory management
The hash table supports generic types for both keys and values and in most cases it frees the memory for you. However, his is **not** the case, if you insert
pointers. If a pointer is inserted into the hash table, the user is responsible for deallocating it before the termination of the program.

## Error handling
Failures are handled througout the program with errno, an integer variable imported from `errno.h`. The user can check if a function returned an error by
using the `HAS_ERROR()` macro, defined in `common.h`. Note that `errno` only gets set by function-calls that has a failure state. 
A function that has a failure state **always** resets the value of `errno` on success.
If a function fails and `HAS_ERROR()` returns true, you should not read or use the return value of the function.

# Initial Profiling Results
## small.txt
Top 3 functions: 
1. `_dl_start (ld-2.32.so)`
2. `_dl_sysdep_start (ld-2.32.so)`
2. `dl_main (ld-2.32.so)`

Top 3 functions from our code:
1. `ioopm_hash_table_create (hash_table.c)`
2. `ioopm_hash_table_lookup (hash_table.c)`
2. `ioopm_hash_table_insert (hash_table.c)`

## 1k-long-word.txt
Top 3 functions: 
1. `process_file (freq_count.c)`
2. `string_hash (freq_count.c)`
3. `process_word (freq_count.c)`

Top 3 functions for our code:
1. `string_hash (freq_count.c)`
2. `ioopm_hash_table_lookup (hash_table.c)`
3. `ioopm_hash_table_insert (hash_table.c)`

## 10k-long-word.txt
Top 3 functions:
1. `process_file (freq_count.c)`
2. `find_previous_entry_for_key (hash_table.c)`
3. `process_word (freq_count.c)`

Top 3 functions for our code:
1. `find_previous_entry_for_key (hash_table.c)`
2. `ioopm_hash_table_lookup (hash_table.c)`
3. `ioopm_hash_table_insert (hash_table.c)`

## 16k-long-word.txt
Top 3 functions:
1. `find_previous_entry_for_key (hash_table.c)`
2. `process_file (freq_count.c)`
3. `eq_elem_string (common.c)`

Top 3 functions for our code:
1. `find_previous_entry_for_key (hash_table.c)`
2. `eq_elem_string (common.c)`
3. `ioopm_hash_table_lookup (hash_table.c)`


As you can see, the top 3 functions are quite consistent across different inputs (except for `small.txt` because of its extremely small size and fast execution time).
`find_previous_entry_for_key` is one of the top functions in inputs of a greater size, which is to be expected since most of the public functions will use
it internally.

### Is there some kind of trend?
As the list grows, finding the previous entry gets more and more expensive and this goes hand in hand with `eq_elem_string` function. 
When finding a previous entry, we must compare the key of each entry that is located before the entry that we are looking for.
Since the hash table stores the value, not the hash code, we must hash each key and compare it to the key that we are searching for.
`process_file` is the function that actually calls `ioopm_hash_table_insert` and `ioopm_hash_table_lookup`, so it is to be expected that it is one 
of the top 3 functions, considering the other top functions.

### Do the results correspond with your expectations?
Yes.

### Based on these results, do you see a way to make your program faster?
Increasing the amount of buckets within the hashtable would allocate more memory, but inserting/looking up would have a faster execution time.