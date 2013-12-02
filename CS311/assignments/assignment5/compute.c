#include <stdio.h>
#include <limits.h>

/* Computer the perfectality of a number.
 *
 * Sum all factors up to 1/2 of the number. If equal to the number, it
 * is perfect, else not, */
int is_perfect(long number)
{
    long perfect = 1;
    long max = (number/2);

    for(long i = 2; i <= max; ++i) {
        if ((number % i) == 0)
            perfect += i;
    }

    if (number == perfect)
        return 1;

    return 0;
}

/* Compute the prefect numbers up to n. Brute Force. */
int compute_perfect_numbers(const long max, int n[], const int n_size)
{
    int idx = 0;
    long i = 0;

    while(i < max && idx < n_size) {
        if(is_perfect(i)) {
            n[idx++] = i;
        }
        ++i;
    }

    return idx;
}

int main(int argc, char *argv[])
{
    const int size = 4;
    const long max = 9000;
    int perfect_numbers[size];

    int nums = compute_perfect_numbers(max, perfect_numbers, size);
        
    for (int i = 0; i < nums; ++i) {
        printf("%d\n", perfect_numbers[i]);
    }
}


