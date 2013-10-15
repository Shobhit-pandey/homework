/*
Title: Sieve of Eratosthenes
Author: Trevor Bramwell
Date: Sun Oct 13 20:11:29 PDT 2013
---

# Write a prime number sieve.

The Sieve of Eratosthenes identifies all prime numbers up to a given number n
as follows:

    # Input: integer
    # Will create an array of size n, where n[0] represents the primality of 1,
    # n[1] represents the primality of 2, and so on.
    # In each case:
    #   0 represents prime
    #   1 represents composite
    #

    Write down the numbers 1, 2, 3, ..., n. We will eliminate composites by
    marking them. Initially all numbers are unmarked.  Mark the number 1 as
    special (it is neither prime nor composite).

    # Loop through the array from 0 to >= sqrt(i)

    Set k=1. Until k exceeds or equals the square root of n do:

        # If n[i] == 0
        # (counter++) - we want a count at the end as well

        Find the first number in the list greater than k that has not been
        identified as composite. (The very first number so found is 2.)
        Call it m. Mark the numbers

        # set n[i*j] to 1, where j increases while i*j < n

        2m, 3m, 4m, ...

        as composite. (Thus in the first run we mark all even numbers greater
        than 2. In the second run we mark all multiples of 3 greater than 3.)
        m is a prime number. Put it on your list.

        # Append m to list

        Set k=m and repeat.

    # i++ until all n[i] == 0 added

    Put the remaining unmarked numbers in the sequence on your list of prime
    numbers.

# Return (counter)

Write a C function which returns the number of primes less than a given value,
as well as the values of all the primes. It might be worth writing this in a
way that can be easily used later...

*/
#include <stdio.h>

/* Tests
 
   Given the size of the array passed in, and a wrongly sized array, the
   function returns an error code.

   Given a size of 1, the function returns int(0).

   Given a size of 2, the functions return int(1), and the array contains 2.

*/

int main(int argc, char* argv[]) {

    printf("hello, world!\n");

    return 0;
}
