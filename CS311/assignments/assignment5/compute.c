#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <inttypes.h>

/* Profile determines the number of operations that can be performed in
 * a matter of 15 seconds */
int profile(void)
{
    time_t start;
    time_t end;
    time_t diff;

	int j = 12345;
    float max = 100000.0;
	volatile int k = 0;

    do {
        start = time(NULL);
        for (int i = 1; i < max; ++i) {
            k = j % i;
        }
        end = time(NULL);
        diff = (time_t) (end - start);
        if (diff <= 1) {
            max *= 10;
        } else if (diff < 15) {
            max *= (float)(15/(float)diff);
        } else if (diff == 16) {
            max -= 100000;
        } else if (diff > 15) {
            max /= 2.0;
        }
    } while ((end - start) != 15);

	return (int) max;
}


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

    int prof = profile();
    printf("%ju\n", (uintmax_t) prof);
}


