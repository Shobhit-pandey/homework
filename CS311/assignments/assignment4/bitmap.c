#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bitmap.h"

#define MAX (UINT_MAX-CHAR_BIT)

int main()
{

    long i, j;

    char *bitarray = (char *) malloc( BITNSLOTS(MAX) * sizeof(char));
    if (bitarray == 0) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memset(bitarray, 0, BITNSLOTS(MAX));

    for (i = 2; i < MAX; ++i) {
        if(!BITTEST(bitarray, i)) {
            for(j = i + i; j < MAX; j += i)
                BITSET(bitarray, j);
        }
    }

    /*
    for (i = 2; i < MAX; ++i) {
        if(!BITTEST(bitarray, i)) {
            printf("%ld\n", (long) i);
        }
    }
    */

    free(bitarray);

    return 0;
}
