# Initial Profiling Results
## small.txt
Top 3 functions: 
1. `_dl_start (ld-2.32.so)`
2. `_dl_sysdep_start (ld-2.32.so)`
2. `dl_main (ld-2.32.so)`

Top 3 functions from our code:
1. `ioopm_hash_table_create (hash_table.c)`
2. `process_file (freq_count.c)`
2. `ioopm_hash_table_lookup (hash_table.c)`

## 1k-long-word.txt
Top 3 functions: 
1. `process_file (freq_count.c)`
2. `string_hash (freq_count.c)`
3. `find_previous_entry_for_key (hash_table.c)`

## 10k-long-word.txt
Top 3 functions:
1. `find_previous_entry_for_key (hash_table.c)`
2. `string_hash (freq_count.c)`
3. `process_file (freq_count.c)`

## 16k-long-word.txt
Top 3 functions:
1. `find_previous_entry_for_key (hash_table.c)`
2. `string_hash (freq_count.c)`
3. `process_file (freq_count.c)`


As you can see, the top 3 functions are consistent across different inputs (except `small.txt` because of its extremely small size and fast execution time).
`find_previous_entry_for_key` is the number one function in inputs of a greater size, which is to be expected since most of the public functions will use
it internally during lookup, insert and remove.

### Is there some kind of trend?
As the list grows, finding the previous entry gets more and more expensive and this goes hand in hand with the hash function. 
Since the hash table stores the value, not the hash code, we must hash each key and compare it to the key that we are searching for.
`process_file` is the function that actually calls `ioopm_hash_table_insert` and `ioopm_hash_table_lookup`, so it is to be expected that it is one 
of the top 3 elements, considering the top 1 and 2.

### Do the results correspond with your expectations?
Yes.

### Based on these results, do you see a way to make your program faster?
Increasing the amount of buckets within the hashtable would allocate more memory, but the search within each bucket would be smaller, making these functions faster.
Since we're always looking up the hash code, saving the hash would cost more memory, but would increase in speed whilst searching for it.
