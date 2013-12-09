#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "compute.h"
#include "main.h"

/* Profile determines the number of operations that can be performed in
 * a matter of 15 seconds */
void* profile(void *perf)
{
    time_t start;
    time_t end;
    time_t diff;

	int j = 12345;
    float max = 100000.0;
	volatile int k = 0;

    do {
        start = time(NULL);
        for (int i = 1; i < max; ++i) {
            k = j % i;
        }
        end = time(NULL);
        diff = (time_t) (end - start);
        if (diff <= 1) {
            max *= 10;
        } else if (diff < 15) {
            max *= (float)(15/(float)diff);
        } else if (diff == 16) {
            max -= (100000 + (int) k);
        } else if (diff > 15) {
            max /= 2.0;
        }
    } while ((end - start) != 15);

    *((int *)perf) = (int) max;

	return NULL;
}

/* Computer the perfectality of a number.
 *
 * Sum all factors up to 1/2 of the number. If equal to the number, it
 * is perfect, else not, */
int is_perfect(long number)
{
    long perfect = 1;
    long max = (number/2);

    for(long i = 2; i <= max; ++i) {
        if ((number % i) == 0)
            perfect += i;
    }

    if (number == perfect)
        return 1;

    return 0;
}

/* Compute the prefect numbers up to n. Brute Force. */
int compute_perfect_numbers(const long max, int n[], const int n_size)
{
    int idx = 0;
    long i = 0;

    while(i < max && idx < n_size) {
        if(is_perfect(i)) {
            n[idx++] = i;
        }
        ++i;
    }

    return idx;
}

/* Open and connects to a socket, and returns the resulting file
 * descriptor */
int setup_socket(const char *ipaddr, int port)
{
    int retval;
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) errExit("socket");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

    // Convert IP address to binary form.
	retval = inet_pton(AF_INET, ipaddr, &servaddr.sin_addr.s_addr);
    if (retval <= 0) {
       if (retval == 0)
           fprintf(stderr, "Not valid presentation format: %s\n", ipaddr);
       else
           perror("inet_pton");
       exit(EXIT_FAILURE);
    }

	retval = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(retval == -1) {
        close(sockfd);
        errExit("connect");
    }

    return sockfd;
}

// Open AF_INET socket to server
//   Makes sure server available
// [T] Find performance metric
//   Sends metric to server
// [T] Read messages from server thread/process
//   If message is terminate, send sigterm to self
//   If message is range of nums, compute and send back result
int compute(const char *ipaddr, int port, FILE* output)
{
    int retval;
    int performance = 1;
    pthread_t perf_pt;

    if (output == NULL) {
        output = stdout;
    }

    fprintf(output, "Connecting to %s:%ld\n", ipaddr, (long) port);

    retval = pthread_create(&perf_pt,
                            NULL,
                            profile,
                            (void *) &performance);
    if(retval != 0) errExit("pthread_create");


    int sockfd = setup_socket(ipaddr, port);

	char sendline[MAXLINE];
	char recvline[MAXLINE];

	while(fgets(sendline, MAXLINE, stdin) != NULL ){

		bzero(recvline, MAXLINE);

		write(sockfd, sendline, strlen(sendline));

		if(read(sockfd, recvline, MAXLINE) == 0) {
			perror("Something broke");
			exit(-1);
		}

		fputs(recvline, stdout);

	}

    const int size = 4;
    const long max = 9000;
    int perfect_numbers[size];

    int nums = compute_perfect_numbers(max, perfect_numbers, size);

    for (int i = 0; i < nums; ++i) {
        printf("%d\n", perfect_numbers[i]);
    }

    pthread_join(perf_pt, NULL);
    printf("%ju\n", (uintmax_t) performance);

    close(sockfd);
    return 0;
}


