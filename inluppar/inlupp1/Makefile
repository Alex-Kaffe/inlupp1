CC=gcc
CFLAGS=-ggdb -Wall
CFLAGS_LIB=-c

linked_list.o: linked_list.c
	gcc $(CFLAGS) $(CFLAGS_LIB) $^

hash_table.o: linked_list.o hash_table.c
	gcc $(CFLAGS) $(CFLAGS_LIB) $^

hash_table_tests.o: linked_list.o hash_table.o hash_table_tests.c
	gcc $(CFLAGS) $^ -o hash_table_tests.out -lcunit

linked_list_tests.o: linked_list.o linked_list_tests.c
	gcc $(CFLAGS) $^ -o linked_list_tests.out -lcunit

hash_table_tests: hash_table_tests.o
	./hash_table_tests.out

linked_list_tests: linked_list_tests.o
	./linked_list_tests.out

hash_table_mem: hash_table_tests.o
	valgrind --leak-check=full ./hash_table_tests.out

linked_list_mem: linked_list_tests.o
	valgrind --leak-check=full ./linked_list_tests.out

tests: hash_table_tests linked_list_tests

memtest: hash_table_mem linked_list_mem

clean:
	rm *.out
	rm *.o
	rm *.gch
