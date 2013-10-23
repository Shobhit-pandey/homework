/*
 * myar.c
 *
 * Author: Trevor Bramwell
 * Class: CS311
 */
#include <getopt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
 * myar is an simple implementation of the UNIX archive (ar) program.
 *  It is design to create archives, extract files, delete files, and add
 *  files to an archive.
 */
int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "Adqtvw:x")) != -1) {
        switch (opt) {
        case 'A':
            // Append all regular files
            break;
        case 'd':
            // Delete files
            break;
        case 'q':
            // Quick append
            break;
        case 't':
            // Table
            break;
        case 'v':
            // Verbose flag
            break;
        case 'w':
            // Append modified with timeout
            break;
        case 'x':
            // Extract files
            break;
        default:
            // ? or : characters
            fprintf(stderr, "Usage: %s\t[-v] [-Atw] <archive>,\n"\
                            "         \t[-v] [-dqx] <archive> [file...]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    printf("Argument(s): ");
    while(optind < argc) {
        printf("%s ", argv[optind++]);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
