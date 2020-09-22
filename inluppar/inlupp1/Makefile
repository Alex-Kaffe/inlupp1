CC=gcc
CFLAGS=-ggdb -Wall
CFLAGS_LIB=-c

#%.o: %.c %.h
#	$(CC) $(CFLAGS) $(CFLAGS_LIB) $< -o $@

#%_tests: %_tests.c %.o
#	$(CC) $(CFLAGS) $< -o $@.out $(CFLAGS_TEST)

hash_table.o: hash_table.c hash_table.h
	gcc $(CFLAGS) $(CFLAGS_LIB) hash_table.c -o $@

linked_list.o: linked_list.c list_linked.h iterator.h
	gcc $(CFLAGS) $(CFLAGS_LIB) linked_list.c -o $@
	
#iterator.o: list_iterator.c iterator.h
#	gcc $(CFLAGS) $(CFLAGS_LIB) list_iterator.c -o $@

hash_table_tests.o: hash_table.o hash_table_tests.c
	gcc $(CFLAGS) hash_table.o hash_table_tests.c -o hash_table_tests.out -lcunit

linked_list_tests.o: linked_list.o linked_list_tests.c
	gcc $(CFLAGS) linked_list.o linked_list_tests.c -o linked_list_tests.out -lcunit
	
#iterator_tests.o: iterator.o iterator_tests.c
#	gcc $(CFLAGS) iterator.o iterator_tests.c -o iterator_tests.out -lcunit

hash_table_tests: hash_table_tests.o
	./hash_table_tests.out

linked_list_tests: linked_list_tests.o
	./linked_list_tests.out
	
#iterator_tests: iterator_tests.o
#	./iterator_tests.out

hash_table_mem: hash_table_tests.o
	valgrind --leak-check=full ./hash_table_tests.out

linked_list_mem: linked_list_tests.o
	valgrind --leak-check=full ./linked_list_tests.out
	
#iterator_mem: iterator_tests.o
#	valgrind --leak-check=full ./iterator_tests.out

tests: hash_table_tests linked_list_tests

memtest: hash_table_mem linked_list_mem

clean:
	rm hash_table_tests.out
	rm linked_list_tests.out
	rm iterator.out
	rm hash_table.o
	rm linked_list.o
	rm iterator.o
