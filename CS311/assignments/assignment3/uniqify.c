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
    char buf[64];
    close_pipes(pipe_out, procs, ALL, WRITE);
    close_pipes(pipe_in, procs, ALL, ALL);

    /* Read from pipe from sort, to output:(stdout|FILE) */
    for (int i = 0; i < procs; ++i) {
        if ((read_stream[i] = fdopen(pipe_out[i][READ], "r")) == NULL) errExit("fdopen");
    }

    long j = 0;
    int s_idx = 0;
    while ((fgets(buf, 64, read_stream[s_idx]) != NULL)) {
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
 * Suppress Loop
 */
void suppress_loop(FILE *read_stream[], FILE *output, int procs) {
    char buf[procs][64];

    int j = 0;
    int s_idx = 0;
    int finished_pipes = 0;
    int pipes_checked = 0;
    char *min_str;

    /* Initially fill the tmp_buf with proc words. */
    for (int i = 0; i < procs; ++i) {
        if (fgets(buf[i], 64, read_stream[i]) == NULL) {
            buf[i][0] = '\0';
            ++finished_pipes;
        }
    }

    /* Begin suppression */
    /* Continue reading words until all pipes are empty */
    while (finished_pipes < procs) {
        /* Find pipe with smallest word and output, but only if we are
         * done with the previous word. */
        if (pipes_checked == 0) {
            pipes_checked = procs;
            min_str = buf[0];
            for (int i = 0; i < procs; ++i) {
                if (strcmp(buf[i], min_str) < 0) {
                    min_str = buf[i];
                }
                ++i;
            }
            /* Remember word, output. */
            if(fprintf(output, "%s", min_str) == EOF) errExit("fputs");
        }

        /* Get next word in pipe, if same, throw away, else, next pipe */
        int cmp = 0;
        while (pipes_checked > 0) {
            s_idx = (int)(j % procs);
            cmp = strcmp(min_str, buf[s_idx]);
            if (cmp == 0) {
                if (fgets(buf[s_idx], 64, read_stream[s_idx]) == NULL) {
                    ++finished_pipes;
                    ++j;
                }
            } else if (cmp > 0) {
                --pipes_checked;
                ++j;
            } else {
                fprintf(stderr, "Oops! Looks like we didn't find the minimum "
                                "string correctly.\n");
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
 * SIGCHLD Handler
 *
 * Used & modified from TLPI pg.557 (Listing 26-5).
 */
static void sigchldHandler(int sig)
{
    int savedErrno = errno;
    pid_t childPid;

    while ((childPid = waitpid(-1, NULL, WNOHANG)) > 0) {
        numLiveChildren--;
    }
    if (childPid == -1 && errno != ECHILD) {
        perror("waitpid");
        _exit(EXIT_FAILURE);
    }

    errno = savedErrno;
}

/*
 * Supervisor
 *
 * All processes should terminate cleanly on INTR, QUIT, and HANGUP
 * signals. This requires the installation of a signal handler for each of
 * these three signals. Ensure you do this via sigaction, rather than
 * signal. Also, ensure you issue QUIT signals to all children, as well.*
 *
 * Catch SIGCHLD and wait
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

    /* Take from TLPI */
    int sigCnt = 0;
    numLiveChildren = (procs-1+2);

    sigset_t blockMask;
    sigset_t emptyMask;
    struct sigaction sa = {
        .sa_flags = 0,
        .sa_handler = sigchldHandler
    };

    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGCHLD, &sa, NULL) == -1) errExit("sigaction");
    /* Block SIGCHLD to prevent its delivery if a child terminates
     * before the parent commences the sigsuspend() loop below */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);

    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1) errExit("sigprocmask");
    /* End reference */

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
    sigemptyset(&emptyMask);
    while (numLiveChildren > 0) {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR) errExit("sigsuspend");
        sigCnt++;
    }

    for(int i = 0; i < (procs+2); ++i) {
        if (wait(NULL) == -1) errExit("wait");
    }
}
