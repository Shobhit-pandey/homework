/*
 * CS311: Project 3 - Uniqify
 *
 * Author: Trevor Bramwell
 */

#ifndef UNIQIFY
#define UNIQIFY

#define TRUE 0
#define FALSE 1

#define READ 0
#define WRITE 1

#define PIPE_SIZE 2

#define ALL -1

int main(int argc, char *argv[]);
void parser(FILE *input, FILE *output, long procs);
void close_pipes(int ***arr, int procs, int proc);
void init_pipes(int ***arr, int procs);

void errExit(const char *func) {
    perror(func);
    exit(EXIT_FAILURE);
}

#endif
