
#ifndef _COMPUTE_H
#define _COMPUTE_H

#define MAXLINE 4096
#define LISTENQ 1024
#define MAXSOCKADDR 128
#define BUFFSIZE 4096
#define ARRSIZE 64

#define LONGSTRSIZE 128

#define SERV_PORT 9879

int compute(const char *ipaddr, int port, FILE* output);
int compute_perfect_numbers(const long init, const long max, int n[]);
int is_perfect(long number);
void* profile(void *perf);

#endif
