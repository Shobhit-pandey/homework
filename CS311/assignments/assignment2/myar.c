/*
 * myar.c
 *
 * Author: Trevor Bramwell
 * Class: CS311
 */
#define _XOPEN_SOURCE 500
#define _BSD_SOURCE

#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <ar.h>
#include "myar.h"

int verbose = FALSE;

/* Utility Functions
 *
 * Note: archive here means the path to the archive file.
 *
 *
 * find_file(fd, file) - if file in archive: return
 *   byte (start) and (end+1); if file not in archive: return -1.
 */

/* file_perm_string used from the file_stat.c example on the class website. */
char *file_perm_string(mode_t perm)
{
    /* Return ls(1)-style string for file permissions mask */
    static char str[STR_SIZE];
    snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
             (perm & S_IRUSR) ? 'r' : '-',
             (perm & S_IWUSR) ? 'w' : '-',
             (perm & S_IXUSR) ? 'x' : '-',
             (perm & S_IRGRP) ? 'r' : '-',
             (perm & S_IWGRP) ? 'w' : '-',
             (perm & S_IXGRP) ? 'x' : '-',
             (perm & S_IROTH) ? 'r' : '-',
             (perm & S_IWOTH) ? 'w' : '-',
             (perm & S_IXOTH) ? 'x' : '-');
    return str;
}

/*
 * Print Header
 *
 * Prints an ar_hdr to stdout.
 */
void print_hdr(struct ar_hdr *hdr)
{
    mode_t perms;
    mode_t *ps = &perms;
    time_t date;
    char name[17];
    long uid;
    long gid;
    long size;

    char date_str[20];

    sscanf(hdr->ar_mode, "%lo", (long *) ps);
    sscanf(hdr->ar_uid, "%ld", &uid);
    sscanf(hdr->ar_gid, "%ld", &gid);
    sscanf(hdr->ar_date, "%ld", &date);
    sscanf(hdr->ar_name, "%16s", name);
    sscanf(hdr->ar_size, "%ld", &size);

    // Remove slash from filename
    char* slash = name;
    while(*slash != '/') slash++;
    *slash = 0;

    strftime(date_str, 20, "%b %d %H:%M %Y", localtime(&date));

    printf("%8s %ld/%ld %6ld %10s %-15s\n",
            file_perm_string(perms),
            uid,
            gid,
            size,
            date_str,
            name);
}

/*
 * Open filename and return a file descriptor
 */
