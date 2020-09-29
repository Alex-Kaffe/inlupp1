CC=gcc
CFLAGS=-ggdb -Wall
CFLAGS_LIB=-c
FREQ_COUNT_ARGS=test.out

linked_list.o: linked_list.c common.h
	gcc $(CFLAGS) $(CFLAGS_LIB) linked_list.c
	
hash_table.o: linked_list.c hash_table.c common.h
	gcc $(CFLAGS) $(CFLAGS_LIB) linked_list.c hash_table.c
	
freq_count.out: linked_list.o hash_table.o freq_count.c common.h
	gcc $(CFLAGS) linked_list.o hash_table.o freq_count.c -o $@

hash_table_tests.out: linked_list.o hash_table.o hash_table_tests.c common.h
	gcc $(CFLAGS) linked_list.o hash_table.o hash_table_tests.c -o hash_table_tests.out -lcunit

linked_list_tests.out: linked_list.o linked_list_tests.c common.h
	gcc $(CFLAGS) linked_list.o linked_list_tests.c -o linked_list_tests.out -lcunit

hash_table_tests: hash_table_tests.out
	./hash_table_tests.out

linked_list_tests: linked_list_tests.out
	./linked_list_tests.out

hash_table_mem: hash_table_tests.out
	valgrind --leak-check=full ./hash_table_tests.out

linked_list_mem: linked_list_tests.out
	valgrind --leak-check=full ./linked_list_tests.out
	
freq_count: freq_count.out
	./freq_count.out $(FREQ_COUNT_ARGS)
	
freq_count_mem: freq_count.out
	valgrind --leak-check=full ./freq_count.out $(FREQ_COUNT_ARGS)

tests: hash_table_tests linked_list_tests

memtest: hash_table_mem linked_list_mem

clean:
	rm -f *.out *.o *.gch