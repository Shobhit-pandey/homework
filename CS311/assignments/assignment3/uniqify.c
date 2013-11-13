/*
 * CS311: Project 3 - Uniqify
 *
 * Author: Trevor Bramwell
 */

#define _POSIX_SOURCE

#ifndef READ
#define READ 0
#endif

#ifndef WRITE
#define WRITE 1
#endif

#ifndef IN_OUT
#define IN_OUT 2
#endif

#include <assert.h>
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
        if (input == NULL)
            errExit("fopen");
    }

    parser(input, output, procs);

    exit(EXIT_SUCCESS);
}

/*
 * Allocate space for 4N pipes
 */
void init_pipes(int ***arr, int procs) {
    arr = (int ***)malloc(IN_OUT * sizeof(int**));
    assert(arr != 0);
    for (int i = 0; i < IN_OUT; ++i) {
        arr[i] = (int **)malloc(procs * sizeof(int*));
        assert(arr[i] != 0);
        for (int j = 0; j < procs; ++j) {
            arr[i][j] = (int *)malloc(IN_OUT * sizeof(int));
            assert(arr[i][j] != 0);
            if (pipe(arr[i][j]) == -1)
                errExit("pipe");
        }
    }
}

/*
 * Free up the pipes that were created.
 */
void free_pipes(int ***arr, int procs) {
    for (int i = 0; i < IN_OUT; ++i) {
        for (int j = 0; j < procs; ++j) {
            free(arr[i][j]);
        }
        free(arr[i]);
    }
    free(arr);
}

/*
 * Close all open pipes in the array, except the onces a child needs
 */
void close_pipes(int proc, int ***arr, int procs, int dir) {
    for (int i = 0; i < IN_OUT; ++i) {
        for (int j = 0; j < procs; ++j) {
            if (j != proc) {
                close(arr[i][j][WRITE]);
                close(arr[i][j][READ]);
            } else if (dir == WRITE) {
                close(arr[i][j][WRITE]);
            } else if (dir == READ) {
                close(arr[i][j][READ]);
            }
        }
    }
}

/*
 * Parser
 */
void parser(FILE *input, FILE *output, long procs)
{
    pid_t cpid;
    int pipe_in[2];
    int pipe_out[2];

    char buf[512];
    FILE *write_stream;
    FILE *read_stream;

    /* Initialize Pipes */
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) errExit("pipe");

    /*
    if (input == NULL) {
        input = stdin;
    }

    if (output == NULL) {
        output = stdout;
    }
    */

    /* Don't worry about procs for time being */
    procs = 1;
    
    /*
     * Parser
     */
    switch(cpid = fork()) {
    case -1: errExit("fork");
    case 0:
        if (close(pipe_in[READ]) == -1) errExit("close");
        if (close(pipe_out[WRITE]) == -1) errExit("close");
        if (close(pipe_out[READ]) == -1) errExit("close");

        if ((write_stream = fdopen(pipe_in[WRITE], "a")) == NULL) errExit("fdopen");

        while ((fscanf(stdin, "%s", buf) != EOF)) {
            if(fputs(buf, write_stream) == EOF) errExit("fputs");
            if(fputs("\n", write_stream) == EOF) errExit("fputs");
        }

        if (fclose(write_stream) == EOF) errExit("fclose");
        
        /* First pipes are now all closed. sort should have recieved it's
         * final input */
        _exit(EXIT_SUCCESS);
    default:
        break;
    }

    /*
     * Sorter
     */
    switch(cpid = fork()) {
    case -1: errExit("fork");
    case 0:
        close(pipe_in[WRITE]);
        close(pipe_out[READ]);

        /* Reassign sort input FD */
        if (pipe_in[READ] != STDIN_FILENO) {
            if (dup2(pipe_in[READ], STDIN_FILENO) == -1) errExit("dup2");
            if (close(pipe_in[READ]) == -1) errExit("close");
        }

        /* Reassign sort output FD */
        if (pipe_out[WRITE] != STDOUT_FILENO) {
            if (dup2(pipe_out[WRITE], STDOUT_FILENO) == -1) errExit("dup2");
            if (close(pipe_out[WRITE]) == -1) errExit("close");
        }

        /* Envoke sort for each child */
        if ((execlp("sort", "sort", (char *) NULL)) == -1) errExit("execve");

        _exit(EXIT_SUCCESS);
    default:
        break;
    }


    /*
     * Suppressor
     */
    switch(cpid = fork()) {
    case -1: errExit("fork");
    case 0:
        /*
         * Round robin distribute words
         *
         * For each word in input, parse, and write round robin to pipe.
         */
        if (close(pipe_out[WRITE]) == -1) errExit("close");
        if (close(pipe_in[WRITE]) == -1) errExit("close");
        if (close(pipe_in[READ]) == -1) errExit("close");

        /* Read from pipe from sort, to output:(stdout|FILE) */
        if ((read_stream = fdopen(pipe_out[READ], "r")) == NULL) errExit("fdopen");
        while ((fgets(buf, 512, read_stream) != NULL)) {
            if(fprintf(stdout, "%s", buf) == EOF) errExit("fputs");
        }
        if (fclose(read_stream) == EOF) errExit("fclose");

        _exit(EXIT_SUCCESS);
    default:
        break;
    }
    
    if (close(pipe_in[WRITE]) == -1) errExit("close");
    if (close(pipe_in[READ]) == -1) errExit("close");
    if (close(pipe_out[WRITE]) == -1) errExit("close");
    if (close(pipe_out[READ]) == -1) errExit("close");

    /* Wait on N process
     *   or SIGCHLD all */
    for(int i = 0; i < 3; ++i) {
        if (wait(NULL) == -1) errExit("wait");
    }
}
