CC=gcc
CFLAGS=-ggdb -Wall -fprofile-arcs -ftest-coverage
CFLAGS_LIB=-c
COVERAGE_DIR=coverage

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
	./freq_count.out $(ARGS)

freq_count_mem: freq_count.out
	valgrind --leak-check=full ./freq_count.out $(ARGS)

freq_count_time: freq_count.out
	/usr/bin/time --verbose ./freq_count.out $(ARGS)

freq_count_profile: freq_count.out
	valgrind --tool=callgrind ./freq_count.out $(ARGS)

tests: hash_table_tests linked_list_tests

memtest: hash_table_mem linked_list_mem

# Could move this to a separate script
coverage: hash_table_tests.out linked_list_tests.out
	mkdir -p $(COVERAGE_DIR)
	./hash_table_tests.out
	./linked_list_tests.out
	gcov hash_table_tests.c
	gcov linked_list_tests.c
	mv -f *.gcov $(COVERAGE_DIR)
	mv -f *.gcda $(COVERAGE_DIR)
	mv -f *.gcno $(COVERAGE_DIR)
	lcov -c --directory $(COVERAGE_DIR) --output-file $(COVERAGE_DIR)/tests.info
	genhtml $(COVERAGE_DIR)/tests.info --output-directory $(COVERAGE_DIR)
	firefox $(COVERAGE_DIR)/index.html || open $(COVERAGE_DIR)/index.html

clean:
	rm -f *.out *.o *.gch callgrind.out.* *.gcno *.gcda *.gcov
	rm -rf coverage
