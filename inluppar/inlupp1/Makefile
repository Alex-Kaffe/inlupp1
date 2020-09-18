CC=gcc
CFLAGS=-ggdb -Wall
CFLAGS_LIB=-c

#%.o: %.c %.h
#	$(CC) $(CFLAGS) $(CFLAGS_LIB) $< -o $@

#%_tests: %_tests.c %.o
#	$(CC) $(CFLAGS) $< -o $@.out $(CFLAGS_TEST)

hash_table.o: hash_table.c hash_table.h
	gcc $(CFLAGS) $(CFLAGS_LIB) hash_table.c -o hash_table.o

linked_list.o: linked_list.c list_linked.h
	gcc $(CFLAGS) $(CFLAGS_LIB) linked_list.c -o linked_list.o

hash_table_tests: hash_table.o hash_table_tests.c 
	gcc $(CFLAGS) hash_table.o hash_table_tests.c -o hash_table_tests.out -lcunit
	
linked_list_tests: linked_list.o linked_list_tests.c
	gcc $(CFLAGS) linked_list.o linked_list_tests.c -o linked_list_tests.out -lcunit

tests: hash_table_tests linked_list_tests
	./hash_table_tests.out
	./linked_list_tests.out

memtest: hash_table_tests linked_list_tests
	valgrind --leak-check=full ./hash_table_tests.out ./linked_list_tests.out

clean:
	rm hash_table_tests.out
	rm linked_list_tests.out
	rm hash_table.o
	rm linked_list.o
