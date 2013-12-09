/*
 * CS311: Project 3 - Happy Primes
 *
 * Author: Trevor Bramwell
 */
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "compute.h"
#include "main.h"

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

const char *usage = "Usage: %s [-i IP] [-p PORT] [-o output]\n\n"
                    "Compute perfect numbers, and send results to IP:PORT.\n\n"
                    "  -i\tIP of the compute server\n"
                    "  -p\tPORT of the compute server\n"
                    "  -o\tLog file for output\n";
/*
 * Main
 *
 */
int main(int argc, char *argv[])
{
    char ipaddr[INET_ADDRSTRLEN];
    int opt;
    int port = 50000;
    FILE *output = NULL;

    while ((opt = getopt(argc, argv, "o:h")) != -1) {
        switch (opt) {
        case 'o':
            output = fopen(optarg, "w");
            break;
        case 'h':
            printf(usage, argv[0]);
            exit(EXIT_SUCCESS);
        default: /* '?' */
            fprintf(stderr, usage, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc != 3) {
        fprintf(stderr, "Expected IP and PORT as arguments.\n");
        exit(EXIT_FAILURE);
    }

    sscanf(argv[optind++], "%s", ipaddr);
    sscanf(argv[optind++], "%d", &port);

    // Compute
    return compute(ipaddr, port, output);
}

void errExit(const char *errmsg)
{
    perror(errmsg);
    exit(EXIT_FAILURE);
}
