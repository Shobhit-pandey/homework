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
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

static volatile int numLiveChildren = 0;

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
    char buf[64];
    close_pipes(pipe_in, procs, ALL, READ);
    close_pipes(pipe_out, procs, ALL, ALL);

    for (int i = 0; i < procs; ++i) {
        if ((write_stream[i] = fdopen(pipe_in[i][WRITE], "a")) == NULL) errExit("fdopen");
    }

    /* http://stackoverflow.com/questions/15865002/
     * removing-special-characters-from-fscanf-string-in-c */
    long j = 0;
    while ((fscanf(input, "%[a-zA-Z]", buf) != EOF)) {
        int s_idx = (int)(j % procs);
        if(fputs(buf, write_stream[s_idx]) == EOF) errExit("fputs");
        if(fputs("\n", write_stream[s_idx]) == EOF) errExit("fputs");
        fscanf(input, "%[^a-zA-Z]", buf);
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
 *
 *  In this assignment words are all alphabetic and case insensitive, with
 *  the parser converting all alphabetic characters to lower case. Any
 *  non-alphabetic characters delimit words and are discarded.
 *
 */
void suppressor (int pipe_in[][2], int pipe_out[][2], int procs, FILE *read_stream[], FILE *output)  {
    /*
     * Round robin distribute words
     *
     * For each word in input, parse, and write round robin to pipe.
     */
    close_pipes(pipe_out, procs, ALL, WRITE);
    close_pipes(pipe_in, procs, ALL, ALL);

    /* Read from pipe from sort, to output:(stdout|FILE) */
    for (int i = 0; i < procs; ++i) {
        if ((read_stream[i] = fdopen(pipe_out[i][READ], "r")) == NULL) errExit("fdopen");
    }

    suppress_loop(read_stream, output, procs);

    for (int i = 0; i < procs; ++i) {
        if (fclose(read_stream[i]) == EOF) errExit("fclose");
    }

    _exit(EXIT_SUCCESS);
}

/*
 * Suppress Loop
 *
 * While there is still input from each pipe
 *   Find the smallest word.
 *   Output the word and read in another in it's place
 *   For each pipe, remove all duplicates of the word.
 *   Repeat.
 */
void suppress_loop(FILE *read_stream[], FILE *output, int procs) {
    char buf[procs][64];

    int min_idx = -1;
    int pipes_finished = 0;
    char min_str[64] = "";
    char closed_pipes[procs];

    /* Fill all the buffers and initialize closed_pipes arr */
    for (int i = 0; i < procs; ++i) {
        closed_pipes[i] = 0;
        if (fgets(buf[i], 64, read_stream[i]) == NULL) {
            closed_pipes[i] = 1;
            ++pipes_finished;
        }

    }

    /* Continue reading words until all pipes are empty */
    while (pipes_finished < procs) {
        /* Find smallest word in pipes */
        for (int i = 0; i < procs; ++i) {
            // Skip closed pipes
            if (closed_pipes[i] == 1) continue;
            if (min_idx == -1) min_idx = i;
            if (strncmp(buf[i], buf[min_idx], 64) < 0) {
                min_idx = i;
            }
        }

        strncpy(min_str, buf[min_idx], 64);
        fprintf(output, "%s", min_str);

        // If that was the last word in the pipe...
        if (fgets(buf[min_idx], 64, read_stream[min_idx]) == NULL) {
            closed_pipes[min_idx] = 1;
            ++pipes_finished;
        }

        // Remove duplicates from every pipe
        for (int i = 0; i < procs; ++i) {
            if (closed_pipes[i] == 1) continue;
            while (strncmp(buf[i], min_str, 64) == 0) {
                if (fgets(buf[i], 64, read_stream[i]) == NULL) {
                    closed_pipes[i] = 1;
                    ++pipes_finished;
                    break;
                }
            }
        }
    }
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
 * SIG Handler
 *
 * Used & modified from TLPI pg.557 (Listing 26-5).
 */
static void sigHandler(int sig)
{
    /* Just propogate signal to children */
    kill(0, SIGHUP);
    exit(EXIT_FAILURE);
}

/*
 * Supervisor
 *
 * All processes should terminate cleanly on INTR, QUIT, and HANGUP
 * signals. This requires the installation of a signal handler for each of
 * these three signals. Ensure you do this via sigaction, rather than
 * signal. Also, ensure you issue QUIT signals to all children, as well.*
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

    struct sigaction sa = {
        .sa_flags = 0,
        .sa_handler = sigHandler
    };
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGQUIT, &sa, NULL) == -1) errExit("sigaction");
    if (sigaction(SIGINT, &sa, NULL) == -1) errExit("sigaction");
    if (sigaction(SIGHUP, &sa, NULL) == -1) errExit("sigaction");

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
    
    /* Wait on N+2 process */
    for(int i = 0; i < (procs+2); ++i) {
        if (wait(NULL) == -1) errExit("wait");
    }
}
