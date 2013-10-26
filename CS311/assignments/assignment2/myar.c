/*
 * myar.c
 *
 * Author: Trevor Bramwell
 * Class: CS311
 */
#include <ar.h>

#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define AR_STRUCT_SIZE 60

#define F_TABLE   0x01
#define F_QUICK   0x02
#define F_APPEND  0x04
#define F_XTRACT  0x08
#define F_DELETE  0x10
#define F_WAIT    0x20

#define TRUE 0
#define FALSE 1

int verbose = FALSE;

/* Utility Functions
 *
 * Note: archive here means the path to the archive file.
 *
 *
 * find_file(fd, file) - if file in archive: return
 *   byte (start) and (end+1); if file not in archive: return -1.
 *
 * append_file(fd, file) - takes both paths, calls out to
 *   open_file, writes the file to the archive.
 *
 */

int open_archive(const char *archive) {
    /*
     * given a ar filename, return a fd for the ar.
     *   Check to ensure fd is valid, and use perror to output failure. Also
     *   contains an optional argument to create the archive if it doesn't
     *   exist.
     *   - This may return a struct containing the file and stat of file...
     *     operation will most likely need the size...
    */
    int fd;
    char armag[SARMAG];

    if ((fd = open(archive, O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (read(fd, armag, SARMAG) != SARMAG) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (memcmp(armag, ARMAG, SARMAG) != 0) {
        fprintf(stderr, "Aborting: Not a valid archive file.\n");
        exit(EXIT_FAILURE);
    }

    return fd;
}

void close_archive(int fd) {
    /*
     * close_archive(fd) - closes the archive at the given fd, checks result
     *   to make sure no errors given. If errors, perror and fail
     */
    if(close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
}
/* Append all regular files 
 * 
 * get list of regular files in current directory (except archive
 *   itself)
 *  - uses stat and checks file type
 * pass list to 'quick append' function
 *
 */

/* Quick Append
 *
 * open archive
 * for each filename
 *   open file
 *   write ar header for file to archive
 *   write file to archive
 *   close file
 * close archive
 *
 * */

void table(const char *archive) {
/* Table
 *
 * Open archive file
 * byte = size of ar header +1
 * while not at end of file
 *   read in till size of ar_struct as ar_struct (60 chars)
 *   if verbose print all file info
 *   else print ar_struct.name
 *   seek to ar_struct.size (+1 ?)
 *   set bytes to current position
 * close archive
 *
 */
    int fd;
    int ar_pos = SARMAG+1;
    int size = 0;
    struct ar_hdr file_header;
    struct stat ar_stat;
    char **endptr = NULL;

    fd = open_archive(archive);

    if (fstat(fd, &ar_stat) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    while(ar_pos < ar_stat.st_size) {
        if(read(fd, &file_header, AR_STRUCT_SIZE) != AR_STRUCT_SIZE) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("%.*s", 60, (char *)&file_header);

        if ((size = strtol(file_header.ar_size, endptr, 10)) < 1)
        {
            perror("strtol");
            exit(EXIT_FAILURE);
        }

        if ((ar_pos = lseek(fd, (size+1), SEEK_CUR)) == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
    }

    close_archive(fd);
}

/* Delete
 *
 * Open archive
 * while not at end of file
 *   read in files until reaching one where ar_struct.name is same as
 *   filename; break
 * (pointer should be at beginning of file now. file can be copied by
 *  copying bytes up to size of file [from ar_struct.size])
 * if not found, fail
 * create a new archive file
 * copy bytes of first file to new file
 * unlink original archive
 * close archive
 *
 */

/* Extract
 * 
 * open archive
 *   while not at end of file
 *     read in headers, skip file content,  until reaching one where
 *     ar_struct.name is same as one of filenames
 *       add filename to list of filenames already found
 *       (pointer should be at beginning of file now. file can be copied by
 *        copying bytes up to size of file [from ar_struct.size])
 *       open and create a new file with name of file asked for, along with
 *         file stats from archive file
 *       copy bytes from archive to file
 *       close file; 
 *   if length of filenames handled not same as filenames requested continue,
 *   else close archive and fail, since we did find any files
 * close archive
 * */

/* Verbose
 *
 * Interact with ar binary to see what information is given
 * Know for sure 'table' will basically print stat of file as well (well
 *   the info it has at least, only saves mtime? so atime and ctime will
 *   be the same)
 */

/*
 * myar is an simple implementation of the UNIX archive (ar) program.
 *  It is design to create archives, extract files, delete files, and add
 *  files to an archive.
 */
int main(int argc, char *argv[])
{
    int opt;
    short flags = 0;

    while ((opt = getopt(argc, argv, "Adqtvw:x")) != -1) {
        switch (opt) {
        case 'A':
            // Append all regular files
            flags |= F_APPEND;
            break;
        case 'd':
            // Delete files
            flags |= F_DELETE;
            break;
        case 'q':
            // Quick append
            flags |= F_QUICK;
            break;
        case 't':
            // Table
            flags |= F_TABLE;
            break;
        case 'v':
            // Enable Verbosity
            verbose = TRUE;
            break;
        case 'w':
            // Append modified with timeout
            flags |= F_WAIT;
            break;
        case 'x':
            // Extract files
            flags |= F_XTRACT;
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

    if (flags == 0) {
        fprintf(stderr, "At least one non-verbose option required.\n");
        exit(EXIT_FAILURE);
    }

    if (verbose == TRUE) {
        printf("argc: %ld, optind: %ld, flags: 0X%03o\n", (long)argc,
                (long)optind, flags);
    }

    if ((flags ^ F_TABLE) == 0) {
        table(argv[optind]);
    } else {
        fprintf(stderr, "Option not supported yet.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
