#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

void errExit(const char *func) {
    perror(func);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int pfd[2];
    if (pipe(pfd) == -1)
        errExit("pipe");
    /* Pipe file descriptors */
    /* Create pipe */
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:
            /* First child: exec 'ls' to write to pipe */
            if (close(pfd[READ]) == -1)
                /* Read end is unused */
                errExit("close 1");
                /* Duplicate stdout on write end of pipe; close
                 * duplicated descriptor */
                if (pfd[WRITE] != STDOUT_FILENO) { /* Defensive check */
                    if (dup2(pfd[WRITE], STDOUT_FILENO) == -1) 
                        errExit("dup2 1"); 
                    if (close(pfd[WRITE]) == -1) 
                        errExit("close 2"); 
                } 
            execlp("ls", "ls", (char *) NULL); /* Writes to pipe */
            errExit("execlp ls"); 
        default:
            break;
    }
    /* Parent falls through to create next child */
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:
            /* Second child: exec 'wc' to read from pipe */
            if (close(pfd[WRITE]) == -1)
                /* Write end is unused */
                errExit("close 3");
            /* Duplicate stdin on read end of pipe; close duplicated
             * descriptor */
            if (pfd[READ] != STDIN_FILENO) { /* Defensive check */
                if (dup2(pfd[READ], STDIN_FILENO) == -1) 
                    errExit("dup2 2"); 
                if (close(pfd[READ]) == -1) 
                    errExit("close 4"); 
            } 
            execlp("sort", "sort", (char *) NULL); /* Reads from pipe */
            errExit("execlp"); 
        default:
            break;
    }
    /* Parent falls through */
    /* Parent closes unused file descriptors for pipe, and waits for
     * children */
    if (close(pfd[READ]) == -1)
        errExit("close 5");
    if (close(pfd[WRITE]) == -1)
        errExit("close 6");
    if (wait(NULL) == -1)
        errExit("wait 1");
    if (wait(NULL) == -1)
        errExit("wait 2");
    exit(EXIT_SUCCESS);
}
