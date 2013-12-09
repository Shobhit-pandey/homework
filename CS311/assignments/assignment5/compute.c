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

FILE *output;
char sendline[MAXLINE];
char recvline[MAXLINE];

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

    *((long *)perf) = (long) max;

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

/* Compute perfect numbers from init to max.
 * Store them in the array n, and return the number of perfect numbers,
 * if any found. Returns -1 if no numbers found. */
int compute_perfect_numbers(const long init, const long max, int n[])
{
    int idx = 0;
    long i = init;

    while(i < max) {
        if(is_perfect(i)) {
            n[idx++] = i;
        }
        ++i;
    }

    if (idx == 0)
        return -1;
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

    fprintf(output, "Connected to %s:%ld\n", ipaddr, (long) port);

    return sockfd;
}

// Open AF_INET socket to server
//   Makes sure server available
// [T] Find performance metric
//   Sends metric to server
// [T] Read messages from server thread/process
//   If message is terminate, send sigterm to self
//   If message is range of nums, compute and send back result
int compute(const char *ipaddr, int port, FILE* out_file)
{
    int retval;
    long performance = 1;
    long init;
    long max;
    char tmpbuf[LONGSTRSIZE];
    int nums;
    int sockfd;
    pthread_t perf_pt;

    if (out_file == NULL){
        output = stdout;
    } else {
        output = out_file;
    }

    retval = pthread_create(&perf_pt,
                            NULL,
                            profile,
                            (void *) &performance);
    if (retval != 0)
        errExit("pthread_create");

    sockfd = setup_socket(ipaddr, port);

    // Wait for the thread to finish.
    pthread_join(perf_pt, NULL);
    performance /= 2;
    printf("%ju\n", (uintmax_t) performance);

    // Begin loop
    for(;;) {
        // Since performance won't change, we will send the same one every
        // time.
        int perfect_numbers[ARRSIZE];

        // Send performance to server, get range back.
        sprintf(sendline, "<xml><perf>%ld</perf></xml>", performance);
        fprintf(output, "Sending: %s\n", sendline);

        if (write(sockfd, sendline, strlen(sendline)) == 0)
            errExit("write - socket");

        bzero(recvline, MAXLINE);

        /* Parse XML on recvline 
         * Set :init to response
         * Set :max to (response + performance)
         */
        if (read(sockfd, recvline, MAXLINE) == 0)
            errExit("read - socket");

        if (sscanf(recvline, "<xml><range>%ld</range></xml>", (long *)&init) != 1)
            errExit("sscanf - wrong data");
        fprintf(output, "Recieved: %ld\n", init);

        // Calculate max of range to compute
        max = init + performance;

        fprintf(output, "Computing perfect numbers: %ld to %ld\n", (long) init, (long) max);
        nums = compute_perfect_numbers(init, max, perfect_numbers);

        // Clear sendline
        bzero(sendline, MAXLINE);

        if (nums > 0) {
            fprintf(output, "Sending perfect numbers to server.\n");
            // Send numbers found to server.
            strcat(sendline, "<xml>");
            for (int i = 0; i < nums; ++i) {
                bzero(tmpbuf, LONGSTRSIZE);
                sprintf(tmpbuf, "<number>%ld</number>", (long) perfect_numbers[i]);
                strcat(sendline, tmpbuf);
            }
            strcat(sendline, "</xml>");

            fprintf(output, "Sending: %s\n", sendline);

            if (write(sockfd, sendline, strlen(sendline)) == 0)
                errExit("write - socket");
        } else {
            fprintf(output, "Didn't find perfect numbers.\n");
        }

    // End loop (Next iteration will get another range)
    }

    close(sockfd);
    return 0;
}


