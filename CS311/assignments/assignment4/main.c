/*
 * CS311: Project 3 - Happy Primes
 *
 * Author: Trevor Bramwell
 */


#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"
#include "threaded.h"
#include "shared.h"

void errExit(const char *func) {
    perror(func);
    exit(EXIT_FAILURE);
}

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

const char *usage = "Usage: %s  [-s|-p] [-o output] [-n procs/threads] N\n\n"
                    "Find all primes up to N using threads, or "
                    "shared memory.\n\n"
                    "  -n\tThe number of processes or threads. [1]\n"
                    "  -o\tThe output file to store primes in. [stdout]\n"
                    "  -s\tUse shared memory and processesto find primes\n"
                    "  -p\tUse posix threads find primes\n";

/*
 * Main
 *
 */
int main(int argc, char *argv[])
{
    int procs = 1;
    int flags = 0;
    long max_prime = 10;
    FILE *output = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "n:o:hsp")) != -1) {
        switch (opt) {
        case 's':
            flags |= F_SHARED;
            break;
        case 'p':
            flags |= F_THREADED;
            break;
        case 'o':
            output = fopen(optarg, "w");
            break;
        case 'n':
            sscanf(optarg, "%d", &procs);
            break;
        case 'h':
            printf(usage, argv[0]);
            exit(EXIT_SUCCESS);
        default: /* '?' */
            fprintf(stderr, usage, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        sscanf(argv[optind], "%ld", &max_prime);
    }

    if ((flags & F_THREADED) && (flags & F_SHARED)) {
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);   
    }

    /* Threaded or Shared */
    if (flags & F_THREADED) {
        run_threaded();
    } else if (flags & F_SHARED) {
        run_shared();
    }

    exit(EXIT_SUCCESS);
}
