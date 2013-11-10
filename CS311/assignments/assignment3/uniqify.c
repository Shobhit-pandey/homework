/*
 * CS311: Project 3 - Uniqify
 *
 * Author: Trevor Bramwell
 */

#define _POSIX_SOURCE

#ifndef INPUT
#define INPUT 0
#endif

#ifndef OUTPUT
#define OUTPUT 1
#endif

#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "uniqify.h"

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

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
    long procs = 1;
    int opt;
    FILE *input = NULL;
    FILE *output = NULL;

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

    if (optind < argc) {
        input = fopen(argv[optind], "r");
        if (input == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
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
    pid_t cpid;
    /* Create 2N Pipes of 2 ints */
    int pipe_arr[2][procs][2];

    // init_pipes(pipe_arr, 2, procs);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < procs; ++j) {
            if (pipe(pipe_arr[i][j]) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (input == NULL) {
        input = stdin;
    }

    if (output == NULL) {
        output = stdout;
    }

    /* Fork n process */
    for(int i = 0; i < procs; ++i) {
        switch(cpid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            /* Close (2N)-4 Pipe Ends */
            // close_pipes(i, pipe_ar[INPUT]);
            // close_pipes(i, pipe_ar[OUTPUT]);
            /* Envoke sort for each child */
            printf("Hello, from Child %d\n", i);
            _exit(EXIT_SUCCESS);
            break;
        default:
            break; /* Continue to next child */
        }
    }

    /* Round robin distribute words
     *
     * For each word in input, parse, and write round robin to pipe.
     */

    /* Wait on N process
     *   or SIGCHLD all */
    for(int i = 0; i < procs; ++i) {
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }
}
