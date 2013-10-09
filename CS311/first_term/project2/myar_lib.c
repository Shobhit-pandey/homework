/*
 * Original Author: Trevor Bramwell (bramwelt)
 * File: bramwelt_proj2.h
 * Created: 2011 October 30, 08:16 by bramwelt
 * Last Modified: 2011 October 30, 00:00 by bramwelt
 * 
 * This file contains functions related to project 2.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> /* file I/O */
#include <fcntl.h>
#include <ar.h>
#include <errno.h>

#define __BSD_SOURCE 1
#include <sys/types.h>
#include <sys/stat.h>
#include <langinfo.h>
#include <time.h>
#include <stdint.h>

#include "myar_lib.h"

int create_archive(int fd) {
    ssize_t wbytes;
    if((wbytes = myar_write(fd, ARMAG, SARMAG)) != FALSE) {
        return TRUE;
    }
    return FALSE;
}

int file_length(int fd) {
    struct stat file_st;
    int status;

    /* If error don't worry about it */
    if((status = myar_fstat(fd, &file_st)) == FALSE)
        return status; /* -1, errno set */
    
    return file_st.st_size;
}

int is_archive(int fd) {
    ssize_t rbytes;
    char buf[SARMAG]; /* SARMAG is length of "!<arch>" string */

    if((rbytes = myar_read(fd, buf, SARMAG)) != FALSE) {
        if(strcmp(buf, ARMAG) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void print_ar_hdr(struct ar_hdr* file_hdr) {
    printf("%-16s%-12s%-6s%-6s%-8s%-10s%2s",
        file_hdr->ar_name,
        file_hdr->ar_date,
        file_hdr->ar_uid,
        file_hdr->ar_gid,
        file_hdr->ar_mode,
        file_hdr->ar_size,
        file_hdr->ar_fmag
        ); 
}

int append_file(int archive_fd, char *filename) {
    struct ar_hdr file_hdr;

    get_ar_hdr(filename, &file_hdr);
    #if DEBUG
    print_ar_hdr(&file_hdr);
    #endif
    //write_hdr(archive_fd, &file_hdr);
    //write_file(archive_fd, filename);

    return 0;
}

void get_ar_hdr(char *filename, struct ar_hdr *hdr) {
    struct stat file_st;
    int filename_len;
    int file_fd;
    errno = 0;

    if((file_fd = myar_open(filename, O_RDONLY)) == FALSE)
        return FALSE;

    myar_fstat(file_fd, &file_st);

    print_file_stat(&file_st);
    filename_len = strlen(filename); /*strlen doesn't include \0*/

    if(filename_len > 16 ) {
        fprintf(stderr, "Filename too long:%s\n", filename);
        exit(EXIT_FAILURE);
    } else {
        /* first header now has: */
        /* ar_name = '//' */
        /* ar_size - the size of the filename+2 for '\n\n' 
             this is number of bytes directly after _this_ header that hold
             the filename */
        /* second header */
        /* hold file info, but ar_name = '/0'*/
        snprintf(hdr->ar_name, 16, "%s/", filename);
    }
    snprintf(hdr->ar_date, 12, "%ju", (uintmax_t)file_st.st_mtime);
    snprintf(hdr->ar_uid, 6, "%d", file_st.st_uid);
    snprintf(hdr->ar_gid, 6, "%d", file_st.st_gid);
    snprintf(hdr->ar_mode, 8, "%o", file_st.st_mode);
    snprintf(hdr->ar_size, 8, "%lld", (long long)file_st.st_size);
    sprintf(hdr->ar_fmag, ARFMAG);

    close(file_fd);
}

char *sperm(mode_t perm) {
    static char str[STR_SIZE];
    char file_type;

    if((S_ISREG(perm)) == 1) {
        file_type = '-';
    } else if(S_ISBLK(perm) == 1) {
        file_type = 'b';
    } else if((S_ISCHR(perm)) == 1) {
        file_type = 'c';
    } else if((S_ISDIR(perm)) == 1) {
        file_type = 'd';
    } else if((S_ISFIFO(perm)) == 1) {
        file_type = 'f';
    } else if((S_ISLNK(perm)) == 1) {
        file_type = 'l';
    } else {
        file_type = '?';
    }
    
    snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c%c",
        file_type,
        (perm & S_IRUSR) ? 'r' : '-',
        (perm & S_IWUSR) ? 'w' : '-',
            ((perm & S_IXUSR) && (perm & S_ISUID) ? 'x' : '-'),
        (perm & S_IRGRP) ? 'r' : '-',
        (perm & S_IWGRP) ? 'w' : '-',
            ((perm & S_IXGRP) && (perm & S_ISGID) ? 'x' : '-'),
        (perm & S_IROTH) ? 'r' : '-',
        (perm & S_IWOTH) ? 'w' : '-',
            ((perm & S_IXOTH) ? 'x' : '-')
        );

    return str;
}
    
/* Function taken from book */
void print_file_stat(struct stat *statbuf) {
    char datestring[256];
    struct tm *tm;
    /* Print out type, permissions, and number of links. */
    printf("%10.10s", sperm(statbuf->st_mode));
    printf("%4d", statbuf->st_nlink);

    /* Print out owner's name if it is found using getpwuid(). */
    printf(" %-8d", statbuf->st_uid);

    /* Print out group name if it is found using getgrgid(). */
    printf(" %-8d", statbuf->st_gid);

    /* Print size of file. */
    printf(" %9jd", (intmax_t)statbuf->st_size);

    tm = localtime(&statbuf->st_mtime);

    /* Get localized date string. */
    strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

    printf(" %s\n", datestring);
}

ssize_t myar_read(int fd, void *buf, size_t count) {
    ssize_t read_size;
    errno = 0;

    read_size = read(fd, buf, count);

    if(read_size == FALSE) {
        fprintf(stderr, "Error: %d\n", stererrno(errno));
        return FALSE;
    }

    return read_size;
}

ssize_t myar_write(int fd, const void *buf, size_t count) {
    ssize_t write_size;
    errno = 0;

    write_size = write(fd, buf, count);

    if(write_size == FALSE)
        fprintf(stderr, "Error: %d\n", stererrno(errno));
        return FALSE;

    return write_size;
}

int myar_fstat(int fd, struct stat *buf) {
    int stat_val;
    errno = 0;
    
    stat_val = fstat(fd, buf);

    if(stat_val == FALSE)
        fprintf(stderr, "Error: %d\n", stererrno(errno));
        return FALSE;

    return stat_val;
}

int myar_open(const char *pathname, int flags, mode_t mode) {
    int open_ret;
    errno = 0;

    open_ret = open(pathname, flags, mode);

    if(open_ret == FALSE)
        fprintf(stderr, "Error: %d\n", stererrno(errno));
        return FALSE;

    return open_ret;
}
