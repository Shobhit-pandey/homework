/*
 * Original Author: Trevor Bramwell (bramwelt)
 * File: bramwelt_proj2.h
 * Created: 2011 October 30, 08:16 by bramwelt
 * Last Modified: 2011 October 30, 00:00 by bramwelt
 * 
 * This file contains functions related to project 2.
 */
#ifndef __MYAR_LIB_
#define __MYAR_LIB_

#include <sys/stat.h>
#include <ar.h>

#define AR_Q 1
#define AR_X 2
#define AR_T 4
#define AR_V 8
#define AR_D 16
#define AR_A 32
#define AR_H 64

#ifndef ENOTAR
#define ENOTAR 542
#endif

#define TRUE 0
#define FALSE -1

#define RADIX 10

#define AR_MAX_CHAR_I 14 /* maximum index the ar_name char should reach */
#define STR_SIZE sizeof("-rwxrwxrwx")

ssize_t myar_read(int fd, void *buf, size_t count);
ssize_t myar_write(int fd, const void *buf, size_t count);
int myar_fstat(int fd, struct stat *buf);
int myar_open(const char *pathname, int flags);
int myar_open(const char *pathname, int flags, mode_t mode);
void parse_ar_hdr(struct stat *statbuf, struct ar_hdr *hdr);

int append_file(int archive_fd, char *filename);
ssize_t create_archive(int fd); 
void print_archive(int fd, int verbose);
void print_ar_hdr(struct ar_hdr *file_hdr);
void print_file_stat(struct stat *statbuf);
void get_ar_hdr(char *filename, struct ar_hdr *hdr);
int is_archive(int fd);
int file_length(int fd);
int extract_archive(int fd, char *filelist[], int filelist_len);
int delete_files(int fd, char *filelist[], int filelist_len);
#endif
