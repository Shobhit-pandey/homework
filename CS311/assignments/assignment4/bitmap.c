#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <pthread.h>

#include "bitmap.h"

//#define MAX (UINT_MAX)
//#define MAX 100

static long MAX;
static uint8_t *bitarray;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *sieve(void *k)
{
    int ret;

    for (long i = 2; i < ceil(sqrt(MAX)); ++i) {
        // Mark all multiples of the number
        if (!BITTEST(bitarray, i)) {
            ret = pthread_mutex_lock(&mutex);
            if (ret != 0) errExit("pthread_mutex_lock"); 

            for(long j = i + i; 0 < j && j < MAX; j += i) {
                //bitarray[BITSLOT(j)]
                BITSET(bitarray, j);
            }

            ret = pthread_mutex_unlock(&mutex);
            if (ret != 0) errExit("pthread_mutex_lock"); 
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    int ret;
    long i = 0;

    int NUM_THREADS = atoi(argv[1]);
    MAX = atoi(argv[2]);

    pthread_t *threads = (pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
    if (threads == 0) errExit("malloc threads");

    fprintf(stderr, "BITNSLOTS = %ld\n", (long) BITNSLOTS(MAX));

    bitarray = (uint8_t *) malloc( BITNSLOTS(MAX) * sizeof(uint8_t));
    if (bitarray == 0) errExit("malloc bitarray");

    memset(bitarray, 0, BITNSLOTS(MAX));

    for (i = 0; i < NUM_THREADS; ++i) {
        ret = pthread_create(&threads[i],
                             NULL,
                             sieve,
                             NULL);
        if (ret != 0) errExit("pthread_create");
    }

    for (int j = 0; j < NUM_THREADS; ++j) {
        ret = pthread_join(threads[j], NULL);
        if (ret != 0) errExit("pthread_join");
    }

    FILE *file = fopen("output.txt", "w");
    for (i = 2; i < MAX; ++i) {
        if(!BITTEST(bitarray, i)) {
            fprintf(file, "%ld\n", i);
        }
    }

    free(threads);
    free(bitarray);

    return 0;
}
