// Following the SIMPLE methodology, we are going to dodge and simplify the specification.
// Our first hash table implementation will only support integer keys and string values,
// and only support a fixed number of buckets (17).

// 1. Each index in the array covers a dynamic number of keys7 instead of a single key; and */
// 2. Each value in the array is a sequence of (key,value) pairs so that each entry in the map is represented by a corresponding (key,value) pair (that we will call an entry). */

// ACHIEVEMENT: A3
typedef struct entry entry_t;

struct entry
{
  int key;       // holds the key
  char *value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  entry_t *buckets[17];
};
