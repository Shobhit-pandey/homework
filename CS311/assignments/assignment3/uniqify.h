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
void supervisor(FILE *input, FILE *output, int procs);
void parser(int pipe_in[][2], int pipe_out[][2], int procs, FILE *write_stream[], FILE *input);
void sorter(int pipe_in[][2], int pipe_out[][2], int procs, int i);
void suppressor(int pipe_in[][2], int pipe_out[][2], int procs, FILE *read_stream[], FILE *output);
void close_pipes(int arr[][2], int procs, int proc, int end);
void init_pipes(int arr[][2], int procs);

void errExit(const char *func) {
    perror(func);
    exit(EXIT_FAILURE);
}

#endif
