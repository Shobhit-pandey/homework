#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <pthread.h>

#include "bitmap.h"

static long MAX;
static int NUM_THREADS;
static uint8_t *bitarray;
static long* working;
pthread_mutex_t *mutexes;

void *sieve(void *k)
{
    int ret = 0;
    int tid = (int)k;

    for (long i = 2; i < ceil(sqrt(MAX)); ++i) {
        // Mark all multiples of the number
        if (!BITTEST(bitarray, i)) {
            // Make sure the number we are using is not divisible by
            // any other number currently being processed.
            for (int v = 0; v < NUM_THREADS; ++v) {
                if((i % working[v]) == 0) continue;
            }

            working[tid] = i;

            for(long j = i + i; 0 < j && j < MAX; j += i) {
                ret = pthread_mutex_lock(&mutexes[BITSLOT(j)]);
                if (ret != 0) errExit("pthread_mutex_lock"); 

                BITSET(bitarray, j);

                ret = pthread_mutex_unlock(&mutexes[BITSLOT(j)]);
                if (ret != 0) errExit("pthread_mutex_lock"); 
            }
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    int ret;
    long i = 0;

    NUM_THREADS = atoi(argv[1]);
    MAX = atoi(argv[2]);

    pthread_t *threads = (pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
    if (threads == 0) errExit("malloc threads");

    working = (long *) malloc(NUM_THREADS * sizeof(long));
    if (working == 0) errExit("malloc working");

    for (int i = 0; i < NUM_THREADS; ++i) {
        working[i] = 1;
    }
    
    mutexes = (pthread_mutex_t *) malloc((BITNSLOTS(MAX)+1) * sizeof(pthread_mutex_t));
    if (mutexes == 0) errExit("malloc mutexes");

    fprintf(stderr, "BITNSLOTS = %ld\n", (long) BITNSLOTS(MAX));

    bitarray = (uint8_t *) malloc( BITNSLOTS(MAX) * sizeof(uint8_t));
    if (bitarray == 0) errExit("malloc bitarray");

    memset(bitarray, 0, BITNSLOTS(MAX));

    for (i = 0; i < BITNSLOTS(MAX); ++i) {
        ret = pthread_mutex_init(&mutexes[i], NULL);
        if (ret != 0) errExit("pthread_mutex_init");
    }

    for (i = 0; i < NUM_THREADS; ++i) {
        ret = pthread_create(&threads[i],
                             NULL,
                             sieve,
                             (void *)i);
        if (ret != 0) errExit("pthread_create");
    }

    for (int j = 0; j < NUM_THREADS; ++j) {
        ret = pthread_join(threads[j], NULL);
        if (ret != 0) errExit("pthread_join");
    }

    for (i = 0; i < BITNSLOTS(MAX); ++i) {
        ret = pthread_mutex_destroy(&mutexes[i]);
        if (ret != 0) errExit("pthread_mutex_destroy");
    }

    FILE *file = fopen("output.txt", "w");
    for (i = 2; i < MAX; ++i) {
        if(!BITTEST(bitarray, i)) {
            fprintf(file, "%ld\n", i);
        }
    }

    free(working);
    free(threads);
    free(mutexes);
    free(bitarray);

    return 0;
}
