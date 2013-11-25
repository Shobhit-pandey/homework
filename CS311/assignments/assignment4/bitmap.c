#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "bitmap.h"

//#define MAX (UINT_MAX)
#define MAX 100
#define NUM_THREADS 3

static uint8_t *bitarray;

void *sieve(void *k)
{
    uint32_t i = (uint32_t) k;
    uint32_t j;

    for(j = i + i; j < MAX; j += i) {
        BITSET(bitarray, j);
    }

    return 0;
}

int main()
{
    uint32_t i;
    pthread_t threads[NUM_THREADS];

    bitarray = (uint8_t *) malloc( BITNSLOTS(MAX) * sizeof(uint8_t));
    if (bitarray == 0) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memset(bitarray, 0, BITNSLOTS(MAX));

    for (int i = 0; i < NUM_THREADS; ++i) {
        if (!BITTEST(bitarray, (i+2))) {
            pthread_create(&threads[i],
                           NULL,
                           sieve,
                           (void *) (i+2));
        }
    }

    for (int j = 0; j < NUM_THREADS; ++j) {
        pthread_join(threads[j], NULL);
    }

    for (i = 2; i < MAX; ++i) {
        if(!BITTEST(bitarray, i)) {
            printf("%u\n", i);
        }
    }

    free(bitarray);

    return 0;
}
