
#ifndef _COMPUTE_H
#define _COMPUTE_H

#define MAXLINE 4096
#define LISTENQ 1024
#define MAXSOCKADDR 128
#define BUFFSIZE 4096

#define SERV_PORT 9879


int main(int argc, char *argv[]);
int compute_perfect_numbers(const long max, int n[], const int n_size);
int is_perfect(long number);
void* profile(void *perf);
int setup_socket(const char *ip_addr);

#endif
