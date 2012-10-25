#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 1024
#define BUFNUM 100
#define PROCS 2

/*
 * Continuously read a large file from the beginning of harddrive.
 *
 * Write a medium size file to end of harddrive.
 *
 * Write should starve, because read never stops
 */

void read_file()
{
	int fd;
    char buf[BUFSIZE];
    ssize_t bytes_read;

    fd = open("large_file", O_RDONLY);

    int f = 0;
    while((bytes_read = read(fd, buf, BUFSIZE)) > 0 ) {
        if(f == 0){
            printf("Read %d\n", bytes_read);
            f=1;
        }
    }
    
    if(close(fd) == -1){
        perror("Reading from large_file failed.");
        exit(-1);
    }
}

void write_file()
{
    int fd;
    int buf[BUFSIZE];
    ssize_t buf_size;

	fd = open("medium_file", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    unsigned long i = 0;
    int j = 0;
    // Sliding window of 5 ints 0 .. BUFSIZE*BUFNUM
    while( i < (BUFSIZE)-4 ) {
        // Add to buffer
        for( j = 0; j < 5; j++ ) {
            buf[i+j] = j;
        }
        
        // Get size of 1st element
        buf_size = sizeof(buf[0]);

        // Write buffer
        write(fd, (char *)buf, 5 * buf_size);
        i = i+5;
    }

    if(close(fd) == -1){
        perror("Writing of medium_file failed.");
        exit(-1);
    }
}

int main( int argc, char *argv[] )
{
    int pid, kid;
    printf("Reading and Writing.\n");

    for (kid = 0; kid < PROCS; kid++) {
        pid = fork();
        if (pid == 0)
            break;
    }

    /* First process will read
     * Second process will write
     */
    if(pid == 0) {
        if (kid % 2 == 0){
            printf("I'm a reader! KID: %d\n", kid);
            read_file();
            printf("Done Reading! KID: %d\n", kid);
        } else {
            printf("I'm a writer! KID: %d\n", kid);
            write_file();
            printf("Done writing! KID: %d\n", kid);
        }
    }

    if (pid != 0) {
        for (kid = 0; kid < PROCS; kid++) {
            wait(NULL);
        }
    }
    
    return 0;
}