int open_file(const char * filename)
{
    int fd;
    if((fd = open(filename, O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/*
 * Format Filename
 *
 * Given a commandline filename, format it to the ar specs.
 */
void fmt_filename(const char *file, char *filename)
{
    // look into strcat
    // s1 = sprintf "%s" file
    // s2 = "/"
    // return strcat, format in another function
    int i = 0;
    while(i < strlen(file)) {
        filename[i] = file[i];
        i++;
    }
    filename[i++] = '/';
    while(i < AR_FILELEN) {
        filename[i++] = ' ';
    }
}

/*
 * Build Header
 *
 * Constructs the ar header for a file.
 *
 * Takes the file metadata from stat and puts it into an ar_hdr
 *   struct. Returns the preferred blksize for writing the file.
 */
void build_hdr(const char *file, const struct stat *st, struct ar_hdr *hdr)
{
    char filename[AR_FILELEN];

    fmt_filename(file, filename);

    sprintf(hdr->ar_name, "%-16s", filename);
    sprintf(hdr->ar_date, "%-12ld", (long) st->st_mtime);
    sprintf(hdr->ar_uid, "%-6ld", (long) st->st_uid);
    sprintf(hdr->ar_gid, "%-6ld", (long) st->st_gid);
    sprintf(hdr->ar_mode, "%-8lo", (unsigned long) st->st_mode);
    sprintf(hdr->ar_size, "%-10lld", (long long) st->st_size);
    memcpy(hdr->ar_fmag, ARFMAG, 2);
}

/*
 * Write Header
 *
 * Writes a ar_hdr to the file descriptor
 */
void write_hdr(int ar, struct ar_hdr *hdr)
{
    if (write(ar, hdr, AR_STRUCT_SIZE) != AR_STRUCT_SIZE) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

/*
 * Write File
 *
 * Write a file to another while keeping even offsets.
 */
void write_file(int ar, int fd, struct stat *st)
{
    ssize_t b_read;
    long blk_size = (long) st->st_blksize;
    long file_size = (long) st->st_size;
    char* buf[blk_size];

    /* read chars from filename into a buffer */
    while((b_read = read(fd, buf, blk_size)) != 0) {
        if(b_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* write chars from a buffer to the archive */
        if((write(ar, buf, b_read)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    /* Add a newline if odd length file. */
    if((file_size % 2) == 1) {
        if((write(ar, "\n", 1)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
}

/*
 * Stat File
 *
 * Stats a file and checks that it completed successfully.
 *
 */
void stat_file(const char *filename, struct stat *st)
{
    if (stat(filename, st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
}

/*
 * Append File
 *
 * Given an already open archive file, and a file to append,
 * build a ar_hdr, and append it, along with the file to the archive.
 *
 */
void append_file(int archive, const char *filename)
{
    int fd;
    struct stat st;
    struct ar_hdr file_hdr;

    fd = open_file(filename);

    stat_file(filename, &st);
    build_hdr(filename, &st, &file_hdr);
    write_hdr(archive, &file_hdr);
    write_file(archive, fd, &st);

    close(fd);
}

/*
 * given a ar filename, return a fd for the ar.
 *   Check to ensure fd is valid, and use perror to output failure. Also
 *   contains an optional argument to create the archive if it doesn't
 *   exist.
 *   - This may return a struct containing the file and stat of file...
 *     operation will most likely need the size...
 */
int open_archive(const char *archive)
{
    int fd;
    char armag[SARMAG];

    if ((fd = open(archive, O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (read(fd, armag, SARMAG) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (memcmp(armag, ARMAG, SARMAG) == -1) {
        fprintf(stderr, "Aborting: Not a valid archive file.\n");
        exit(EXIT_FAILURE);
    }

    return fd;
}

/*
 * close_archive(fd) - closes the archive at the given fd, checks result
 *   to make sure no errors given. If errors, perror and fail
 */
void close_archive(int fd)
{
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
void append_all(const char *archive)
{
    char **files;
    DIR *dir;
    struct dirent *ed;
    int nfiles = 0;

    char *dirname = ".";

    if ((dir = opendir(dirname)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    files = (char **) malloc( sizeof(char *) * 50);

    while ((ed = readdir(dir)) != NULL) {
        if(ed->d_type == DT_REG && strcmp(ed->d_name, archive) != 0) {
            files[nfiles++] = ed->d_name;
        }
    }

    closedir(dir);
    
    quick_append(archive, (const char **)files, nfiles);
    free(files);
}

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
 */
void quick_append(const char *archive, const char *files[], int num_files)
{
    int fd;

    if ((fd = open(archive, O_WRONLY | O_APPEND)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_files; i++) {
        append_file(fd, files[i]);
    }

    close_archive(fd);
}

/*
 * Print Header Name
 *
 * Prints the ar_name of a ar_hdr.
 */
void print_hdr_name(struct ar_hdr *file_header)
{
    int i = 0;
    while(file_header->ar_name[i] != '/') {
        ++i;
    }
    printf("%.*s\n", i, file_header->ar_name);
}

/*
 * Table
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
void table(const char *archive)
{
    int fd;
    int size;
    ssize_t buf;
    struct ar_hdr file_header;

    fd = open_archive(archive);

    while((buf = read(fd, &file_header, AR_STRUCT_SIZE)) == AR_STRUCT_SIZE) {
        size = 0;

        if (buf == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (memcmp(file_header.ar_fmag, ARFMAG, 2) != 0) {
            fprintf(stderr, "Bad read.\n");
            exit(EXIT_FAILURE);
        }

        if(verbose == TRUE) {
            print_hdr(&file_header);
        } else {
            print_hdr_name(&file_header);
        }

        if ((size = strtol(file_header.ar_size, (char **)NULL, 10)) == LONG_MIN
                || size == LONG_MAX)
        {
            perror("strtol");
            exit(EXIT_FAILURE);
        }

        /* Compensate for '\n' when the file length is even */
        if ((size % 2) == 1) {
            size++;
        }

        if (lseek(fd, size, SEEK_CUR) == -1) {
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
 * prints 'd - Filename' in verbose mode 
 */
void delete(const char *archive, const char* files[], int num_files) {
    ;
}

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
 *
 * prints 'x - Filename' in verbose mode
 */
void extract(const char *archive, const char* files[], int num_files) {
    ;
}

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

    archive = argv[optind++];
    if ((flags ^ F_TABLE) == 0) {
        table(archive);
    } else if ((flags ^ F_APPEND) == 0) {
        append_all(archive);
    } else {
        int i = 0;
        while (optind < argc) {
            files[i++] = argv[optind++];
        }

        if (optind < 4) {
            fprintf(stderr, "At least one file required for appending, "
                            "extracting or deleting.\n");
            exit(EXIT_FAILURE);
        }

        if ((flags ^ F_QUICK) == 0) {
            quick_append(archive, files, i);
        } else if ((flags ^ F_XTRACT) == 0) {
            extract(archive, files, i);
        } else {
            fprintf(stderr, "Option not supported yet.\n");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
