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

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

const char *usage = "Usage: %s [-o output] [-n processes] file\n\n"
                    "Uniqify takes a input file, or stdin, and "
                    "sorts the words.\n\n"
                    "  -n\tThe number of sorting processes. [1]\n"
                    "  -o\tThe output file to store the sorted values "
                    "in. [stdout]\n";

/*
 * Main
 *
 */
int main(int argc, char *argv[])
{
    int procs = 1;
    int opt;

    while ((opt = getopt(argc, argv, "n:o:h")) != -1) {
        switch (opt) {
        case 'o':
            //output = fopen(optarg, "w");
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
        //input = fopen(argv[optind], "r");
        //if (input == NULL)
        errExit("fopen");
    }

    /* Threaded or Shared */
    run_threaded();
    run_shared();

    exit(EXIT_SUCCESS);
}
