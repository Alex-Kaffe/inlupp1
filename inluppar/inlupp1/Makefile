hash_table_tests: hash_table_tests.c hash_table.o
	gcc -ggdb -Wall hash_table.o hash_table_tests.c -o hash_table_tests.out -lcunit

hash_table.o: hash_table.c hash_table.h
	gcc -ggdb -c -Wall hash_table.c -o hash_table.o

tests: hash_table_tests
	./hash_table_tests.out

memtest: hash_table_tests
	valgrind --leak-check=full ./hash_table_tests.out

clean:
	rm hash_table_tests.out
	rm hash_table.o
