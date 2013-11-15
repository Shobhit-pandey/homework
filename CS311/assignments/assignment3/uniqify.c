/*
 * CS311: Project 3 - Uniqify
 *
 * Author: Trevor Bramwell
 */

#define _POSIX_SOURCE

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
    int procs = 1;
    int opt;
    FILE *input = NULL;
    FILE *output = NULL;

    while ((opt = getopt(argc, argv, "n:o:h")) != -1) {
        switch (opt) {
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
        input = fopen(argv[optind], "r");
        if (input == NULL)
            errExit("fopen");
    }

    supervisor(input, output, procs);

    exit(EXIT_SUCCESS);
}

/*
 * Allocate space for 4N pipes
 */
void init_pipes(int arr[][2], int procs) {
    for (int i = 0; i < procs; ++i) {
        if (pipe(arr[i]) == -1) errExit("pipe");
    }
}

/*
 * Close both ends of all pipes but only one 'end' for the 'proc' pipe.
 */
void close_pipes(int arr[][2], int procs, int proc, int end) {
    for (int i = 0; i < procs; ++i) {
        if (i == proc) {
            close(arr[i][end]);
        } else if (end != ALL) {
            close(arr[i][end]);
        } else {
            close(arr[i][READ]);
            close(arr[i][WRITE]);
        }
    }
}
        

/*
 * Parser
 */
void parser(int pipe_in[][2], int pipe_out[][2], int procs, FILE *write_stream[], FILE *input)  {
    char buf[512];
    close_pipes(pipe_in, procs, ALL, READ);
    close_pipes(pipe_out, procs, ALL, ALL);

    for (int i = 0; i < procs; ++i) {
        if ((write_stream[i] = fdopen(pipe_in[i][WRITE], "a")) == NULL) errExit("fdopen");
    }

    long j = 0;
    while ((fscanf(input, "%s", buf) != EOF)) {
        int s_idx = (int)(j % procs);
        if(fputs(buf, write_stream[s_idx]) == EOF) errExit("fputs");
        if(fputs("\n", write_stream[s_idx]) == EOF) errExit("fputs");
        ++j;
    }

    for (int i = 0; i < procs; ++i) {
        if (fclose(write_stream[i]) == EOF) errExit("fclose");
    }
    
    /* First pipes are now all closed. sort should have recieved it's
     * final input */
    _exit(EXIT_SUCCESS);
}

/* 
 * Suppressor
 */
void suppressor (int pipe_in[][2], int pipe_out[][2], int procs, FILE *read_stream[], FILE *output)  {
    /*
     * Round robin distribute words
     *
     * For each word in input, parse, and write round robin to pipe.
     */
    char buf[512];
    close_pipes(pipe_out, procs, ALL, WRITE);
    close_pipes(pipe_in, procs, ALL, ALL);

    /* Read from pipe from sort, to output:(stdout|FILE) */
    for (int i = 0; i < procs; ++i) {
        if ((read_stream[i] = fdopen(pipe_out[i][READ], "r")) == NULL) errExit("fdopen");
    }

    long j = 0;
    int s_idx = 0;
    while ((fgets(buf, 512, read_stream[s_idx]) != NULL)) {
        s_idx = (int)(j % procs);
        if(fprintf(output, "%s", buf) == EOF) errExit("fputs");
        ++j;
    }

    for (int i = 0; i < procs; ++i) {
        if (fclose(read_stream[i]) == EOF) errExit("fclose");
    }

    _exit(EXIT_SUCCESS);
}

/*
 * Sorter
 */
void sorter(int pipe_in[][2], int pipe_out[][2], int procs, int i) {
    close_pipes(pipe_in, procs, i, WRITE);
    close_pipes(pipe_out, procs, i, READ);

    /* Reassign sort input FD */
    if (pipe_in[i][READ] != STDIN_FILENO) {
        if (dup2(pipe_in[i][READ], STDIN_FILENO) == -1) errExit("dup2");
        if (close(pipe_in[i][READ]) == -1) errExit("close");
    }

    /* Reassign sort output FD */
    if (pipe_out[i][WRITE] != STDOUT_FILENO) {
        if (dup2(pipe_out[i][WRITE], STDOUT_FILENO) == -1) errExit("dup2");
        if (close(pipe_out[i][WRITE]) == -1) errExit("close");
    }

    /* Envoke sort for each child */
    if ((execlp("sort", "sort", (char *) NULL)) == -1) errExit("execve");

    _exit(EXIT_SUCCESS);
}

/*
 * Supervisor
 */
void supervisor(FILE *input, FILE *output, int procs)
{
    pid_t cpid;
    int pipe_in[procs][2];
    int pipe_out[procs][2];

    FILE *write_stream[procs];
    FILE *read_stream[procs];

    /* Initialize Pipes */
    init_pipes(pipe_in, procs);
    init_pipes(pipe_out, procs);

    if (input == NULL) input = stdin;
    if (output == NULL) output = stdout;

    /* Parser */
    switch (cpid = fork()) {
    case -1: errExit("fork");
    case 0: parser(pipe_in, pipe_out, procs, write_stream, input);
    default: break;
    }

    /* Sorter */
    for (int i = 0; i < procs; ++i) {
        switch (cpid = fork()) {
        case -1: errExit("fork");
        case 0: sorter(pipe_in, pipe_out, procs, i);
        default: break;
        }
    }

    /* Suppressor */
    switch(cpid = fork()) {
    case -1: errExit("fork");
    case 0: suppressor(pipe_in, pipe_out, procs, read_stream, output);
    default: break;
    }
    
    close_pipes(pipe_in, procs, ALL, ALL);
    close_pipes(pipe_out, procs, ALL, ALL);
    
    /* Wait on N process
     *   or SIGCHLD all */
    for(int i = 0; i < (procs+2); ++i) {
        if (wait(NULL) == -1) errExit("wait");
    }
}
