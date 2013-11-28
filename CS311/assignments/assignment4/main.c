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

const char *usage = "Usage: %s -s [-o output] [-n procs/threads] N\n"
                    "       %s -p [-o output] [-n procs/threads] N\n\n"
                    "Find all primes up to N using threads, or "
                    "shared memory.\n\n"
                    "  -n\tThe number of processes or threads. [1]\n"
                    "  -o\tThe output file to store primes in. [stdout]\n"
                    "  -s\tUse shared memory and processes to find primes\n"
                    "  -p\tUse posix threads and implicit sharing to "
                    "find primes\n";

/*
 * Main
 *
 */
int main(int argc, char *argv[])
{
    long procs = 1;
    int flags = 0;
    long max = 0;
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
            sscanf(optarg, "%ld", &procs);
            break;
        case 'h':
            printf(usage, argv[0], argv[0]);
            exit(EXIT_SUCCESS);
        default: /* '?' */
            fprintf(stderr, usage, argv[0], argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        sscanf(argv[optind], "%ld", &max);
    } else {
        fprintf(stderr, usage, argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((flags & F_THREADED) && (flags & F_SHARED)) {
        fprintf(stderr, usage, argv[0], argv[0]);
        exit(EXIT_FAILURE);   
    } else if ((flags & F_THREADED) == 0 && (flags & F_SHARED) == 0) {
        fprintf(stderr, "Either -p or -s required.\n");
        exit(EXIT_FAILURE);
    }

    /* Threaded or Shared */
    if (flags & F_THREADED) {
        threaded_main(max, procs, output);
    } else if (flags & F_SHARED) {
        shared_main(max, procs, output);
    }

    exit(EXIT_SUCCESS);
}
