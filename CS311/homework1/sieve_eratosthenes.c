/*
Title: Sieve of Eratosthenes
Author: Trevor Bramwell
Date: Sun Oct 13 20:11:29 PDT 2013
*/

/*
Directions:

Write a C function which returns the number of primes less than a given value,
as well as the values of all the primes. It might be worth writing this in a
way that can be easily used later...
*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 20

#define DONE -1

#define PRIME 0
#define UNMARKED 0
#define SPECIAL 1
#define COMPOSITE 1

int
num_zeros(int* array, const int size) {
    /* Return the number of PRIMES in an array */
    int counter = 0;
    for(int i = 0; i < size; i++) {
        if(array[i] == PRIME) {
            ++counter;
        }
    }
    return counter;
}

void
mark_numbers(int* primes, const int m, const int n) {
    /*
    Mark the numbers

    2m, 3m, 4m, ...

    as composite. (Thus in the first run we mark all even numbers greater
    than 2. In the second run we mark all multiples of 3 greater than 3.)
    */
    int i = 1;
    while(i*m < n) {
       primes[i*m] = COMPOSITE; 
       ++i;
    }
}

int
get_larger_prime(int* primes, const int k, const int n) {
    /*
    Find the first number in the list greater than k that has not been
    identified as composite. (The very first number so found is 2.)
    Call it m.
    */
    int m = k;
    while(m < n) {
        ++m;
        //printf("m:%ld n:%ld\n", m, n);
        if(primes[m] == PRIME) {
            return m;
        }
    }
    return DONE;
}

int
prime_sieve(int* primes, const int n)
{
    assert(primes != 0);
    assert(n > 0);
    int m = 1;
    int counter = 0;
    /*
    Write down the numbers 1, 2, 3, ..., n. We will eliminate composites by
    marking them. Initially all numbers are unmarked.  Mark the number 1 as
    special (it is neither prime nor composite).
    */

    primes[0] = SPECIAL;
    //primes[1] = SPECIAL;

    /* Set k=1. */ 
    int k = 1;

    /*
    Until k exceeds or equals the square root of n do:
    */
    while(k < ceil(sqrt(n))) {
        //++k;
        /*
        Find the first number in the list greater than k that has not been
        identified as composite. (The very first number so found is 2.)
        Call it m.
        */
        m = get_larger_prime(primes, k, n);
        ++counter;
        /*
        Mark the numbers

        2m, 3m, 4m, ...

        as composite. (Thus in the first run we mark all even numbers greater
        than 2. In the second run we mark all multiples of 3 greater than 3.)
        */
        mark_numbers(primes, m, n);

        /*
        m is a prime number. Put it on your list.
        */
        primes[m] = PRIME;

        /*
        Set k=m and repeat.
        */
        k = m;
    }

    /*
    Put the remaining unmarked numbers in the sequence on your list of prime
    numbers.
    */
    --counter;
    while(k < n) {
        if (primes[k] == PRIME) {
            ++counter;
        }
        ++k;
    }

    return counter;
}


void
print_array(int* array, const int size) {
    /* Print an array of size to stdout. */
    printf("[");
    for(int i = 0; i < size; i++) {
        if (i != (size-1))
        {
            printf("%ld, ", (long)array[i]);
        }
        else
        {
            printf("%ld", (long)array[i]);
        }
    }
    printf("]\n");
}

void
print_primes(int* array, const int size, const int num) {
    /* Print the prime numbers in array */
    assert(size >= 2);
    int j = 0;
    printf("[");
    for(int i = 2; i < size; i++) {
        if (array[i] == PRIME) {
            ++j;
            if(j != num) {
                printf("%ld, ", (long)i);
            } else {
                printf("%ld", (long)i);
            }
        }
    }
    printf("]\n");
}

void
init_array(int* array, const int size) {
    /* Initialize an integer array of size to UNMARKED. */
    for(int i = 0; i < size; i++) {
        array[i] = UNMARKED;
    }
}

int
main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Please provide a number as a single argument.\n");
        return 1;
    }

    int n = atoi(argv[1]);
    //printf("%ld\n", atoi(argv[1]));

    int* primes = (int *)malloc( sizeof(int)*n );
    assert(primes != 0);

    init_array(primes, n);

    int num_prime = prime_sieve(primes, n);

    printf("%ld Primes less than %ld.\n", (long)num_prime, (long)n);
    print_primes(primes, n, num_prime);

    free(primes);
    return 0;
}
