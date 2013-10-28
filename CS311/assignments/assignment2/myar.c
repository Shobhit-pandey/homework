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

void print_hdr(struct ar_hdr *hdr) {
    /*
     * Print Header
     *
     * Prints an ar_hdr to stdout.
     */
    printf("%.60s", (char *)hdr);
}

long build_hdr(const char *file, struct ar_hdr* hdr) {
    /*
     * Build Header
     *
     * Constructs the ar header for a file.
     *
     * Takes the file metadata from stat and puts it into an ar_hdr
     *   struct. Returns the preferred blksize for writing the file.
     */
    struct stat st;

    if (stat(file, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // Might require '/' at end of name
    sprintf(hdr->ar_name, "%-16s", file);
    sprintf(hdr->ar_date, "%-12ld", (long) st.st_mtime);
    sprintf(hdr->ar_uid, "%-6ld", (long) st.st_uid);
    sprintf(hdr->ar_gid, "%-6ld", (long) st.st_gid);
    sprintf(hdr->ar_mode, "%-8lo", (unsigned long) st.st_mode);
    sprintf(hdr->ar_size, "%-10lld", (long long) st.st_size);
    memcpy(hdr->ar_fmag, ARFMAG, 2);

    return ((long) st.st_blksize);
}

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

void quick_append(const char *archive, const char *files[], int num_files) {
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
    int fd;
    const char *cur_file;
    struct ar_hdr file_hdr;

    if ((fd = open(archive, O_WRONLY | O_APPEND)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_files; i++) {
        //printf("Appending: %s\n", files[i]);
        cur_file = files[i];
        build_hdr(cur_file, &file_hdr);
        print_hdr(&file_hdr);
        //write_file(fd, cur_file, &file_hdr);
    }

    close_archive(fd);
}

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

        print_hdr(&file_header);

        if ((size = strtol(file_header.ar_size, (char **)NULL, 10)) < 1)
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
    const char *archive;
    const char *files[argc];

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

    archive = argv[optind++];
    if ((flags ^ F_TABLE) == 0) {
        table(archive);
    } else if ((flags ^ F_QUICK) == 0) {
        int i = 0;
        while (optind < argc) {
            files[i++] = argv[optind++];
        }
        if (optind < 4) {
            fprintf(stderr, "At least one file required for appending.\n");
        }
        quick_append(archive, files, i);
    } else {
        fprintf(stderr, "Option not supported yet.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
