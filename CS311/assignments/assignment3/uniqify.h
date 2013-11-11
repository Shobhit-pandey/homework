/*
 * CS311: Project 3 - Uniqify
 *
 * Author: Trevor Bramwell
 */

#ifndef UNIQIFY
#define UNIQIFY

#define TRUE 0
#define FALSE 1

int main(int argc, char *argv[]);
void parser(FILE *input, FILE *output, long procs);
void close_pipes(int pid, int ***arr, int procs, int dir);
void init_pipes(int ***arr, int procs);
void free_pipes(int ***arr, int procs);

void errExit(const char *func) {
    perror(func);
    exit(EXIT_FAILURE);
}

#endif
