# ifndef HashMap
# define HashMap

/*
 this is useful for you in answering the questions. if your write your code such
 that you call stringHash1 when HASHING_FUNCTION==1 and stringHash2 when
 HASHING_FUNCTION==2 then you only need to change this value in order to switch
 hashing functions that your code uses. you are not required to use this value
 though.
 */
# define HASHING_FUNCTION 1

# ifndef KeyType
# define KeyType char *
# endif

# ifndef ValueType
# define ValueType int
# endif

struct hashMap {
    struct List ** table;
    int tablesize;
    int count;
};

typedef struct hashMap hashMap;

/*the first hashing function you can use*/
int stringHash1(char * str);

/*the second hashing function you can use*/
int stringHash2(char * str);


/*initialize the supplied hashMap struct*/
void initHashMap (struct hashMap * ht, int tableSize);

/*
 free all memory used for your hashMap, but do not free the pointer to ht. you
 can see why this would not work by examining main(). the hashMap passed into
 your functions was never malloc'ed, so it can't be free()'ed either.
 */
void freeHashMap (struct hashMap * ht);

/*
 insert the following values into the hashTable.  Remember to first
 confirm that this key does not already exist in the table. you
 cannot have two Associations for the word "taco".
 
 if an element already exists in the table for the key provided you should
 replace that element (really this only requires replacing the value v).
 */
void addHashMap (struct hashMap * ht, KeyType k, ValueType v);

/*
 this returns a POINTER to the value stored in the hashTable specified by the key k.
 
 if the user supplies the key "taco" you should find taco in the hashTable, then
 return a pointer to the value member of the element that represents taco. Keep
 in mind we are storing an int for value, so you don't use malloc or anything.
 
 if the supplied key is not in the hashtable return NULL.
 */
ValueType* getHashMap (struct hashMap * ht, KeyType k);

/*
 a simple yes/no if the key is in the hashtable. 0 is no, all other values are
 yes.
 */
int containsKeyHashMap (struct hashMap * ht, KeyType k);

/*
 find the element for the supplied key and remove it from the collection.  It is not an error to be unable to find the element... if it
 cannot be found do nothing (or print a message) but do not use an assert which
 will end your program.
 */
void removeKeyHashMap (struct hashMap * ht, KeyType k);

/*
 returns the number of elements stored in the table
 */
int sizeHashMap (struct hashMap *ht);

/*
 returns the number of buckets in the table
 */
int capacityHashMap(struct hashMap *ht);

/*
 returns the number of empty buckets in the table, these are buckets which have
 no chains with items in them.
 */
int emptyBucketsHashMap(struct hashMap *ht);

/*
 returns the ratio of: (number of elements) / (number of buckets)
 
 this value can range anywhere from zero (an empty table) to more then 1, which
 would mean that there are more elements than buckets (but remember, elements
 are stored like linked list nodes so they can hang from each other)
 */
float tableLoadHashMap(struct hashMap *ht);

/*
 prints the table in the form of 'key: val'
 */
void printHashMap(struct hashMap *ht);
# endif
