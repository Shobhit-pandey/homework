/*
 * http://c-faq.com/misc/bitsets.html
 */

#ifndef _THREADED_H
#define _THREADED_H

#include <limits.h>     /* for CHAR_BIT */

#define BITMASK(b) (1 << ((b) % CHAR_BIT)) /* Index of b in byte */
#define BITSLOT(b) ((b) / CHAR_BIT) /* Index of byte containing b in bitmap */
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb) / CHAR_BIT)

int threaded_main(long max, long procs, FILE* output);
void *sieve(void *k);

#endif
