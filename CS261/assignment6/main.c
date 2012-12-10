#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashMap.h"

/*
 the getWord function takes a FILE pointer and returns you a string which was
 the next word in the in the file. words are defined (by this function) to be
 characters or numbers seperated by periods, spaces, or newlines.
 
 when there are no more words in the input file this function will return NULL.
 
 this function will malloc some memory for the char* it returns. it is your job
 to free this memory when you no longer need it.
 */
char* getWord(FILE *file);

int main (int argc, const char * argv[]) {
    const char* filename;
	FILE *inputFile;
    struct hashMap hashTable;
    int tableSize = 100;
	clock_t timer;
    
    /*
     this part is using command line arguments, you can use them if you wish
     but it is not required. DO NOT remove this code though, we will use it for
     testing your program.
     
     if you wish not to use command line arguments manually type in your
     filename and path in the else case.
     */
    if(argc == 2)
        filename = argv[1];
    else
        filename = "input.txt"; /*specify your input text file here*/
    
	
    printf("opening file: %s\n", filename);
    
	/* open the input file */
	inputFile = fopen(filename, "r");
	if(inputFile != 0) {
		printf("Opened Successfully\n");
	} else {
	    printf("Failed to open\n");
	    /* No file. Exit code 1. */
	    return 1;
    }
	
	timer = clock();
	
    initHashMap(&hashTable, tableSize);
    
    char* word;
    int new;
    int* val;
    new = 1;
    
    while( !feof(inputFile) ) {
        word = getWord(inputFile);
        if(!word) {
            break;
        }
        val = getHashMap(&hashTable, word);
        if( val == NULL ) {
            addHashMap(&hashTable, word, new);
        } else {
            *val = *val + 1;
        }
    }
	
	timer = clock() - timer;
	printf("concordance ran in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
	/* Output table info */
	/*
	printf("----Table Info----\n");
	printf("Table emptyBuckets = %d\n",emptyBucketsHashMap(&hashTable));
    printf("Table size = %d\n",sizeHashMap(&hashTable));
    printf("Table capacity = %d\n",capacityHashMap(&hashTable));
    printf("Table load = %f\n",tableLoadHashMap(&hashTable));
    printf("Hashing Function: %d\n", HASHING_FUNCTION);
    printf("------------------\n");
    printf("Outputing file...\n");
    */
	printHashMap(&hashTable);
    
	/* Close the file */
	fclose(inputFile);
	freeHashMap(&hashTable);
	
    return 0;
}

char* getWord(FILE *file)
{
	int length = 0;
	int maxLength = 16;
	char character;
    
	char* word = (char*)malloc(sizeof(char) * maxLength);
	assert(word != NULL);
    
	while( (character = fgetc(file)) != EOF)
	{
		if((length+1) > maxLength)
		{
			maxLength *= 2;
			word = (char*)realloc(word, maxLength);
		}
		if((character >= '0' && character <= '9') || /*is a number*/
		   (character >= 'A' && character <= 'Z') || /*or an uppercase letter*/
		   (character >= 'a' && character <= 'z') || /*or a lowercase letter*/
		   character == 39) /*or is an apostrophy*/
		{
			word[length] = character;
			length++;
		}
		else if(length > 0)
			break;
	}
    
	if(length == 0)
	{
		free(word);
		return NULL;
	}
	word[length] = '\0';
	return word;
}
