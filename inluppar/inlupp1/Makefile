hash_table_tests: hash_table_tests.c hash_table.c
	gcc -ggdb -Wall hash_table.c hash_table_tests.c -o hash_table_tests -lcunit 

tests: hash_table_tests
	./hash_table_tests
	
clean:
	rm hash_table_tests