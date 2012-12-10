/**
 Trevor Bramwell - bramwelt@onid.orst.edu
 CS261 - Datastructures - Assignment 6
 Professor Ron Metoyer
 12/05/2010
*/

#include <stdlib.h>
#include <stdio.h>
#include "hashMap.h"
#include "assert.h"
#include "list.h"

int _hash(char * str, int tablesize);

/*
 Resize the table to a new size, and copy all the data over
 */
void _resizeHashMap(struct hashMap* ht);

int stringHash1(char * str)
{
    int i;
    int r = 0;
    for (i = 0; str[i] != '\0'; i++)
        r += str[i];
    return r;
}

int stringHash2(char * str)
{
    int i;
    int r = 0;
    for (i = 0; str[i] != '\0'; i++)
        r += (i+1) * str[i]; /*the difference between 1 and 2 is on this line*/
    return r;
}

/* Hash meta function to select the correct hash function to use. */
int _hash(char * str, int tablesize) {
    int idx;
    if( HASHING_FUNCTION == 1 ) {
        idx = (int)(stringHash1(str) % tablesize);
    } else if ( HASHING_FUNCTION == 2 ) {
        idx = (int)(stringHash2(str) % tablesize);
    }
    /* Unshure of modulus operator used in hash function
       correct for possible negative */
    if( idx < 0 ) idx += tablesize;
    return idx;
}

void initHashMap (struct hashMap * ht, int tableSize)
{
    int i;
    assert( ht != 0);
    assert( tableSize > 0);
    /* Create an array of pointers to Lists */
    ht->table = (List**) malloc(tableSize * sizeof(List*));
    assert( ht->table != 0 );
    /* Initialize each pointer to NULL */
    for( i = 0; i < tableSize; i++ ) {
        ht->table[i] = 0;
    }
    /* Set tablesize and init count to 0 */
    ht->tablesize = tableSize;
    ht->count = 0;
}

void freeHashMap (struct hashMap * ht)
{
    int i;
    assert( ht != 0);
    /* If a list was created, free it using freeList */
    for( i = 0; i < ht->tablesize; i++) {
        if( ht->table[i] != 0 ) {
            freeList(ht->table[i]);
        }
    }
    /* Set tablesize and count to 0 */
    ht->tablesize = 0;
    ht->count = 0;
}

void addHashMap (struct hashMap * ht, KeyType k, ValueType v)
{
    int idx;
    float load;
    assert(ht != 0);
    /* Compute hash */
    idx = _hash(k, ht->tablesize);
    /* Create new list if not already created */
    if( ht->table[idx] == 0 ) {
        ht->table[idx] = newList();
        assert( ht->table[idx] != 0 );
    }
    /* Place value in array. */
    addList(ht->table[idx], k, v);
    /* Increment data count */
    ht->count++;
    /* Reorganize if load factor too great */
    load = tableLoadHashMap(ht);
    if( load > 8.0) {
       _resizeHashMap(ht);
    }
}

ValueType* getHashMap (struct hashMap * ht, KeyType k)
{
    int idx;
    assert( ht != 0 );
    idx = _hash(k, ht->tablesize);
    if( ht->table[idx] == 0 ) {
        return NULL;
    } else {
        if( containsList(ht->table[idx], k) ) {
            return getList(ht->table[idx], k);
        } else {
            return NULL;
        }
    }
}

int containsKeyHashMap (struct hashMap * ht, KeyType k)
{
    int idx;
    assert( ht != 0 );
    idx = _hash(k, ht->tablesize);
    if( ht->table[idx] != 0 ) {
        return containsList(ht->table[idx], k);
    }
    return 0;
}

void removeKeyHashMap (struct hashMap * ht, KeyType k)
{
    int idx;
    assert(ht != 0);
    if( containsKeyHashMap(ht, k) ) {
        idx = _hash(k, ht->tablesize);
        if( ht->table[idx] != 0 ) {
            removeList(ht->table[idx], k);
            ht->count--;
        }
    }
}

int sizeHashMap (struct hashMap *ht)
{
    assert(ht != 0);
    return ht->count;
}

int capacityHashMap(struct hashMap *ht)
{
    assert(ht != 0);
    return ht->tablesize;
}

int emptyBucketsHashMap(struct hashMap *ht)
{
    int i;
    int count;
    assert(ht != 0);
    count = 0;
    for( i = 0; i < ht->tablesize; i++ ) {
        if( ht->table[i] == 0 ) count++;
    }
    
    return count;
}

float tableLoadHashMap(struct hashMap *ht)
{
    assert(ht != 0);
    return (ht->count / (double) ht->tablesize);
}

void _resizeHashMap(struct hashMap *ht) {
    struct hashMap hashTable;
    struct List* tmp;
    struct DLink* l;
    int i;
    
    assert( ht != 0 );
    initHashMap(&hashTable, ht->tablesize*2);
    /* Add to hashTable, free old hash map */
    for( i = 0; i < ht->tablesize; i++) {
        tmp = ht->table[i];
        if( tmp != 0 ) {
            l = ht->table[i]->head->next;
            while( l != ht->table[i]->tail ) {
                addHashMap(&hashTable, l->key, l->val);
                l = l->next;
            }
        }
    }
    freeHashMap(ht);
    *ht = hashTable;
}

void printHashMap(struct hashMap *ht) {
    List* tmp;
    int i;
    assert(ht != 0);
    for( i = 0; i < ht->tablesize; i++ ) {
        tmp = ht->table[i];
        if( tmp != 0 ) {
            printList(ht->table[i]);
        }
    }
}
