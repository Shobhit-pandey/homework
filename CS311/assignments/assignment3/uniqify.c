/*
 * CS311: Project 3 - Uniqify
 *
 * Author: Trevor Bramwell
 */

#define _POSIX_SOURCE

#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "uniqify.h"

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

const char *usage = "Usage: %s [-i input] [-o output] [-n processes]\n"
                    "\t-n\tThe number of sorting processes. [1]\n"
                    "\t-i\tThe input file to sort. [stdin]\n"
                    "\t-o\tThe output file to store the sorted values "
                    "in. [stdout]\n";

/*
 * Main
 *
 */
int main(int argc, char *argv[])
{
    long procs = 1;
    int opt;
    FILE *input = stdin;
    FILE *output = stdout;

    while ((opt = getopt(argc, argv, "n:o:h")) != -1) {
        switch (opt) {
        case 'o':
            output = fopen(optarg, "w");
            break;
        case 'n':
            sscanf(optarg, "%ld", &procs);
            break;
        case 'h':
            printf(usage, argv[0]);
            exit(EXIT_SUCCESS);
        default: /* '?' */
            fprintf(stderr, usage, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        input = fopen(argv[optind], "r");
    }

    parser(input, output, procs);

    exit(EXIT_SUCCESS);
}

/*
 * Parser
 *
 */
void parser(FILE *input, FILE *output, long procs)
{

    printf("procs: %ld\n", procs);

    //fputc(fgetc(input),output);
    /*
    while((fchar = fgetc(input)) != EOF) {
        if(fchar == -1) {
            perror("fgetc");
            exit(EXIT_FAILURE);
        }

        if(fputc(fchar, output) != fchar) {
            perror("fputc");
            exit(EXIT_FAILURE);
        }
    }
    */
}
