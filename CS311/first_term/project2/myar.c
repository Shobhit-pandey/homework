/*
 * Original Author: Trevor Bramwell (bramwelt)
 * File: bramwelt_proj2.c
 * Created: 2011 October 24, 08:16 by bramwelt
 * Last Modified: 2011 October 30, 00:00 by bramwelt
 * 
 * This file contains functions related to project 2.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h> /* fchmod */
#include <getopt.h> /* getopt libs */
#include <string.h> /* strcmp,strcat,strcpy */
#include <fcntl.h>
#include <ar.h> /* AR */

#include "myar_lib.h"

#define AUTHOR "Trevor Bramwell"
#define PROG "myar"
#define DEBUG 1
#define FILE_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) /* 666 */
#define ARGLIST "qxtvdAh"
#define USAGE "Usage: %s [options] archive [file,...]\n"
#define LONG_USAGE \
    USAGE \
    "\nArguments:\n" \
    " archive  the archive to use\n" \
    " file     file to add/remove/extract from archive\n" \
    "\nOptions:\n" \
    " -q  quickly append named files to archive\n" \
    " -x  extract named files\n" \
    " -t  print a concise table of contents of the archive\n" \
    " -v  print a verbose table of contents of the archive\n" \
    " -d  delete named files from archive\n" \
    " -A  quickly append all 'regular' files in the current " \
    "directory (except the archive itself)\n" \
    " -h  Output this help text and exit\n"

/* Extern function delecrations */
extern int fchmod(int fd, mode_t mode);

/* Function Declarations */
void
process_opts( int argc, char *argv[], int *flags, int *open_flags);

void
handle_flags(
    int archive_fd,
    char *archive,
    char *filelist[], int filelist_len,
    int flags
);


int main(int argc, char *argv[]) {
    int flags = 0x00; /* 8 bits to hold flags */
    int filelist_len = 0;
    int open_flags = O_RDWR;

    char *archive;
    int archive_fd;
    char *filelist[argc];

    /* short-circuit if only no args */
    if(argc == 1) {
        printf(USAGE, PROG);
    } 

    /* handle arguments */
    process_opts(argc, argv, &flags, &open_flags);
    
    /* Get the archive filename */
    archive = argv[optind++];

    /* TODO: DEBUG CODE */
    filelist_len = 0;

    /* add the filenamess passed in to fileslist */
    for(int i = optind; i < argc; i++) {
        filelist[filelist_len++] = argv[i];
    }

    errno = 0;
    if((archive_fd = open(archive, open_flags, FILE_PERMS)) != -1) {
        handle_flags(archive_fd, archive, filelist, filelist_len, flags);
    } else if(errno != 0) {
        printf("stuff");
        exit(errno);
    }

    close(archive_fd);
    exit(EXIT_SUCCESS);
}

void process_opts(int argc, char *argv[], int *flags, int *open_flags) {
    int opt;
    /* get opts */
    while((opt = getopt(argc, argv, ARGLIST)) != -1) {
        switch(opt) {
            case 'q': *flags |= AR_Q; *open_flags |= O_CREAT; break;
            case 'x': *flags |= AR_X; *open_flags &= ~O_WRONLY; break;
            case 't': *flags |= AR_T; *open_flags &= ~O_WRONLY; break;
            case 'v': *flags |= AR_V; *open_flags &= ~O_WRONLY; break;
            case 'd': *flags |= AR_D; *open_flags |= O_TRUNC; break;
            case 'A': *flags |= AR_A; break;
            case 'h': *flags |= AR_H; break;
            case '?': 
                fprintf(stderr, USAGE, PROG);
                exit(EXIT_FAILURE);
            default: break;
        }
    }
    /* Output set flags */
    #if DEBUG 
    printf("\n------\nFLAGS: %x\n------\n", *flags);
    printf("optind: %d, argc: %d\n", optind, argc);
    #endif
    if(optind >= argc && ((*flags & AR_H) <= 0)) {
        fprintf(stderr, "error: Expected argument after options\n");
        exit(EXIT_FAILURE);
    } else if ((*flags & AR_H) > 0){
        printf(LONG_USAGE, PROG);
        exit(EXIT_SUCCESS);
    } 

    if(argv[1][0] != '-') { /* first character of first option/arg passed */
        fprintf(stderr, "error: Must provide at least one option\n");
        exit(EXIT_FAILURE);
    }
}


void handle_flags(int archive_fd, char* archive, 
    char *filelist[], int filelist_len, int flags) {
    #if DEBUG
    struct stat file_st;
    int chmod_success; //, file_appended;
    #endif
    if(is_archive(archive_fd) == FALSE) {
        if((flags & AR_Q) <= 0)  {
            /* not an archive, and -q not passed */
            fprintf(stderr, "error: '%s' is not an archive file\n", archive);
            exit(ENOTAR);
        } else {
            /* not an archive, and -q passed */
            if(file_length(archive_fd) == 0) {
                if(create_archive(archive_fd) == FALSE) {
                    fprintf(stderr, "Error creating archive.\n");
                    exit(EXIT_FAILURE);
                }
                printf("%s: creating %s\n", PROG, archive);
                #if DEBUG
                myar_fstat(archive_fd, &file_st);
                print_file_stat(&file_st);
                if((file_st.st_mode & FILE_PERMS) > 0){
                    errno = 0;
                    if((chmod_success = fchmod(archive_fd, FILE_PERMS)) == 0) {
                        printf("setting archive perms to rw-rw-rw- (666)\n");
                    } else {
                        printf("could not change file permissions\n");
                        exit(errno);
                    }
                }
                #endif
            }
        }
    } else {
        if((flags & AR_Q) > 0) {
            /* append files */
            for(int j = 0; j < filelist_len; j++) {
                printf("q - %s\n", filelist[j]);
                append_file(archive_fd, filelist[j]);
            }
        } else if ((flags & AR_X) > 0) {
            printf("Flag X passed\n");
        } else if ((flags & AR_T) > 0) {
            printf("Flag T passed\n");
        } else if ((flags & AR_V) > 0) {
            printf("Flag V passed\n");
        } else if ((flags & AR_D) > 0) {
            printf("Flag D passed\n");
        } else if ((flags & AR_A) > 0) {
            printf("Flag A passed\n");
        }
    }
}

